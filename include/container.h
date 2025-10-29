/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Container {
    Box* box;
    List* children;
    void* parent;
};

Container* Container_new(float x, float y, float width, float height, Color* color, void* parent);
void Container_destroy(Container* container);
void Container_addChild(Container* container, Element* child);
void Container_removeChild(Container* container, Element* child);
void Container_render(Container* container, SDL_Renderer* renderer);
void Container_update(Container* container);
void Container_focus(Container* container);
void Container_unFocus(Container* container);
void Container_setSize(Container* container, float width, float height);