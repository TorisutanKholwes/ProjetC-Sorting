/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "main_frame.h"

#include "app.h"
#include "button.h"
#include "column_graph.h"
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
#include "help_frame.h"
#include "image.h"
#include "input_box.h"
#include "resource_manager.h"
#include "select.h"
#include "style.h"
#include "text.h"
#include "tinyfiledialogs.h"

static void MainFrame_addElements(MainFrame* self, App* app);

static void MainFrame_onEscape(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneS(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneP(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneM(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneQ(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_DelaySort(MainFrame* self, ColumnGraph* graph, ColumnGraphBar* actual);

static void MainFrame_onEnter(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onClick(Input* input, SDL_Event* evt, MainFrame* self);

static bool MainFrame_createPopup(MainFrame* self, void* value, ColumnGraphType type);

static bool MainFrame_removePopup(MainFrame* self, void* value, ColumnGraphType type);

static void MainFrame_quitApp(Input* input, SDL_Event* evt, Button* button);

static void MainFrame_loadFile(Input* input, SDL_Event* evt, Button* button);

static void MainFrame_onRuneA(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onGraphThemeChange(Input* input, SDL_Event* evt, Select* select);

static void MainFrame_onMouseMove(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_updateHelpImage(MainFrame* self);

static void MainFrame_onWindowResize(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_onRuneI(Input* input, SDL_Event* evt, MainFrame* self);

static void MainFrame_showGraphInfo(MainFrame* self, int index, ColumnGraph* graph);

static void MainFrame_hideGraphInfo(MainFrame* self);

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
    self->selected_graph_index = 0;
    self->graph_style = GRAPH_RAINBOW;
    for (int i = 0; i < self->graph_count; i++) {
        self->graph[i] = ColumnGraph_new(w, h, Position_new(0, 0), app->input, self, GRAPH_TYPE_INT,
                                         (ColumnsHoverFunc) MainFrame_createPopup,
                                         (ColumnsHoverFunc) MainFrame_removePopup);
        ColumnGraph_initBarsIncrement(self->graph[i], self->bar_count, self->graph_style);
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

    self->settings_width = 300;
    float baseWidth = w;
    if (self->showSettings) {
        baseWidth -= self->settings_width;
    }
    int y = 10;

    Container* container = Container_new(baseWidth, 0, self->settings_width, h, false, Color_rgba(0, 0, 0, 200), self);
    Text* titleText = Text_new(app->renderer,
                               TextStyle_new(
                                   ResourceManager_getDefaultBoldFont(app->manager, 36),
                                   36, COLOR_WHITE, TTF_STYLE_UNDERLINE),
                               Position_new(baseWidth + 10, 10),
                               false,
                               "Settings"
    );
    Container_addChild(container, Element_fromText(titleText, NULL));
    y += Text_getSize(titleText).height + 20;

    // Input change bar count
    Text* barText = Text_new(app->renderer,
                             TextStyle_new(
                                 ResourceManager_getDefaultBoldFont(app->manager, 24), 24, COLOR_WHITE,
                                 TTF_STYLE_NORMAL),
                             Position_new(baseWidth + 10, y), false, "Bar Count:");

    y += Text_getSize(barText).height + 10;

    int inputHeight = 40;
    InputBox* inputBar = InputBox_new(self->app,
                                      SDL_CreateRect(baseWidth + 10, y, self->settings_width - 24, inputHeight, false),
                                      InputBoxStyle_default(self->app->manager),
                                      container);
    InputBox_setStringf(inputBar, "%d", self->bar_count);
    y += inputHeight + 40;

    Container_addChild(container, Element_fromText(barText, NULL));
    Container_addChild(container, Element_fromInput(inputBar, "inputBar"));

    // Input change graph count
    Text* graphText = Text_new(app->renderer,
                               TextStyle_new(
                                   ResourceManager_getDefaultBoldFont(app->manager, 24), 24, COLOR_WHITE,
                                   TTF_STYLE_NORMAL),
                               Position_new(baseWidth + 10, y), false, "Graph Count:");
    y += Text_getSize(graphText).height + 10;

    InputBox* inputGraph = InputBox_new(self->app,
                                        SDL_CreateRect(baseWidth + 10, y, self->settings_width - 24, inputHeight,
                                                       false),
                                        InputBoxStyle_default(self->app->manager),
                                        container);
    InputBox_setStringf(inputGraph, "%d", self->graph_count);
    y += inputHeight + 20;

    Container_addChild(container, Element_fromText(graphText, NULL));
    Container_addChild(container, Element_fromInput(inputGraph, "inputGraph"));

    List* options = List_create();
    for (int i = 0; i < GRAPH_STYLE_COUNT; i++) {
        List_push(options, ColumnGraph_getStyleName(i));
    }
    Select* select = Select_new(app, Position_new(baseWidth, y), false,
                                SelectStyle_default(self->app->manager), container,
                                options, 0);
    int selectWidth = Select_getSize(select).width;
    Select_setPosition(select, baseWidth + ((self->settings_width - selectWidth) / 2), y);
    Select_setSelectedIndex(select, self->graph_style);
    Select_onChange(select, (EventHandlerFunc) MainFrame_onGraphThemeChange);
    Container_addChild(container, Element_fromSelect(select, NULL));

    //y += select->rect.h + 40;

    float buttonXOffset = 20;
    Button* closeButton = Button_new(self->app,
                                     Position_new(baseWidth + buttonXOffset, h - 75),
                                     false,
                                     ButtonStyle_default(self->app->manager),
                                     container,
                                     "Quit"
    );
    Button_onClick(closeButton, (EventHandlerFunc) MainFrame_quitApp);
    Container_addChild(container, Element_fromButton(closeButton, NULL));

    Size closeSize = Button_getSize(closeButton);
    Button* loadButton = Button_new(self->app,
                                    Position_new(baseWidth + closeSize.width + 15 + buttonXOffset, h - 75),
                                    false,
                                    ButtonStyle_default(self->app->manager),
                                    container,
                                    "Load file");
    Button_onClick(loadButton, (EventHandlerFunc) MainFrame_loadFile);
    Container_addChild(container, Element_fromButton(loadButton, NULL));

    List_push(self->elements, Element_fromContainer(container, "settings"));

    if (!self->all_selected) {
        for (int i = 0; i < self->graph_count; i++) {
            if (i == self->selected_graph_index) continue;
            ColumnGraph* graph = self->graph[i];
            Box* box = Box_new(graph->size.width, graph->size.height, 0,
                               Position_new(graph->position->x, graph->position->y),
                               Color_rgba(0, 0, 0, 75), NULL, false);
            List_push(self->elements, Element_fromBox(box, NULL));
        }
    }
    Image* help_image = Image_load(self->app , "help_white.svg", Position_new(0, 0), false);
    Image_setRatio(help_image, 0.05);

    List_push(self->elements, Element_fromImage(help_image, "help_image"));

    if (self->graph_info) {
        List_push(self->elements, Element_fromContainer(self->graph_info, "graph_info"));
    }
}

void MainFrame_destroy(MainFrame* self) {
    if (!self) return;

    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_ESCAPE, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_S, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_P, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_SEMICOLON, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_A, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_RETURN, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_Q, self);

    Input_removeOneEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, self);
    Input_removeOneEventHandler(self->app->input, SDL_MOUSEMOTION, self);
    Input_removeOneEventHandler(self->app->input, SDL_WINDOWEVENT, self);

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
        float xPosBefore = container->box->position->x;
        container->box->position->x = self->box_start_x + (self->box_target_x - self->box_start_x) * self->
                                      box_anim_progress;
        ListIterator* it = ListIterator_new(container->children);
        while (ListIterator_hasNext(it)) {
            Element* child = ListIterator_next(it);
            float x, y;
            Element_getPosition(child, &x, &y);
            float diff = container->box->position->x - xPosBefore;
            x += diff;
            Element_setPosition(child, x, y);
        }
    }

    Element_updateList(self->elements);
}

void MainFrame_focus(MainFrame* self) {
    Element_focusList(self->elements);

    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_ESCAPE, (EventHandlerFunc) MainFrame_onEscape, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_S, (EventHandlerFunc) MainFrame_onRuneS, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_P, (EventHandlerFunc) MainFrame_onRuneP, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_SEMICOLON, (EventHandlerFunc) MainFrame_onRuneM, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_A, (EventHandlerFunc) MainFrame_onRuneQ, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_RETURN, (EventHandlerFunc) MainFrame_onEnter, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_Q, (EventHandlerFunc) MainFrame_onRuneA, self);
    Input_addKeyEventHandler(self->app->input, SDL_SCANCODE_I, (EventHandlerFunc) MainFrame_onRuneI, self);

    Input_addEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, (EventHandlerFunc) MainFrame_onClick, self);
    Input_addEventHandler(self->app->input, SDL_MOUSEMOTION, (EventHandlerFunc) MainFrame_onMouseMove, self);
    Input_addEventHandler(self->app->input, SDL_WINDOWEVENT, (EventHandlerFunc) MainFrame_onWindowResize, self);
}

void MainFrame_unfocus(MainFrame* self) {
    Element_unfocusList(self->elements);

    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_ESCAPE, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_S, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_P, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_SEMICOLON, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_A, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_RETURN, self);
    Input_removeOneKeyEventHandler(self->app->input, SDL_SCANCODE_Q, self);

    Input_removeOneEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, self);
    Input_removeOneEventHandler(self->app->input, SDL_MOUSEMOTION, self);
    Input_removeOneEventHandler(self->app->input, SDL_WINDOWEVENT, self);
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
    if (!self || self->graph_info) return;
    UNUSED(input);
    UNUSED(evt);
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    self->box_animating = true;
    self->showSettings = !self->showSettings;
    self->hovered_help = false;
    MainFrame_updateHelpImage(self);
    self->box_anim_progress = 0.f;
    Element* settings = Element_getById(self->elements, "settings");
    Container* container = settings->data.container;
    self->box_start_x = container->box->position->x;
    self->box_target_x = self->showSettings ? w - self->settings_width : w;
    if (self->showSettings && self->popup) {
        for (int i = 0; i < self->graph_count; i++) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
    }
}

static void MainFrame_onRuneS(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self || self->showSettings || self->graph_info) return;
    if (self->all_selected) {
        for (int i = 0; i < self->graph_count; i++) {
            if (self->popup) {
                ColumnGraph_removeHovering(self->graph[i]);
            }
            ColumnGraph_shuffleBars(self->graph[i]);
        }
    } else {
        if (self->popup) {
            ColumnGraph_removeHovering(self->graph[self->selected_graph_index]);
        }
        ColumnGraph_shuffleBars(self->graph[self->selected_graph_index]);
    }
    MainFrame_addElements(self, self->app);
}

static bool MainFrame_createPopup(MainFrame* self, void* value, ColumnGraphType type) {
    if (!self || self->showSettings || self->graph_info) return false;
    if (self->popup) return true;
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    float x, y;
    Input_getMousePosition(self->app->input, &x, &y);

    if (!self->all_selected) {
        ColumnGraph* graph = self->graph[self->selected_graph_index];
        SDL_FRect focus_graph_rect = {graph->position->x, graph->position->y, graph->size.width, graph->size.height};
        if (!Input_mouseInRect(self->app->input, focus_graph_rect)) {
            return false;
        }
    }
    float popupWidth = 120;
    float popupHeight = 40;
    float popupX = x - 50 < 0 ? 0 : x - 50 + popupWidth > w ? w - popupWidth : x - 50;
    float popupY = y - 50 < 0 ? 0 : y - 50 + popupHeight > h ? h - popupHeight : y - 50;
    self->popup = Container_new(popupX, popupY, popupWidth, popupHeight, false, Color_copy(COLOR_BLACK), self);
    char* format;
    switch (type) {
        case GRAPH_TYPE_INT:
            format = "Value : %d";
            break;
        case GRAPH_TYPE_STRING:
            format = "Value : %s";
            break;
        default:
            format = "Value : %d";
            break;
    }
    Text* popupLabel = Text_newf(self->app->renderer,
                                 TextStyle_new(
                                     ResourceManager_getDefaultBoldFont(
                                         self->app->manager, 20), 20, COLOR_WHITE,
                                     TTF_STYLE_NORMAL),
                                 Position_new(popupX + 10, popupY),
                                 false,
                                    format, value
    );
    Size txtSize = Text_getSize(popupLabel);
    if (txtSize.width + 20 > popupWidth) {
        Container_setSize(self->popup, txtSize.width + 20, popupHeight);
        popupX = x - 50 < 0 ? 0 : x - 50 + popupWidth > w ? w - popupWidth : x - 50;
        popupY = y - 50 < 0 ? 0 : y - 50 + popupHeight > h ? h - popupHeight : y - 50;
        Text_setPosition(popupLabel, popupX + 10, popupY);
    }

    Container_addChild(self->popup, Element_fromText(popupLabel, NULL));

    return true;
}

static bool MainFrame_removePopup(MainFrame* self, void* _, ColumnGraphType type) {
    UNUSED(type);
    UNUSED(_);
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
        self->graph[i] = ColumnGraph_new(width, height, Position_new(x, y), self->app->input, self, GRAPH_TYPE_INT,
                                         (ColumnsHoverFunc) MainFrame_createPopup,
                                         (ColumnsHoverFunc) MainFrame_removePopup);
        ColumnGraph_initBarsIncrement(self->graph[i], self->bar_count, self->graph_style);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onRuneP(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings || self->graph_info) return;
    UNUSED(input);
    UNUSED(evt);
    if (self->graph_count == MAX_GRAPHS) return;
    if (self->popup) {
        for (int i = 0; i < self->graph_count; i++) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
    }
    self->graph_count++;
    for (int i = 0; i < self->graph_count - 1; i++) {
        ColumnGraph_destroy(self->graph[i]);
    }
    MainFrame_updateGraphs(self);
}

static void MainFrame_onRuneM(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings || self->graph_info) return;
    UNUSED(input);
    UNUSED(evt);
    if (self->popup) {
        for (int i = 0; i < self->graph_count; i++) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
    }
    if (self->graph_count > 1) {
        if (self->selected_graph_index == self->graph_count - 1) {
            self->selected_graph_index--;
        }
        self->graph_count--;
        for (int i = 0; i < self->graph_count + 1; i++) {
            ColumnGraph_destroy(self->graph[i]);
        }
        MainFrame_updateGraphs(self);
    }
}

