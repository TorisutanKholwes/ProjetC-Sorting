/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */
#include "checkbox.h"

#include "geometry.h"
#include "input.h"
#include "logger.h"
#include "resource_manager.h"
#include "utils.h"

static void Checkbox_checkPressed(Input* input, SDL_Event* evt, Checkbox* checkbox);

Checkbox* Checkbox_new(float x, float y, float size, Color* color, Color* bg_color, bool default_checked, Input* input, void* parent) {
    Checkbox* self = calloc(1, sizeof(Checkbox));
    if (!self) {
        error("Failed to allocate memory for Checkbox");
        return NULL;
    }
    self->position = Position_new(x, y);
    self->size = (Size){size, size};
    self->color = color;
    self->bg_color = bg_color;
    self->checked = default_checked;
    self->input = input;
    self->parent = parent;
    return self;
}

void Checkbox_destroy(Checkbox* checkbox) {
    if (!checkbox) return;

    Input_removeOneEventHandler(checkbox->input, SDL_MOUSEBUTTONDOWN, checkbox);

    Position_destroy(checkbox->position);
    Color_destroy(checkbox->color);
    safe_free((void **) &checkbox);
}

void Checkbox_setChecked(Checkbox* checkbox, bool checked) {
    if (!checkbox) return;
    checkbox->checked = checked;
}

bool Checkbox_isChecked(Checkbox* checkbox) {
    if (!checkbox) return false;
    return checkbox->checked;
}

void Checkbox_toggle(Checkbox* checkbox) {
    if (!checkbox) return;
    checkbox->checked = !checkbox->checked;
}

void Checkbox_render(Checkbox* checkbox, SDL_Renderer* renderer) {
    if (!checkbox || !renderer) return;

    SDL_SetRenderDrawColor(renderer, checkbox->color->r, checkbox->color->g,
                               checkbox->color->b, checkbox->color->a);
    SDL_FRect rect = { checkbox->position->x, checkbox->position->y,
                       checkbox->size.width, checkbox->size.height };
    SDL_RenderStroke(renderer, &rect, 2.0f);
    if (checkbox->checked) {
        SDL_FRect fillRect = { checkbox->position->x + 4.0f, checkbox->position->y + 4.0f,
                               checkbox->size.width - 8.0f, checkbox->size.height - 8.0f };
        SDL_SetRenderDrawColor(renderer, checkbox->bg_color->r, checkbox->bg_color->g,
                                   checkbox->bg_color->b, checkbox->bg_color->a);
        SDL_RenderFillRectF(renderer, &fillRect);
    }
}

void Checkbox_update(Checkbox* checkbox) {
    if (!checkbox) return;
    if (!checkbox->focused) {
        Checkbox_focus(checkbox);
    }
}

void Checkbox_focus(Checkbox* checkbox) {
    if (!checkbox) return;
    checkbox->focused = true;
    Input_addEventHandler(checkbox->input, SDL_MOUSEBUTTONDOWN, (EventHandlerFunc) Checkbox_checkPressed, checkbox);
}

void Checkbox_unFocus(Checkbox* checkbox) {
    if (!checkbox) return;
    checkbox->focused = false;
    Input_removeOneEventHandler(checkbox->input, SDL_MOUSEBUTTONDOWN, checkbox);
}

void Checkbox_setPosition(Checkbox* checkbox, float x, float y) {
    if (!checkbox) return;
    checkbox->position->x = x;
    checkbox->position->y = y;
}

Position* Checkbox_getPosition(Checkbox* checkbox) {
    if (!checkbox) return NULL;
    return checkbox->position;
}

Size Checkbox_getSize(Checkbox* checkbox) {
    if (!checkbox) return (Size){0, 0};
    return checkbox->size;
}

static void Checkbox_checkPressed(Input* input, SDL_Event* evt, Checkbox* checkbox) {
    if (!checkbox) return;
    SDL_FRect rect = { checkbox->position->x, checkbox->position->y,
                       checkbox->size.width, checkbox->size.height };
    bool isHovering = Input_mouseInRect(input, rect);
    if (isHovering && evt->type == SDL_MOUSEBUTTONDOWN) {
        checkbox->checked = !checkbox->checked;
        if (checkbox->onChange) {
            checkbox->onChange(input, evt, checkbox);
        }
    }
}

void Checkbox_onChange(Checkbox* checkbox, EventHandlerFunc handler) {
    if (!checkbox) return;
    checkbox->onChange = handler;
}