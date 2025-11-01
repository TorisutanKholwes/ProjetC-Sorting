/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

struct Checkbox {
    Position* position;
    Size size;
    Color* color;
    Color* bg_color;

    Input* input;
    bool checked;
    bool focused;

    void* parent;

    EventHandlerFunc onChange;
};

Checkbox* Checkbox_new(float x, float y, float size, Color* color, Color* bg_color, bool default_checked, Input* input, void* parent);
void Checkbox_destroy(Checkbox* checkbox);
void Checkbox_setChecked(Checkbox* checkbox, bool checked);
bool Checkbox_isChecked(Checkbox* checkbox);
void Checkbox_toggle(Checkbox* checkbox);
void Checkbox_render(Checkbox* checkbox, SDL_Renderer* renderer);
void Checkbox_update(Checkbox* checkbox);
void Checkbox_focus(Checkbox* checkbox);
void Checkbox_unFocus(Checkbox* checkbox);
void Checkbox_setPosition(Checkbox* checkbox, float x, float y);
Position* Checkbox_getPosition(Checkbox* checkbox);
Size Checkbox_getSize(Checkbox* checkbox);
void Checkbox_onChange(Checkbox* checkbox, EventHandlerFunc handler);