static void MainFrame_quitApp(Input* input, SDL_Event* evt, Button* button) {
    UNUSED(input);
    UNUSED(evt);
    if (button && button->parent) {
        Container* parent = button->parent;
        if (parent->parent) {
            MainFrame* mainFrame = parent->parent;
            mainFrame->app->running = false;
        }
    }
}

static void MainFrame_onRuneQ(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings || self->graph_info) return;
    UNUSED(input);
    UNUSED(evt);
    if (!self || self->showSettings) return;
    bool hasPopup = self->popup != NULL;
    if (self->all_selected) {
        for (int i = 0; i < self->graph_count; i++) {
            if (hasPopup) {
                ColumnGraph_removeHovering(self->graph[i]);
            }
            ColumnGraph_sortGraph(self->graph[i], LIST_SORT_TYPE_BUBBLE, MainFrame_DelaySort, self);
        }
    } else {
        if (hasPopup) {
            ColumnGraph_removeHovering(self->graph[self->selected_graph_index]);
        }
        ColumnGraph_sortGraph(self->graph[self->selected_graph_index], LIST_SORT_TYPE_BUBBLE, MainFrame_DelaySort, self);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_DelaySort(MainFrame* self, ColumnGraph* graph, ColumnGraphBar* actual) {
    if (!graph) {
        log_message(LOG_LEVEL_WARN, "No graph to sort");
        return;
    }
    actual->element->data.box->background = COLOR_WHITE;
    ColumnGraph_resetContainer(graph);
    MainFrame_addElements(self, self->app);
    Color* background = self->app->theme->background;
    SDL_SetRenderDrawColor(self->app->renderer, background->r, background->g, background->b, background->a);
    SDL_RenderClear(self->app->renderer);
    MainFrame_render(self->app->renderer, self);
    SDL_RenderPresent(self->app->renderer);
    actual->element->data.box->background = actual->color;
    SDL_Delay(16);
}

static void MainFrame_onEnter(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self || !self->showSettings) return;
    Container* container = Element_getById(self->elements, "settings")->data.container;

    InputBox* inputBar = Element_getById(container->children, "inputBar")->data.input_box;
    InputBox* inputGraph = Element_getById(container->children, "inputGraph")->data.input_box;
    if (!String_isNumeric(InputBox_getString(inputBar)) || !String_isNumeric(InputBox_getString(inputGraph))) {
        return;
    }
    int barCount = atoi(InputBox_getString(inputBar));
    int graphCount = atoi(InputBox_getString(inputGraph));
    if (barCount <= 0 || graphCount <= 0) {
        return;
    }
    if (graphCount > MAX_GRAPHS) {
        return;
    }
    if (self->graph_count == graphCount && self->bar_count == barCount) {
        return;
    }
    if (self->graph_count == graphCount && !self->all_selected) {
        ColumnGraph* graph = self->graph[self->selected_graph_index];
        graph->bars_count = barCount;
        MainFrame_updateGraphs(self);
        return;
    }
    self->bar_count = barCount;
    for (int i = 0; i < self->graph_count; i++) {
        ColumnGraph_destroy(self->graph[i]);
    }
    self->graph_count = graphCount;
    MainFrame_updateGraphs(self);
}

