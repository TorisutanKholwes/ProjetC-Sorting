/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct InputBox {
    App* app;
    Input* input;

    Timer* timer;

    Text* text;
    SDL_FRect rect;

    char* str;
    int cursor_index;
    bool password_mode;

    InputBoxStyle* style;

    bool focused;
    bool selected;
    bool cursor_visible;

    void* parent;
};

InputBox* InputBox_new(App* app, SDL_FRect rect, InputBoxStyle* style, void* parent);
void InputBox_destroy(InputBox* input_box);
void InputBox_render(InputBox* input_box, SDL_Renderer* renderer);
void InputBox_update(InputBox* input_box);
void InputBox_setParent(InputBox* input_box, void* parent);
void InputBox_focus(InputBox* input_box);
void InputBox_unFocus(InputBox* input_box);
void InputBox_setString(InputBox* input_box, const char* str);
void InputBox_setStringf(InputBox* input_box, const char* format, ...);
char* InputBox_getString(InputBox* input_box);
char* InputBox_getFormattedString(InputBox* input_box);
void InputBox_setPasswordMode(InputBox* input_box, bool password_mode);


