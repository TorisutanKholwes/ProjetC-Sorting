//
// Created by bapti on 10/29/25.
//

#include "help_frame.h"

#include "app.h"
#include "color.h"
#include "container.h"
#include "element.h"
#include "frame.h"
#include "geometry.h"
#include "image.h"
#include "input.h"
#include "list.h"
#include "logger.h"
#include "resource_manager.h"
#include "style.h"
#include "text.h"
#include "utils.h"

#define MAX_KEY_SHOW 8

static void HelpFrame_addElements(HelpFrame* self, App* app, int curr_page);
static Container* HelpFrame_createKeyContainer(HelpFrame* self, const char* key, const char* description, int index, int curr_page);
static void HelpFrame_checkClickArrows(Input* input, SDL_Event* evt, HelpFrame* self);

HelpFrame* HelpFrame_new(App *app) {
    HelpFrame *self = calloc(1, sizeof(HelpFrame));
    if (!self) {
        error("Failed to allocate memory for HelpFrame");
        return NULL;
    }

    self->app = app;
    self->elements = List_create();
    if (!self->elements) {
        error("Failed to create elements list for HelpFrame");
        safe_free((void **) &self);
        return NULL;
    }
    self->curr_page = 1;

    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);

    HelpFrame_addElements(self, app, self->curr_page);

    return self;
}

static void HelpFrame_addElements(HelpFrame *self, App *app, int curr_page) {
    ListIterator *it = ListIterator_new(self->elements);
    while (ListIterator_hasNext(it)) {
        Element *element = ListIterator_next(it);
        Element_destroy(element);
    }
    ListIterator_destroy(it);
    List_clear(self->elements);

    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);

    Text* title = Text_new(app->renderer, TextStyle_new(
        ResourceManager_getDefaultBoldFont(app->manager, 32), 32, COLOR_WHITE, TTF_STYLE_UNDERLINE),
        POSITION_NULL, true, "Help");
    Size size = Text_getSize(title);
    Text_setPosition(title,  w / 2, size.height / 2 + 5);
    List_push(self->elements, Element_fromText(title, NULL));

    int idx = 0;
    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "ESC", "Open / Close the setting panel", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "Space", "Launch the sort (shuffle if graph are already sorted)", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "H", "Open the help page", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "S", "Shuffle the graph", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "P", String_format("Add a graph (maximum : %d)", MAX_GRAPHS), idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "M", "Remove a graph (minimum : 1)", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "A", "Select all the graph / Select only one graph", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
        HelpFrame_createKeyContainer(self, "I", "Show info of a graph", idx++,curr_page) , NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "O", "Change the sort type of the graph", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "B", "Pause the graph if sorting", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "Shift + S", "Change the seed of a graph", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "Tab", "Open a popup for selecting sort type", idx++, curr_page), NULL));

    List_push(self->elements, Element_fromContainer(
            HelpFrame_createKeyContainer(self, "Shift + Space", "Create and launch multiple graph with different sort type", idx++, curr_page), NULL));

    self->max_page = ceilf((float)(idx - 1) / MAX_KEY_SHOW);

    Image* left_arrow = Image_load(app, "left-arrow.svg", Position_new(w / 2 - 150, h - 80), false);
    Image* right_arrow = Image_load(app, "right-arrow.svg", Position_new(w / 2 + 85, h - 80), false);
    Image_setRatio(left_arrow, 0.08f);
    Image_setRatio(right_arrow, 0.08f);

    Text* page_text = Text_newf(app->renderer,
                              TextStyle_new(ResourceManager_getDefaultBoldFont(app->manager, 24),
                                            24, COLOR_WHITE, TTF_STYLE_NORMAL),
                              POSITION_NULL, true, "Page %d/%d", curr_page, self->max_page);
    Text_setPosition(page_text, w / 2, h -  50);

    List_push(self->elements, Element_fromText(page_text, NULL));
    List_push(self->elements, Element_fromImage(left_arrow, "left-arrow"));
    List_push(self->elements, Element_fromImage(right_arrow, "right-arrow"));

}

void HelpFrame_destroy(HelpFrame *self) {
    if (!self) return;

    Element_destroyList(self->elements);

    Input_removeOneEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, self);

    safe_free((void **) &self);
}

