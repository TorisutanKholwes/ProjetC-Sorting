//
// Created by bapti on 10/29/25.
//

#include "help_frame.h"

#include "app.h"
#include "color.h"
#include "container.h"
#include "element.h"
#include "frame.h"
#include "image.h"
#include "input.h"
#include "list.h"
#include "logger.h"
#include "resource_manager.h"
#include "style.h"
#include "text.h"
#include "utils.h"


static void HelpFrame_addElements(HelpFrame *self, App *app);

HelpFrame *HelpFrame_new(App *app) {
    HelpFrame *self = calloc(1, sizeof(HelpFrame));
    if (!self) {
        error("Failed to allocate memory for HelpFrame");
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

    HelpFrame_addElements(self, app);

    return self;
}

static void HelpFrame_addElements(HelpFrame *self, App *app) {
    ListIterator *it = ListIterator_new(self->elements);
    while (ListIterator_hasNext(it)) {
        Element *element = ListIterator_next(it);
        Element_destroy(element);
    }
    ListIterator_destroy(it);
    List_clear(self->elements);
    TextStyle *text_title_style = TextStyle_new(
        ResourceManager_getDefaultBoldFont(app->manager, 36),
        36, COLOR_WHITE, TTF_STYLE_NORMAL);

    int w, h;
    SDL_GetWindowSize(self->app->window, &w, &h);
    Text *title_text = Text_new(app->renderer, text_title_style, Position_new(w / 2, 0), true, "Help");
    Size size_title_text = Text_getSize(title_text);
    Text_setPosition(title_text, size_title_text.width, size_title_text.height / 2);
    List_push(self->elements, Element_fromText(title_text, NULL));

    Position *position_A = Position_new(0, (h / 8) * 2);
    Image *image_A = Image_load(self->app, "letter_A.svg", position_A, false);
    Image_setRatio(image_A, 0.05);
    List_push(self->elements, Element_fromImage(image_A, NULL));
    Size size_image_A = Image_getSize(image_A);
    TextStyle *text_A_style = TextStyle_new(
        ResourceManager_getDefaultBoldFont(app->manager, 20),
        20, COLOR_WHITE, TTF_STYLE_NORMAL);
    Text *text_A = Text_new(app->renderer,
                            text_A_style,
                            Position_new(position_A->x + 10 + size_image_A.width, position_A->y+5),
                            false,
                            "explication de A");
    List_push(self->elements, Element_fromText(text_A, NULL));
}

void HelpFrame_destroy(HelpFrame *self) {
    if (!self) return;

    Element_destroyList(self->elements);

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
}

void HelpFrame_unfocus(HelpFrame *self) {
    Element_unfocusList(self->elements);
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
