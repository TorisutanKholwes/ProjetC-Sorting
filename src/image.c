/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "image.h"

#include "app.h"
#include "logger.h"
#include "resource_manager.h"
#include "utils.h"

Image* Image_new(SDL_Texture* texture, Position* position, bool from_center) {
    Image* self = calloc(1, sizeof(Image));
    if (!self) {
        error("Failed to allocate memory for Image");
        return NULL;
    }
    self->texture = texture;
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
    self->size.width = w;
    self->size.height = h;
    self->position = position;
    self->from_center = from_center;
    self->custom_size = false;
    self->ratio = 1.f;

    return self;
}

Image* Image_load(App* app, const char* path, Position* position, bool from_center) {
    Image* self = calloc(1, sizeof(Image));
    if (!self) {
        error("Failed to allocate memory for Image");
        return NULL;
    }
    self->position = position;
    self->from_center = from_center;
    self->custom_size = false;
    self->ratio = 1.f;
    SDL_Texture* texture = ResourceManager_getTexture(app->manager, path);
    if (!texture) {
        error("Failed to load texture from path: %s", path);
        safe_free((void**)&self);
        return NULL;
    }
    self->texture = texture;
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
    self->size.width = w;
    self->size.height = h;
    return self;
}

void Image_destroy(Image* self) {
    if (!self) return;
    safe_free((void**)&self->position);
    safe_free((void**)&self);
}

void Image_render(Image* self, SDL_Renderer* renderer) {
    if (!self || !renderer || !self->texture) return;
    if (Position_isNull(self->position)) {
        error("Image position is null");
        return;
    }

    float x, y;
    if (self->from_center) {
        x = self->position->x - (self->size.width / 2);
        y = self->position->y - (self->size.height / 2);
    } else {
        x = self->position->x;
        y = self->position->y;
    }

    float width = self->size.width;
    float height = self->size.height;
    if (!self->custom_size) {
        width *= self->ratio;
        height *= self->ratio;
    }

    SDL_FRect dst = { x, y, width, height };
    if (!SDL_RenderTexture(renderer, self->texture, NULL, &dst)) {
        error("Failed to render image texture : %s", SDL_GetError());
    }
}

void Image_setSize(Image* self, float width, float height) {
    if (!self) return;
    self->custom_size = true;
    self->size.width = width;
    self->size.height = height;
}

Size Image_getSize(Image* self) {
    if (!self) return (Size){-1, -1};
    if (!self->custom_size) {
        return (Size){ self->size.width * self->ratio, self->size.height * self->ratio };
    }
    return self->size;
}

void Image_setPosition(Image* self, float x, float y) {
    if (!self) return;
    if (!self->position) {
        self->position = Position_new(x, y);
    } else {
        self->position->x = x;
        self->position->y = y;
    }
}

void Image_setRatio(Image* self, float ratio) {
    if (!self) return;
    self->ratio = ratio;
}

void Image_setTexture(Image* self, SDL_Texture* texture) {
    self->texture = texture;
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
    self->size.width = w;
    self->size.height = h;
}

void Image_changePath(Image* self, App* app, const char* path) {
    ResourceManager* manager = app->manager;
    SDL_Texture* texture = ResourceManager_getTexture(manager, path);
    if (!texture) {
        error("Failed to load texture from path: %s", path);
        return;
    }
    Image_setTexture(self, texture);
}