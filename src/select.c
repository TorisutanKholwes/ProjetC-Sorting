/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "select.h"

#include "app.h"
#include "color.h"
#include "input.h"
#include "list.h"
#include "logger.h"
#include "style.h"
#include "text.h"
#include "utils.h"

static void Select_checkPressed(Input* input, SDL_Event* evt, Select* select);

Select* Select_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, List* options, int default_index) {
    Select* select = calloc(1, sizeof(Select));
    if (!select) {
        error("Failed to allocate memory for Select");
        return NULL;
    }
    select->input = app->input;
    select->style = style;
    select->options = options;
    select->selected_index = default_index;
    select->parent = parent;
    select->from_center = from_center;
    select->text = Text_newf(app->renderer, TextStyle_new(
        style->text_font,
        style->text_size,
        Color_copy(style->colors->text),
        style->text_style), POSITION_NULL, false, "%s", List_get(options, default_index));
    Size size = Text_getSize(select->text);
    select->rect = SDL_CreateRect(position->x, position->y, size.width, size.height, from_center);
    return select;
}

void Select_destroy(Select* select) {
    if (!select) return;

    Input_removeOneEventHandler(select->input, SDL_EVENT_MOUSE_BUTTON_DOWN, select);

    Text_destroy(select->text);
    ButtonStyle_destroy(select->style);
    List_destroy(select->options);

    safe_free((void**)&select);
}

void Select_render(Select* select, SDL_Renderer* renderer) {
    Text_setColor(select->text, Color_copy(select->style->colors->text));
    Color* border = select->style->colors->border;
    Color* fill = select->style->colors->background;
    int borderWidth = select->style->border_width;

    EdgeInsets* paddings = select->style->paddings;
    SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);
    if (!Color_equals(select->style->colors->background, COLOR_TRANSPARENT)) {
        SDL_FRect borderRect;
        if (select->from_center) {
            borderRect = (SDL_FRect) { select->rect.x - borderWidth - paddings->left, select->rect.y - borderWidth - paddings->top, select->rect.w + (borderWidth * 2)+ (paddings->right + paddings->left), select->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top)};
        } else {
            borderRect = (SDL_FRect) { select->rect.x, select->rect.y, select->rect.w + (borderWidth * 2)+ (paddings->right + paddings->left), select->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top)};
        }
        SDL_RenderFillRect(renderer, &borderRect);
    } else {
        if (borderWidth > 0) {
            float bx, by, bw, bh;
            if (select->from_center) {
                bx = select->rect.x - borderWidth - paddings->left;
                by = select->rect.y - borderWidth - paddings->top;
                bw = select->rect.w + (borderWidth * 2) + (paddings->right + paddings->left);
                bh = select->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top);
            } else {
                bx = select->rect.x;
                by = select->rect.y;
                bw = select->rect.w + (borderWidth * 2) + (paddings->right + paddings->left);
                bh = select->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top);
            }

            SDL_FRect top    = { bx, by, bw, (float)borderWidth };
            SDL_FRect bottom = { bx, by + bh - borderWidth, bw, (float)borderWidth };
            SDL_FRect left   = { bx, by, (float)borderWidth, bh };
            SDL_FRect right  = { bx + bw - borderWidth, by, (float)borderWidth, bh };

            SDL_RenderFillRect(renderer, &top);
            SDL_RenderFillRect(renderer, &bottom);
            SDL_RenderFillRect(renderer, &left);
            SDL_RenderFillRect(renderer, &right);
        }
    }

    SDL_SetRenderDrawColor(renderer, fill->r, fill->g, fill->b, fill->a);
    SDL_FRect fillRect;
    if (select->from_center) {
        fillRect = (SDL_FRect) { select->rect.x - paddings->left, select->rect.y - paddings->top, select->rect.w + (paddings->right + paddings->left),  select->rect.h + (paddings->bottom + paddings->top)};
    } else {
        fillRect = (SDL_FRect) { select->rect.x + borderWidth, select->rect.y + borderWidth, select->rect.w + (paddings->right + paddings->left),  select->rect.h + (paddings->bottom + paddings->top)};
    }
    SDL_RenderFillRect(renderer, &fillRect);

    const float textX = fillRect.x + (fillRect.w / 2) - (Text_getSize(select->text).width / 2);
    const float textY = fillRect.y + (fillRect.h / 2) - (Text_getSize(select->text).height / 2);
    Text_setPosition(select->text, textX, textY);
    Text_render(select->text);

}

void Select_update(Select* select) {
    if (!select) return;

    if (!select->focused) {
        Select_focus(select);
    }
}

void Select_focus(Select* select) {
    if (!select) return;
    select->focused = true;

    Input_addEventHandler(select->input, SDL_EVENT_MOUSE_BUTTON_DOWN, (EventHandlerFunc) Select_checkPressed, select);
}

void Select_unFocus(Select* select) {
    if (!select) return;
    select->focused = false;

    Input_removeOneEventHandler(select->input, SDL_EVENT_MOUSE_BUTTON_DOWN, select);
}

void Select_setPosition(Select* select, float x, float y) {
    if (!select) return;
    select->rect.x = x;
    select->rect.y = y;
}

void Select_setSize(Select* select, float w, float h) {
    if (!select) return;
    select->rect.w = w;
    select->rect.h = h;
}

void Select_setParent(Select* select, void* parent) {
    if (!select) return;
    select->parent = parent;
}

void Select_onChange(Select* select, EventHandlerFunc func) {
    if (!select) return;
    select->onChange = func;
}

void Select_setSelectedIndex(Select* select, int index) {
    if (!select) return;
    if (index < 0 || (size_t) index >= List_size(select->options)) {
        log_message(LOG_LEVEL_WARN, "Select_setSelectedIndex: index %d out of bounds", index);
        return;
    }
    select->selected_index = index;
    Text_setStringf(select->text, "%s", List_get(select->options, select->selected_index));
}

int Select_getSelectedIndex(Select* select) {
    if (!select) return -1;
    return select->selected_index;
}

char* Select_getSelectedOption(Select* select) {
    if (!select) return NULL;
    return List_get(select->options, select->selected_index);
}

static void Select_checkPressed(Input* input, SDL_Event* evt, Select* select) {
    EdgeInsets* paddings = select->style->paddings;
    SDL_FRect fullRect;
    if (select->from_center) {
        fullRect = (SDL_FRect) { select->rect.x - paddings->left, select->rect.y - paddings->top, select->rect.w + (paddings->right + paddings->left),  select->rect.h + (paddings->bottom + paddings->top)};
    } else {
        fullRect = (SDL_FRect) { select->rect.x, select->rect.y, select->rect.w + (paddings->right + paddings->left),  select->rect.h + (paddings->bottom + paddings->top)};
    }
    bool isHovering = Input_mouseInRect(input, fullRect);
    if (isHovering) {
        select->selected_index = modulo(select->selected_index+1, List_size(select->options));
        Text_setStringf(select->text, "%s", List_get(select->options, select->selected_index));
        if (select->onChange) {
            select->onChange(input, evt, select);
        }
    }
}

Size Select_getSize(Select* select) {
    if (!select) return (Size){0,0};
    EdgeInsets* paddings = select->style->paddings;
    float width = select->rect.w + (paddings->left + paddings->right) + (select->style->border_width * 2);
    float height = select->rect.h + (paddings->top + paddings->bottom) + (select->style->border_width * 2);
    return (Size) { width, height };
}