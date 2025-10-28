/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

#define MAX_KEY_DOWN 256

struct Input {
    List* keysDown;
    SDL_Scancode lastPressed;
    Map* keyEventHandlers;
    Map* eventHandlers;
    Position* mousePos;
    bool mouse_left, mouse_right;
    bool shift, ctrl, alt;
    bool esc;
    bool quit;
};

struct EventHandler {
    EventHandlerFunc func;
    void* data;
};

Input* Input_create();
void Input_destroy(Input* input);
void Input_update(Input* input);
bool Input_keyDown(Input* input, SDL_Scancode key);
bool Input_mouseInRect(Input* input, SDL_FRect rect);
void Input_addKeyEventHandler(Input* input, SDL_Scancode key, EventHandlerFunc func, void* data);
void Input_removeKeyEventHandler(Input* input, SDL_Scancode key);
void Input_removeOneKeyEventHandler(Input* input, SDL_Scancode key, void* data);
void Input_clearKeyEventHandlers(Input* input);
void Input_addEventHandler(Input* input, Uint32 eventType, EventHandlerFunc func, void* data);
void Input_removeEventHandler(Input* input, Uint32 eventType);
void Input_removeOneEventHandler(Input* input, Uint32 eventType, void* data);
void Input_clearEventHandlers(Input* input);

void Input_getMousePosition(Input* input, float* x, float* y);