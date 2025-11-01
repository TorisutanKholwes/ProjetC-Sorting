/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Box {
    Size size;
    int border_size;
    Position* position;
    Color* background;
    Color* border_color;
    bool center;
};

Box* Box_new(float width, float height, int border_size, Position* position, Color* background, Color* border_color, bool center);
void Box_destroy(Box* self);
void Box_render(Box* self, SDL_Renderer* renderer);
void Box_setBorder(Box* self, int border_size, Color* border_color);
void Box_getPosition(Box* self, float* x, float* y);
void Box_setPosition(Box* self, float x, float y);

struct Circle {
    int radius;
    int border_size;
    Position* center;
    Color* background;
    Color* border_color;
};

Circle* Circle_new(int radius, int border_size, Position* center, Color* background, Color* border_color);
void Circle_destroy(Circle* self);
void Circle_render(Circle* self, SDL_Renderer* renderer);

struct Polygon {
    Position** vertices;
    int vertex_count;
    Color* background;
    Color* border;
    int border_size;
};

Polygon* Polygon_new(Position** vertices, int vertex_count, int border_size, Color* background, Color* border);
void Polygon_destroy(Polygon* self);
void Polygon_render(Polygon* self, SDL_Renderer* renderer);

Polygon* Polygon_newEmpty(int border_size, Color* background, Color* border);
void Polygon_addVertex(Polygon* self, Position* vertex);
