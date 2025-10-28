/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "main_frame.h"

#include "app.h"
#include "button.h"
#include "column_graphic.h"
#include "element.h"
#include "frame.h"
#include "geometry.h"
#include "input.h"
#include "layout.h"
#include "logger.h"
#include "list.h"
#include "utils.h"
#include "color.h"
#include "container.h"
#include "input_box.h"
#include "resource_manager.h"
#include "style.h"
#include "text.h"

static void MainFrame_addElements(MainFrame* self, App* app);

static void MainFrame_onEscape(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneS(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneP(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneM(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneQ(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onEnter(Input* input, SDL_Event* evt, MainFrame* self);

static bool MainFrame_createPopup(MainFrame* self, float value);

static bool MainFrame_removePopup(MainFrame* self, float value);

static void MainFrame_quitApp(Input* input, SDL_Event* evt, Button* button);

MainFrame* MainFrame_new(App* app) {
    MainFrame* self = calloc(1, sizeof(MainFrame));
    if (!self) {
        error("Failed to allocate memory for MainFrame");
        return NULL;
    }

    self->app = app;
    self->elements = List_create();
    if (!self->elements) {
        error("Failed to create elements list for MainFrame");
        safe_free((void **) &self);
        return NULL;
    }

    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);

    self->graph_count = 1;
    self->graph = calloc(self->graph_count, sizeof(ColumnGraph *));
    if (!self->graph) {
        error("Failed to allocate memory for ColumnGraph array in MainFrame");
        MainFrame_destroy(self);
        return NULL;
    }
    self->bar_count = 50;
    for (int i = 0; i < self->graph_count; i++) {
        self->graph[i] = ColumnGraph_new(w, h, Position_new(0, 0), app->input, self,
                                         (ColumnsHoverFunc) MainFrame_createPopup,
                                         (ColumnsHoverFunc) MainFrame_removePopup);
        ColumnGraph_initBars(self->graph[i], self->bar_count);
    }

    self->popup = NULL;

    MainFrame_addElements(self, app);

    return self;
}

static void MainFrame_addElements(MainFrame* self, App* app) {
    ListIterator* it = ListIterator_new(self->elements);
    while (ListIterator_hasNext(it)) {
        Element* element = ListIterator_next(it);
        if (String_equals(element->id, "settings")) {
            Element_destroy(element);
        }
    }
    ListIterator_destroy(it);
    List_clear(self->elements);
    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);

    for (int i = 0; i < self->graph_count; i++) {
        ColumnGraph* graph = self->graph[i];
        ListIterator* graphIt = ListIterator_new(graph->bars);
        while (ListIterator_hasNext(graphIt)) {
            ColumnGraphBar* bar = ListIterator_next(graphIt);
            List_push(self->elements, bar->element);
        }
        ListIterator_destroy(graphIt);
        FlexContainer_layout(graph->container);
    }

    float settingsWidth = 200;
    Container* container = Container_new(w, 0, settingsWidth, h, Color_rgba(0, 0, 0, 200), self);
    Container_addChild(container, Element_fromText(Text_new(app->renderer,
                                                            TextStyle_new(
                                                                ResourceManager_getDefaultBoldFont(app->manager, 36),
                                                                36, COLOR_WHITE, TTF_STYLE_UNDERLINE),
                                                            Position_new(w + 10, 10),
                                                            false,
                                                            "Settings"
                                                   ), NULL));

    InputBox* inputBar = InputBox_new(self->app,
        SDL_CreateRect(w + 2, 50, settingsWidth - 24, 40, false), InputBoxStyle_default(self->app->manager),
        container);
    InputBox_setStringf(inputBar, "%d", self->bar_count);

    InputBox* inputGraph = InputBox_new(self->app,
        SDL_CreateRect(w + 2, 100, settingsWidth - 24, 40, false), InputBoxStyle_default(self->app->manager),
        container);
    InputBox_setStringf(inputGraph, "%d", self->graph_count);

    Container_addChild(container, Element_fromInput(inputBar, "inputBar"));
    Container_addChild(container, Element_fromInput(inputGraph, "inputGraph"));

    Button* closeButton = Button_new(self->app,
                               Position_new(w, h - 75),
                               false,
                               ButtonStyle_default(self->app->manager),
                               container,
                               "Close"
        );
    Button_onClick(closeButton, (EventHandlerFunc) MainFrame_quitApp);
    Container_addChild(container, Element_fromButton(closeButton, NULL));

    List_push(self->elements, Element_fromContainer(container, "settings"));
}

void MainFrame_destroy(MainFrame* self) {
    if (!self) return;

    Element_destroyList(self->elements);

    safe_free((void **) &self);
}

void MainFrame_render(SDL_Renderer* renderer, MainFrame* self) {
    Element_renderList(self->elements, renderer);

    if (self->popup) {
        Container_render(self->popup, renderer);
    }
}

void MainFrame_update(MainFrame* self) {
    if (self->box_animating) {
        self->box_anim_progress += 0.03f;
        if (self->box_anim_progress >= 1.f) {
            self->box_animating = false;
            self->box_anim_progress = 1.f;
        }
        Element* settings = Element_getById(self->elements, "settings");
        Container* container = settings->data.container;
        container->box->position->x = self->box_start_x + (self->box_target_x - self->box_start_x) * self->
                                      box_anim_progress;
        ListIterator* it = ListIterator_new(container->children);
        while (ListIterator_hasNext(it)) {
            Element* child = ListIterator_next(it);
            float x, y;
            Element_getPosition(child, &x, &y);
            x = self->box_start_x + (self->box_target_x - self->box_start_x) * self->box_anim_progress + 10;
            Element_setPosition(child, x, y);
        }
    }

    Element_updateList(self->elements);
}

void MainFrame_focus(MainFrame* self) {
    Element_focusList(self->elements);

    Input_addKeyEventHandler(self->app->input, SDLK_ESCAPE, (EventHandlerFunc) MainFrame_onEscape, self);
    Input_addKeyEventHandler(self->app->input, SDLK_S, (EventHandlerFunc) MainFrame_onRuneS, self);
    Input_addKeyEventHandler(self->app->input, SDLK_P, (EventHandlerFunc) MainFrame_onRuneP, self);
    Input_addKeyEventHandler(self->app->input, SDLK_M, (EventHandlerFunc) MainFrame_onRuneM, self);
    Input_addKeyEventHandler(self->app->input, SDLK_Q, (EventHandlerFunc) MainFrame_onRuneQ, self);
    Input_addKeyEventHandler(self->app->input, SDLK_RETURN, (EventHandlerFunc) MainFrame_onEnter, self);
}

void MainFrame_unfocus(MainFrame* self) {
    Element_unfocusList(self->elements);

    Input_removeOneKeyEventHandler(self->app->input, SDLK_ESCAPE, self);
    Input_removeOneKeyEventHandler(self->app->input, SDLK_S, self);
    Input_removeOneKeyEventHandler(self->app->input, SDLK_P, self);
    Input_removeOneKeyEventHandler(self->app->input, SDLK_M, self);
    Input_removeOneKeyEventHandler(self->app->input, SDLK_Q, self);
    Input_removeOneKeyEventHandler(self->app->input, SDLK_RETURN, self);
}

Frame* MainFrame_getFrame(MainFrame* self) {
    Frame* frame = Frame_new(self,
                             (FrameRenderFunc) MainFrame_render,
                             (FrameUpdateFunc) MainFrame_update,
                             (FrameFocusFunc) MainFrame_focus,
                             (FrameFocusFunc) MainFrame_unfocus,
                             (DestroyFunc) MainFrame_destroy);
    Frame_setTitle(frame, "MainFrame");
    return frame;
}

static void MainFrame_onEscape(Input* input, SDL_Event* evt, MainFrame* self) {
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    self->box_animating = true;
    self->showSettings = !self->showSettings;
    self->box_anim_progress = 0.f;
    Element* settings = Element_getById(self->elements, "settings");
    Container* container = settings->data.container;
    self->box_start_x = container->box->position->x;
    self->box_target_x = self->showSettings ? w - 200 : w;
    if (self->showSettings && self->popup) {
        for (int i = 0; i < self->graph_count; i++) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
    }
}

static void MainFrame_onRuneS(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings) return;
    bool hasPopup = self->popup != NULL;
    for (int i = 0; i < self->graph_count; i++) {
        if (hasPopup) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
        ColumnGraph_shuffleBars(self->graph[i]);
    }
    MainFrame_addElements(self, self->app);
}

static bool MainFrame_createPopup(MainFrame* self, float value) {
    if (!self || self->popup || self->showSettings) return false;
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    float x, y;
    Input_getMousePosition(self->app->input, &x, &y);
    float popupX = x - 50 < 0 ? 0 : x - 50 + 120 > w ? w - 120 : x - 50;
    float popupY = y - 50 < 0 ? 0 : y - 50 + 40 > h ? h - 40 : y - 50;
    self->popup = Container_new(popupX, popupY, 120, 40, Color_copy(COLOR_BLACK), self);
    Container_addChild(self->popup, Element_fromText(Text_newf(self->app->renderer,
                                                               TextStyle_new(
                                                                   ResourceManager_getDefaultBoldFont(
                                                                       self->app->manager, 20), 20, COLOR_WHITE,
                                                                   TTF_STYLE_NORMAL),
                                                               Position_new(popupX, popupY),
                                                               false,
                                                               "Value : %d", (int) value
                                                     ), NULL));

    return true;
}

static bool MainFrame_removePopup(MainFrame* self, float _) {
    if (!self) return false;
    if (self->popup) {
        Container_destroy(self->popup);
        self->popup = NULL;
    }
    return true;
}

static void MainFrame_updateGraphs(MainFrame* self) {
    self->graph = realloc(self->graph, self->graph_count * sizeof(ColumnGraph *));
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    for (int i = 0; i < self->graph_count; i++) {
        float graphs = self->graph_count % 2 == 0 ? self->graph_count : self->graph_count + 1;
        float width = self->graph_count == 1 ? w : w / 2;
        float height = h / (graphs / 2);
        float x = (i % 2) * width;
        float y = (i / 2) * height;
        self->graph[i] = ColumnGraph_new(width, height, Position_new(x, y), self->app->input, self,
                                         (ColumnsHoverFunc) MainFrame_createPopup,
                                         (ColumnsHoverFunc) MainFrame_removePopup);
        ColumnGraph_initBars(self->graph[i], self->bar_count);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onRuneP(Input* input, SDL_Event* evt, MainFrame* self) {
    self->graph_count++;
    for (int i = 0; i < self->graph_count - 1; i++) {
        ColumnGraph_destroy(self->graph[i]);
    }
    MainFrame_updateGraphs(self);
}

static void MainFrame_onRuneM(Input* input, SDL_Event* evt, MainFrame* self) {
    if (self->graph_count > 1) {
        self->graph_count--;
        for (int i = 0; i < self->graph_count + 1; i++) {
            ColumnGraph_destroy(self->graph[i]);
        }
        MainFrame_updateGraphs(self);
    }
}

static void MainFrame_quitApp(Input* input, SDL_Event* evt, Button* button) {
    if (button && button->parent) {
        Container* parent = button->parent;
        if (parent->parent) {
            MainFrame* mainFrame = parent->parent;
            mainFrame->app->running = false;
        }
    }
}

static void MainFrame_onRuneQ(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings) return;
    bool hasPopup = self->popup != NULL;
    for (int i = 0; i < self->graph_count; i++) {
        if (hasPopup) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
        ColumnGraph_sortGraph(self->graph[i], LIST_SORT_TYPE_QUICK);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onEnter(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || !self->showSettings) return;
    Container* container = Element_getById(self->elements, "settings")->data.container;

    InputBox* inputBar = Element_getById(container->children, "inputBar")->data.input_box;
    InputBox* inputGraph = Element_getById(container->children, "inputGraph")->data.input_box;
    if (!String_isNumeric(InputBox_getString(inputBar)) || ! String_isNumeric(InputBox_getString(inputGraph))) {
        return;
    }
    int barCount = atoi(InputBox_getString(inputBar));
    int graphCount = atoi(InputBox_getString(inputGraph));
    if (barCount <= 0 || graphCount <= 0) {
        return;
    }
    self->bar_count = barCount;
    self->showSettings = false;
    for (int i = 0; i < self->graph_count; i++) {
        ColumnGraph_destroy(self->graph[i]);
    }
    self->graph_count = graphCount;
    MainFrame_updateGraphs(self);
}