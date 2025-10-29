//
// Created by bapti on 10/29/25.
//

#include "help_frame.h"

#include "app.h"
#include "color.h"
#include "container.h"
#include "element.h"
#include "frame.h"
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
    ListIterator* it = ListIterator_new(self->elements);
    while (ListIterator_hasNext(it)) {
        Element* element = ListIterator_next(it);
        Element_destroy(element);
    }
    ListIterator_destroy(it);
    List_clear(self->elements);    TextStyle *text_style = TextStyle_new(
        ResourceManager_getDefaultBoldFont(app->manager, 36),
        36, COLOR_WHITE, TTF_STYLE_UNDERLINE);
    Text *title_text = Text_new(app->renderer, text_style, Position_new(0, 0), false, "Help");
    List_push(self->elements, Element_fromText(title_text, NULL));
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
