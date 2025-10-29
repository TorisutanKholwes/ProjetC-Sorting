//
// Created by bapti on 10/29/25.
//

#pragma once

#include "Settings.h"

struct HelpFrame {
    List* elements;
    App *app;
};

HelpFrame* HelpFrame_new(App* app);
void HelpFrame_destroy(HelpFrame* self);
void HelpFrame_focus(HelpFrame* self);
void HelpFrame_unfocus(HelpFrame* self);
void HelpFrame_render(SDL_Renderer* renderer, HelpFrame* self);
void HelpFrame_update(HelpFrame* self);

Frame* HelpFrame_getFrame(HelpFrame* self);
