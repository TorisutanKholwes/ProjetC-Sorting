/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Container {
    Box* box;
    List* children;
};

Container* Container_new(float x, float y, float width, float height, Color* color);
void Container_destroy(Container* container);
void Container_addChild(Container* container, Element* child);
void Container_removeChild(Container* container, Element* child);
void Container_render(Container* container, SDL_Renderer* renderer);