void HelpFrame_render(SDL_Renderer *renderer, HelpFrame *self) {
    Element_renderList(self->elements, renderer);
}

void HelpFrame_update(HelpFrame *self) {
    Element_updateList(self->elements);

    if (self->app->input->esc) {
        App_frameBack(self->app);
    }
}

void HelpFrame_focus(HelpFrame *self) {
    Element_focusList(self->elements);

    Input_addEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, (EventHandlerFunc) HelpFrame_checkClickArrows, self);
}

void HelpFrame_unfocus(HelpFrame *self) {
    Element_unfocusList(self->elements);

    Input_removeOneEventHandler(self->app->input, SDL_MOUSEBUTTONDOWN, self);
}

Frame *HelpFrame_getFrame(HelpFrame *self) {
    Frame *frame = Frame_new(self,
                             (FrameRenderFunc) HelpFrame_render,
                             (FrameUpdateFunc) HelpFrame_update,
                             (FrameFocusFunc) HelpFrame_focus,
                             (FrameFocusFunc) HelpFrame_unfocus,
                             (DestroyFunc) HelpFrame_destroy);
    Frame_setTitle(frame, "HelpFrame");
    return frame;
}

static Container* HelpFrame_createKeyContainer(HelpFrame* self, const char* key, const char* description, int index, int curr_page) {
    if (index < (curr_page -1) * MAX_KEY_SHOW || index >= curr_page * MAX_KEY_SHOW) {
        return NULL;
    }

    int new_index = index % MAX_KEY_SHOW;

    const int xPos = 50;
    const int yPos = 65 + new_index * 50;
    const int boxWidth = 170;
    const int boxHeight = 40;

    Container* container = Container_new(xPos, yPos, 720, boxHeight, false, COLOR_GRAY(120), self);

    Box* key_box = Box_new(boxWidth, boxHeight, 0, Position_new(xPos, yPos), COLOR_GRAY(80), NULL, false);
    Text* key_text = Text_newf(self->app->renderer,
                              TextStyle_new(ResourceManager_getDefaultBoldFont(self->app->manager, 24),
                                            24, COLOR_WHITE, TTF_STYLE_NORMAL),
                              POSITION_NULL, true, "%s", key);
    Text_setPosition(key_text, xPos + boxWidth / 2,
        yPos + boxHeight / 2);

    Text* description_text = Text_newf(self->app->renderer,
                                      TextStyle_new(ResourceManager_getDefaultFont(self->app->manager, 20),
                                                    20, COLOR_WHITE, TTF_STYLE_NORMAL),
                                      POSITION_NULL, true, "%s", description);
    Size desc_size = Text_getSize(description_text);
    Text_setPosition(description_text, xPos + boxWidth + 20 + desc_size.width / 2,
        yPos + boxHeight / 2);


    Container_addChild(container, Element_fromBox(key_box, NULL));
    Container_addChild(container, Element_fromText(key_text, NULL));
    Container_addChild(container, Element_fromText(description_text, NULL));

    return container;
}

static void HelpFrame_checkClickArrows(Input* input, SDL_Event* evt, HelpFrame* self) {
    UNUSED(evt);

    Image* left_arrow = Element_getById(self->elements, "left-arrow")->data.image;
    Size left_size = Image_getSize(left_arrow);
    SDL_FRect left_rect = {
        left_arrow->position->x,
        left_arrow->position->y,
        left_size.width,
        left_size.height
    };

    if (Input_mouseInRect(input, left_rect)) {
        if (self->curr_page > 1) {
            self->curr_page--;
            HelpFrame_addElements(self, self->app, self->curr_page);
        }
        return;
    }

    Image* right_arrow = Element_getById(self->elements, "right-arrow")->data.image;
    Size right_size = Image_getSize(right_arrow);
    SDL_FRect right_rect = {
        right_arrow->position->x,
        right_arrow->position->y,
        right_size.width,
        right_size.height
    };

    if (Input_mouseInRect(input, right_rect)) {
        if (self->curr_page < self->max_page) {
            self->curr_page++;
            HelpFrame_addElements(self, self->app, self->curr_page);
        }
    }

}