static void MainFrame_loadFileCallback(MainFrame* self, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        log_message(LOG_LEVEL_WARN, "No file selected or failed to open file");
        return;
    }
    int values_len;
    fscanf(file, "%d", &values_len);
    void** values = calloc(values_len, sizeof(void*));
    if (!values) {
        error("Failed to allocate memory for values");
        fclose(file);
        return;
    }
    char typeString[64];
    fscanf(file, "%s", typeString);
    ColumnGraphType type = String_equals(typeString, "int") ? GRAPH_TYPE_INT : String_equals(typeString, "string") ? GRAPH_TYPE_STRING : GRAPH_TYPE_INT;
    char* format;
    switch (type) {
        case GRAPH_TYPE_INT:
            format = "%ld";
            for (int i = 0; i < values_len; i++) {
                fscanf(file, format, &values[i]);
            }
            break;
        case GRAPH_TYPE_STRING:
            format = "%s";
            for (int i = 0; i < values_len; i++) {
                char buffer[256];
                fscanf(file, format, buffer);
                values[i] = Strdup(buffer);
            }
            break;
    }
    fclose(file);
    if (self->all_selected) {
        for (int i = 0; i < self->graph_count; i++) {
            ColumnGraph* graph = self->graph[i];
            ListIterator* it = ListIterator_new(graph->bars);
            while (ListIterator_hasNext(it)) {
                ColumnGraphBar_destroy(ListIterator_next(it));
            }
            ListIterator_destroy(it);
            List_clear(graph->bars);
            ColumnGraph_setGraphType(graph, type);
            FlexContainer_clear(graph->container);
            ColumnGraph_initBars(graph, values_len, values, self->graph_style);
        }
    } else {
        ColumnGraph* graph = self->graph[self->selected_graph_index];
        ListIterator* it = ListIterator_new(graph->bars);
        while (ListIterator_hasNext(it)) {
            ColumnGraphBar_destroy(ListIterator_next(it));
        }
        ListIterator_destroy(it);
        List_clear(graph->bars);
        ColumnGraph_setGraphType(graph, type);
        FlexContainer_clear(graph->container);
        ColumnGraph_initBars(graph, values_len, values, self->graph_style);
    }
    safe_free((void **) &values);
    MainFrame_addElements(self, self->app);
}

