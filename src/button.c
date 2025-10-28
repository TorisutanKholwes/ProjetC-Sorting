/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "button.h"

#include "logger.h"
#include "utils.h"
#include "text.h"
#include "app.h"
#include "input.h"
#include "style.h"

static void Button_checkHover(Input* input, SDL_Event* evt, void* buttonData);
static void Button_checkPressed(Input* input, SDL_Event* evt, void* buttonData);

Button* Button_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* label) {
    Button* button = calloc(1, sizeof(Button));
    if (!button) {
        error("Failed to allocate memory for Button");
        return NULL;
    }
    button->text = Text_new(app->renderer, TextStyle_new(
        style->text_font,
        style->text_size,
        Color_copy(style->colors->text),
        style->text_style), POSITION_NULL, false, label);
    Size size = Text_getSize(button->text);
    button->rect = SDL_CreateRect(position->x, position->y, size.width, size.height, from_center);
    button->style = style;
    button->input = app->input;
    button->hovered = false;
    button->pressed = false;
    button->focused = false;
    button->from_center = from_center;
    button->parent = parent;

    return button;
}

Button* Button_newf(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* format, ...) {
    Button* button = calloc(1, sizeof(Button));
    if (!button) {
        error("Failed to allocate memory for Button");
        return NULL;
    }
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    button->text = Text_new(app->renderer, TextStyle_new(
        style->text_font,
        style->text_size,
        Color_copy(style->colors->text),
        style->text_style), POSITION_NULL, false, buffer);
    Size size = Text_getSize(button->text);
    const float x = position && !Position_isNull(position) ? position->x : 0;
    const float y = position && !Position_isNull(position) ? position->y : 0;
    button->rect = SDL_CreateRect(x, y, size.width, size.height, from_center);
    button->style = style;
    button->input = app->input;
    button->hovered = false;
    button->pressed = false;
    button->focused = false;
    button->parent = parent;

    Position_destroy(position);

    return button;
}

void Button_destroy(Button* button) {
    if (!button) return;
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_MOTION, button);
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_DOWN, button);
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_UP, button);

    Text_destroy(button->text);
    ButtonStyle_destroy(button->style);
    safe_free((void**)&button);
}

void Button_render(Button* button, SDL_Renderer* renderer) {

    Text_setColor(button->text, Color_copy(button->style->colors->text));

    Color* border = button->style->colors->border;
    Color* fill = button->style->colors->background;
    int borderWidth = button->style->border_width;

    EdgeInsets* paddings = button->style->paddings;
    SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);
    SDL_FRect borderRect;
    if (button->from_center) {
        borderRect = (SDL_FRect) { button->rect.x - borderWidth - paddings->left, button->rect.y - borderWidth - paddings->top, button->rect.w + (borderWidth * 2)+ (paddings->right + paddings->left), button->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top)};
    } else {
        borderRect = (SDL_FRect) { button->rect.x, button->rect.y, button->rect.w + (borderWidth * 2)+ (paddings->right + paddings->left), button->rect.h + (borderWidth * 2) + (paddings->bottom + paddings->top)};
    }
    SDL_RenderFillRect(renderer, &borderRect);

    SDL_SetRenderDrawColor(renderer, fill->r, fill->g, fill->b, fill->a);
    SDL_FRect fillRect;
    if (button->from_center) {
        fillRect = (SDL_FRect) { button->rect.x - paddings->left, button->rect.y - paddings->top, button->rect.w + (paddings->right + paddings->left),  button->rect.h + (paddings->bottom + paddings->top)};
    } else {
        fillRect = (SDL_FRect) { button->rect.x + borderWidth, button->rect.y + borderWidth, button->rect.w + (paddings->right + paddings->left),  button->rect.h + (paddings->bottom + paddings->top)};
    }
    SDL_RenderFillRect(renderer, &fillRect);

    const float textX = fillRect.x + (fillRect.w / 2) - (Text_getSize(button->text).width / 2);
    const float textY = fillRect.y + (fillRect.h / 2) - (Text_getSize(button->text).height / 2);
    Text_setPosition(button->text, textX, textY);
    Text_render(button->text);
}

void Button_update(Button* button) {
    if (!button->focused) {
        Button_focus(button);
    }
}

void Button_unFocus(Button* button) {
    log_message(LOG_LEVEL_DEBUG, "Button %s unfocused", button->text->text);
    button->focused = false;
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_MOTION, button);
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_DOWN, button);
    Input_removeOneEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_UP, button);
}

void Button_focus(Button* button) {
    log_message(LOG_LEVEL_DEBUG, "Button %s focused", button->text->text);
    button->focused = true;
    Input_addEventHandler(button->input, SDL_EVENT_MOUSE_MOTION, Button_checkHover, button);
    Input_addEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_DOWN, Button_checkPressed, button);
    Input_addEventHandler(button->input, SDL_EVENT_MOUSE_BUTTON_UP, Button_checkPressed, button);
}

void Button_setString(Button* button, const char* str) {
    if (!str || !button) return;
    Text_setString(button->text, str);
}

void Button_setStringf(Button* button, const char* format, ...) {
    if (!format || !button) return;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Text_setString(button->text, buffer);
}

void Button_setPosition(Button* button, float x, float y) {
    if (!button) return;
    button->rect.x = x;
    button->rect.y = y;
}

void Button_setSize(Button* button, float w, float h) {
    if (!button) return;
    button->rect.w = w;
    button->rect.h = h;
}

void Button_setParent(Button* button, void* parent) {
    if (!button) return;
    button->parent = parent;
}

static void Button_checkHover(Input* input, SDL_Event* evt, void* buttonData) {
    Button* button = buttonData;
    bool isHovering = Input_mouseInRect(input, (SDL_FRect){
        .x = (int)button->rect.x,
        .y = (int)button->rect.y,
        .w = (int)button->rect.w,
        .h = (int)button->rect.h
    });
    if (isHovering && !button->hovered) {
        //log_message(LOG_LEVEL_DEBUG, "Button hovered");
        button->hovered = true;
        if (button->onHover) {
            button->onHover(input, evt, buttonData);
        }
    } else if (!isHovering && button->hovered) {
        button->hovered = false;
        if (button->onHoverEnd) {
            button->onHoverEnd(input, evt, buttonData);
        }
    }
}

static void Button_checkPressed(Input* input, SDL_Event* evt, void* buttonData) {
    Button* button = buttonData;
    EdgeInsets* paddings = button->style->paddings;
    SDL_FRect fullRect;
    if (button->from_center) {
        fullRect = (SDL_FRect) { button->rect.x - paddings->left, button->rect.y - paddings->top, button->rect.w + (paddings->right + paddings->left),  button->rect.h + (paddings->bottom + paddings->top)};
    } else {
        fullRect = (SDL_FRect) { button->rect.x, button->rect.y, button->rect.w + (paddings->right + paddings->left),  button->rect.h + (paddings->bottom + paddings->top)};
    }
    bool isHovering = Input_mouseInRect(input, fullRect);
    if (isHovering && evt->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        button->pressed = true;
        if (button->onClick) {
            button->onClick(input, evt, buttonData);
        }
    } else if (isHovering && evt->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        button->pressed = false;
    } else {
        button->pressed = false;
    }
}

void Button_onClick(Button* button, EventHandlerFunc func) {
    button->onClick = func;
}

void Button_onHover(Button* button, EventHandlerFunc func) {
    button->onHover = func;
}

void Button_onHoverEnd(Button* button, EventHandlerFunc func) {
    button->onHoverEnd = func;
}