/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Select {
    Input* input;
    Text* text;
    SDL_FRect rect;

    ButtonStyle* style;
    bool from_center;

    List* options;
    int selected_index;
    bool focused;

    EventHandlerFunc onChange;

    void* parent;
};

Select* Select_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, List* options, int default_index);
void Select_destroy(Select* select);
void Select_render(Select* select, SDL_Renderer* renderer);
void Select_update(Select* select);
void Select_focus(Select* select);
void Select_unFocus(Select* select);
void Select_setPosition(Select* select, float x, float y);
void Select_setSize(Select* select, float w, float h);
void Select_setParent(Select* select, void* parent);
void Select_onChange(Select* select, EventHandlerFunc func);
void Select_setSelectedIndex(Select* select, int index);
int Select_getSelectedIndex(Select* select);
char* Select_getSelectedOption(Select* select);
Size Select_getSize(Select* select);