static void MainFrame_loadFile(Input* input, SDL_Event* evt, Button* button) {
    UNUSED(input);
    UNUSED(evt);
    if (button && button->parent) {
        Container* parent = button->parent;
        if (parent->parent) {
            MainFrame* mainFrame = parent->parent;
            const char* filterPatterns[1] = { "*.txt" };
            const char* filePath = tinyfd_openFileDialog("Open data File",
                "",
                1,
                filterPatterns,
                "Text Files",
                0
                );
            if (filePath) {
                const char* paths[1] = { filePath };
                MainFrame_loadFileCallback(mainFrame, paths[0]);
            }
        }
    }
}

static void MainFrame_onClick(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self || self->showSettings || self->all_selected || self->graph_info) return;
    float x, y;
    Input_getMousePosition(self->app->input, &x, &y);

    if (self->graph_count > 0) {
        for (int i = 0; i < self->graph_count; i++) {
            if (i ==  self->selected_graph_index) continue;
            ColumnGraph* graph = self->graph[i];
            SDL_FRect focus_graph_rect = { graph->position->x, graph->position->y, graph->size.width, graph->size.height };
            if (Input_mouseInRect(self->app->input, focus_graph_rect)) {
                if (self->popup) {
                    ColumnGraph_removeHovering(self->graph[self->selected_graph_index]);
                    if (self->popup) {
                        MainFrame_removePopup(self, 0, GRAPH_TYPE_INT);
                    }
                }
                self->selected_graph_index = i;
                MainFrame_addElements(self, self->app);
                break;
            }
        }
    }
    Image* image_help = Element_getById(self->elements, "help_image")->data.image;
    Size size_help_image = Image_getSize(image_help);
    Position* position_help_image = image_help->position;
    SDL_FRect help_rect = {position_help_image->x,position_help_image->y, size_help_image.width, size_help_image.height};
    if (Input_mouseInRect(self->app->input, help_rect)) {
        self->hovered_help = false;
        MainFrame_updateHelpImage(self);
        App_addFrame(self->app, HelpFrame_getFrame(HelpFrame_new(self->app)));
    }
}

