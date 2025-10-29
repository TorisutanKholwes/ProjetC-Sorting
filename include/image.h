/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct Image {
    SDL_Texture* texture;
    Position* position;
    Size size;
    float ratio;
    bool from_center;
    bool custom_size;
};

Image* Image_new(SDL_Texture* texture, Position* position, bool from_center);
Image* Image_load(App* app, const char* path, Position* position, bool from_center);
void Image_destroy(Image* self);
void Image_render(Image* self, SDL_Renderer* renderer);

void Image_setTexture(Image* self, SDL_Texture* texture);
void Image_changePath(Image* self, App* app, const char* path);
void Image_setSize(Image* self, float width, float height);
Size Image_getSize(Image* self);
void Image_setPosition(Image* self, float x, float y);
void Image_setRatio(Image* self, float ratio);