/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Button {
    Text* text;
    SDL_FRect rect;

    ButtonStyle* style;

    // When using hover, click who change style, keep a copy of the original style to restore it
    ButtonStyle* copy_style;

    Input* input;

    bool hovered;
    bool pressed;
    bool focused;

    bool from_center;

    void* parent;

    EventHandlerFunc onClick;
    EventHandlerFunc onHover;
    EventHandlerFunc onHoverEnd;
};

Button* Button_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* label);
Button* Button_newf(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* format, ...);
void Button_destroy(Button* button);
void Button_render(Button* button, SDL_Renderer* renderer);
void Button_setString(Button* button, const char* str);
void Button_setStringf(Button* button, const char* format, ...);
void Button_setPosition(Button* button, float x, float y);
void Button_setSize(Button* button, float w, float h);
void Button_setParent(Button* button, void* parent);
void Button_update(Button* button);
void Button_unFocus(Button* button);
void Button_focus(Button* button);
void Button_onClick(Button* button, EventHandlerFunc func);
void Button_onHover(Button* button, EventHandlerFunc func);
void Button_onHoverEnd(Button* button, EventHandlerFunc func);
Size Button_getSize(Button* button);