static void MainFrame_onRuneA(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self || self->showSettings || self->graph_info) return;
    self->all_selected = !self->all_selected;
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onGraphThemeChange(Input* input, SDL_Event* evt, Select* select) {
    UNUSED(input);
    UNUSED(evt);
    int selected = Select_getSelectedIndex(select);
    if (selected < 0) return;
    Container* parent = select->parent;
    if (!parent || !parent->parent) return;
    MainFrame* self = parent->parent;
    self->graph_style = selected;
    for (int i = 0; i < self->graph_count; i++) {
        if (self->popup) {
            ColumnGraph_removeHovering(self->graph[i]);
        }
        int len;
        void** voidValues = ColumnGraph_getValues(self->graph[i], &len);
        ColumnGraph_resetBars(self->graph[i]);
        ColumnGraph_initBars(self->graph[i], len, voidValues, self->graph_style);
        safe_free((void **) &voidValues);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onMouseMove(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self || self->showSettings || self->graph_info) return;
    Image* img = Element_getById(self->elements, "help_image")->data.image;
    Size imgSize = Image_getSize(img);
    SDL_FRect rect = { img->position->x, img->position->y, imgSize.width, imgSize.height };
    if (Input_mouseInRect(self->app->input, rect)) {
        self->hovered_help = true;
        Image_changePath(img, self->app, "help_hover.svg");
    } else if (self->hovered_help) {
        self->hovered_help = false;
        Image_changePath(img, self->app, "help_white.svg");
    }
}

static void MainFrame_updateHelpImage(MainFrame* self) {
    Image* img = Element_getById(self->elements, "help_image")->data.image;
    Image_changePath(img, self->app, "help_white.svg");
}

static void MainFrame_onWindowResize(Input* input, SDL_Event* evt, MainFrame* self) {
    UNUSED(input);
    UNUSED(evt);
    if (!self) return;
    if (evt->window.event != SDL_WINDOWEVENT_RESIZED) return;
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    if (w < WINDOW_WIDTH || h < WINDOW_HEIGHT) {
        SDL_SetWindowSize(self->app->window, WINDOW_WIDTH, WINDOW_HEIGHT);
        return;
    }
    for (int i = 0; i < self->graph_count; i++) {
        ColumnGraph_renderBar(self->graph[i], w, h);
    }
    MainFrame_addElements(self, self->app);
}

static void MainFrame_onRuneI(Input* input, SDL_Event* evt, MainFrame* self) {
    if (!self || self->showSettings) return;
    if (self->all_selected && self->graph_count > 1) return;
    UNUSED(input);
    UNUSED(evt);
    if (self->graph_info) {
        MainFrame_hideGraphInfo(self);
        return;
    }
    int index = self->all_selected ? 0 : self->selected_graph_index;
    ColumnGraph* graph = self->graph[index];
    MainFrame_showGraphInfo(self, index, graph);
}

static void MainFrame_showGraphInfo(MainFrame* self, int index, ColumnGraph* graph) {
    if (!self || self->graph_info) return;
    UNUSED(index);
    UNUSED(graph);
    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    float graph_info_width = 350;
    float graph_info_height = 250;
    SDL_Renderer* renderer = self->app->renderer;
    self->graph_info = Container_new(w / 2, h / 2, graph_info_width, graph_info_height, true, Color_copy(self->app->theme->background), self);
    Box_setBorder(self->graph_info->box, 4, Color_copy(COLOR_WHITE));
    Position* graph_info_pos = Container_getPosition(self->graph_info);
    Text* graph_title = Text_newf(renderer, TextStyle_new(
                                   ResourceManager_getDefaultBoldFont(self->app->manager, 24),
                                   24, COLOR_WHITE, TTF_STYLE_NORMAL),
                                   POSITION_NULL, false, "Graph %d Info", index + 1);
    Size title_size = Text_getSize(graph_title);
    Text_setPosition(graph_title, graph_info_pos->x + (graph_info_width - title_size.width) / 2,
                     graph_info_pos->y + 10);

    TextStyle* base_text_style = TextStyle_new(
        ResourceManager_getDefaultFont(self->app->manager, 20),
        20, COLOR_WHITE, TTF_STYLE_NORMAL);

    float y = graph_info_pos->y + 60;

    Text* type_graph_text = Text_newf(renderer, base_text_style,
        Position_new(graph_info_pos->x + 10, y),
        false,
        "Graph Type: %s", ColumnGraph_getTypeName(graph->type));

    y += 30;

    Text* bar_count_text = Text_newf(renderer, TextStyle_deepCopy(base_text_style),
        Position_new(graph_info_pos->x + 10, y),
        false,
        "Bar Count: %d", graph->bars_count);

    y += 30;

    Text* is_sorted_text = Text_newf(renderer, TextStyle_deepCopy(base_text_style),
        Position_new(graph_info_pos->x + 10, y),
        false,
        "Is Sorted: %s", List_isSorted(graph->bars, ColumnGraphBar_compare) ? "Yes" : "No");

    Container_addChild(self->graph_info, Element_fromText(graph_title, NULL));
    Container_addChild(self->graph_info, Element_fromText(type_graph_text, NULL));
    Container_addChild(self->graph_info, Element_fromText(bar_count_text, NULL));
    Container_addChild(self->graph_info, Element_fromText(is_sorted_text, NULL));

    Position_destroy(graph_info_pos);
    MainFrame_addElements(self, self->app);

}

static void MainFrame_hideGraphInfo(MainFrame* self) {
    if (!self || !self->graph_info) return;
    Container_destroy(self->graph_info);
    self->graph_info = NULL;
    MainFrame_addElements(self, self->app);
}
