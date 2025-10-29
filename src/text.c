/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "text.h"

#include "logger.h"
#include "style.h"
#include "utils.h"

void refreshTexture(Text* self);

Text* Text_new(SDL_Renderer* renderer, TextStyle* style, Position* position, bool fromCenter, const char* str) {
    Text* text = calloc(1, sizeof(Text));
    if (!text) {
        error("Failed to allocate memory for Text");
        return NULL;
    }
    text->renderer = renderer;
    text->text = Strdup(str);
    text->style = style;
    text->position = position;
    text->fromCenter = fromCenter;
    text->texture = NULL;

    refreshTexture(text);

    return text;
}

Text* Text_newf(SDL_Renderer* renderer, TextStyle* style, Position* position, bool fromCenter, const char* format, ...) {
    Text* text = calloc(1, sizeof(Text));
    if (!text) {
        error("Failed to allocate memory for Text");
        return NULL;
    }
    text->renderer = renderer;
    text->style = style;
    text->position = position;
    text->fromCenter = fromCenter;
    text->texture = NULL;
    if (format) {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        text->text = Strdup(buffer);
    } else {
        text->text = Strdup("");
    }

    refreshTexture(text);

    return text;
}

void Text_destroy(Text* self) {
    if (!self) return;

    if (self->texture) {
        SDL_DestroyTexture(self->texture);
    }
    TextStyle_destroy(self->style);
    Position_destroy(self->position);
    safe_free((void**)&(self->text));
    safe_free((void**)&self);
}

void Text_setString(Text* self, const char* str) {
    if (self->text && strcmp(self->text, str) == 0) {
        return;
    }

    safe_free((void**)&(self->text));
    self->text = Strdup(str);

    refreshTexture(self);
}

void Text_setStringf(Text* self, const char* format, ...) {
    if (!format) return;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Text_setString(self, buffer);
}

void Text_setColor(Text* self, Color* color) {
    if (memcmp(&(self->style->color), color, sizeof(Color)) == 0) {
        return;
    }

    if (self->style->color) {
        Color_destroy(self->style->color);
    }

    self->style->color = color;
    refreshTexture(self);
}

void Text_setPosition(Text* self, float x, float y) {
    if (!self->position) {
        self->position = Position_new(x, y);
    } else {
        self->position->x = x;
        self->position->y = y;
    }
}

void refreshTexture(Text* self) {
    if (self->texture) {
        SDL_DestroyTexture(self->texture);
        self->texture = NULL;
    }
    char* text = self->text;
    if (String_isNullOrEmpty(self->text)) {
        // Avoid creating an empty texture
        text = " ";
    }
    TTF_SetFontStyle(self->style->font, self->style->style);

    SDL_Surface *surface = TTF_RenderText_Blended(self->style->font, text, Color_toSDLColor(self->style->color));
    if (!surface) {
        error("Failed to create text surface.");
        return;
    }

    self->texture = SDL_CreateTextureFromSurface(self->renderer, surface);

    if (!self->custom_size) {
        int w, h;
        SDL_QueryTexture(self->texture, NULL, NULL, &w, &h);
        self->size.width = w;
        self->size.height = h;
    }

    if (!self->texture) {
        error("Failed to create text texture.");
        SDL_FreeSurface(surface);
        return;
    }

    SDL_FreeSurface(surface);
}

void Text_render(Text* self) {
    if (!self || !self->texture) return;
    if (Position_isNull(self->position)) {
        error("Text position is not set.");
        return;
    }

    float x, y;
    if (self->fromCenter) {
        x = self->position->x - (self->size.width / 2);
        y = self->position->y - (self->size.height / 2);
    } else {
        x = self->position->x;
        y = self->position->y;
    }

    SDL_FRect dst = { x, y, self->size.width, self->size.height };
    SDL_RenderCopyF(self->renderer, self->texture, NULL, &dst);
}

Size Text_getSize(Text* self) {
    if (!self) return (Size){-1, -1};
    return self->size;
}

void Text_setSize(Text* self, float width, float height) {
    if (!self) return;
    self->custom_size = true;
    self->size.width = width;
    self->size.height = height;
}