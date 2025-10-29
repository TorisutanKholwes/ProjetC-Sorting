/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "input_box.h"

#include "app.h"
#include "input.h"
#include "logger.h"
#include "style.h"
#include "text.h"
#include "timer.h"
#include "utils.h"

static void InputBox_checkKeyDown(Input* input, SDL_Event* event, void* data);
static void InputBox_checkMouseClick(Input* input, SDL_Event* event, void* data);

InputBox *InputBox_new(App *app, SDL_FRect rect, InputBoxStyle *style, void* parent) {
    InputBox *self = calloc(1, sizeof(InputBox));
    if (!self) {
        error("Failed to allocate memory for InputBox");
        return NULL;
    }
    self->rect = rect;
    self->style = style;
    self->app = app;
    self->str = Strdup("");
    self->cursor_index = fmaxf(0, strlen(self->str));
    self->input = app->input;
    self->timer = Timer_new();
    self->text = Text_new(app->renderer, TextStyle_new(
                              style->font,
                              style->text_size,
                              Color_copy(style->colors->text),
                              style->style),
                              POSITION_NULL, false, "");
    self->focused = false;
    self->selected = false;
    self->parent = parent;
    return self;
}

void InputBox_destroy(InputBox *self) {
    if (!self) return;

    Timer_stop(self->timer);
    Input_removeOneEventHandler(self->input, SDL_TEXTINPUT, self);
    Input_removeOneEventHandler(self->input, SDL_KEYDOWN, self);
    Input_removeOneEventHandler(self->input, SDL_MOUSEBUTTONDOWN, self);
    if (self->selected) {
        SDL_StopTextInput();
    }

    Text_destroy(self->text);
    InputBoxStyle_destroy(self->style);
    Timer_destroy(self->timer);
    safe_free((void**)&self->str);
    safe_free((void **) &self);
}

void InputBox_render(InputBox *self, SDL_Renderer *renderer) {

    Text_setColor(self->text, Color_copy(self->style->colors->text));

    Color *border = self->style->colors->border;
    Color *fill = self->style->colors->background;
    SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);

    SDL_FRect borderRect = {self->rect.x - 2, self->rect.y - 2, self->rect.w + 4, self->rect.h + 4};
    SDL_RenderFillRectF(renderer, &borderRect);

    SDL_SetRenderDrawColor(renderer, fill->r, fill->g, fill->b, fill->a);
    SDL_RenderFillRectF(renderer, &self->rect);

    const float textX = self->rect.x + 5;
    const float textY = self->rect.y + (self->rect.h / 2) - (Text_getSize(self->text).height / 2);
    Text_setPosition(self->text, textX, textY);

    if (self->selected && self->cursor_visible) {
        //Text_setStringf(self->text, "%s|", InputBox_getFormattedString(self));
        char* formattedStr = InputBox_getFormattedString(self);
        size_t len = strlen(formattedStr);
        char* displayStr = calloc(len + 2, sizeof(char));
        if (!displayStr) {
            error("Failed to allocate memory for InputBox display string");
            return;
        }
        memcpy(displayStr, formattedStr, self->cursor_index);
        displayStr[self->cursor_index] = '|';
        memcpy(displayStr + self->cursor_index + 1, formattedStr + self->cursor_index, len - self->cursor_index);
        displayStr[len + 1] = '\0';
        Text_setString(self->text, displayStr);
        safe_free((void**)&displayStr);
    } else {
        Text_setString(self->text, InputBox_getFormattedString(self));
    }

    if (!String_isNullOrEmpty(self->text->text)) {
        Text_render(self->text);
    }
}

void InputBox_update(InputBox *self) {
    if (!self->focused) {
        InputBox_focus(self);
    }
    if (self->selected && Timer_getTicks(self->timer) >= 500) {
        self->cursor_visible = !self->cursor_visible;
        Timer_reset(self->timer);
    }
}

void InputBox_setParent(InputBox *self, void *parent) {
    self->parent = parent;
}

void InputBox_focus(InputBox *self) {
    self->focused = true;
    Timer_start(self->timer);
    Input_addEventHandler(self->input, SDL_TEXTINPUT, InputBox_checkKeyDown, self);
    Input_addEventHandler(self->input, SDL_KEYDOWN, InputBox_checkKeyDown, self);
    Input_addEventHandler(self->input, SDL_MOUSEBUTTONDOWN, InputBox_checkMouseClick, self);
}

void InputBox_unFocus(InputBox *self) {
    self->focused = false;
    Timer_stop(self->timer);
    Input_removeOneEventHandler(self->input, SDL_TEXTINPUT, self);
    Input_removeOneEventHandler(self->input, SDL_KEYDOWN, self);
    Input_removeOneEventHandler(self->input, SDL_MOUSEBUTTONDOWN, self);
    if (self->selected) {
        SDL_StopTextInput();
    }
}

void InputBox_setString(InputBox *self, const char *str) {
    self->str = Strdup(str);
    self->cursor_index = fmaxf(0, strlen(self->str));
    Text_setString(self->text, str);
}

char* InputBox_getFormattedString(InputBox* self) {
    if (!self) return NULL;
    if (self->password_mode) {
        size_t len = strlen(self->str);
        char* maskedStr = calloc(len + 1, sizeof(char));
        for (size_t i = 0; i < len; i++) {
            maskedStr[i] = '*';
        }
        maskedStr[len] = '\0';
        return maskedStr;
    } else {
        return self->str;
    }
}

void InputBox_setStringf(InputBox *self, const char *format, ...) {
    if (!format || !self) return;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    self->str = Strdup(buffer);
    self->cursor_index = fmaxf(0, strlen(self->str));
    Text_setString(self->text, buffer);
}

char* InputBox_getString(InputBox* input_box) {
    if (!input_box) return NULL;
    return input_box->str;
}

void InputBox_setPasswordMode(InputBox* self, bool password_mode) {
    self->password_mode = password_mode;
}

static void InputBox_checkMouseClick(Input* input, SDL_Event* event, void* data) {
    if (!data || !event || !input) return;
    InputBox* self = data;
    if (event->button.button != SDL_BUTTON_LEFT) {
        return;
    }
    if (Input_mouseInRect(input, self->rect)) {
        //log_message(LOG_LEVEL_DEBUG, "Input now selected");
        self->selected = true;
        SDL_StartTextInput();
    } else if (self->selected) {
        //log_message(LOG_LEVEL_DEBUG, "Input now unselected");
        self->selected = false;
        SDL_StopTextInput();
    }
}

static void InputBox_checkKeyDown(Input* input, SDL_Event* event, void* data) {
    if (!data || !event || !input) return;
    InputBox* self = data;
    if (!self->selected) {
        return;
    }
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_BACKSPACE:
                    char* currentText = self->str;
                    size_t len = strlen(currentText);
                    if (len > 0 && self->cursor_index > 0) {
                        char* newStr = calloc(len, sizeof(char));
                        if (!newStr) {
                            error("Failed to allocate memory for InputBox string");
                            return;
                        }
                        memcpy(newStr, currentText, self->cursor_index - 1);
                        memcpy(newStr + self->cursor_index - 1, currentText + self->cursor_index, len - self->cursor_index + 1);
                        safe_free((void**)&self->str);
                        self->str = newStr;
                        Text_setString(self->text, newStr);
                        self->cursor_index--;
                    }
                    break;
            case SDL_SCANCODE_LEFT:
                    if (self->cursor_index > 0) {
                        self->cursor_index--;
                    }
                    break;
            case SDL_SCANCODE_RIGHT:
                    if (self->cursor_index < (int)strlen(self->str)) {
                        self->cursor_index++;
                    }
                default:
                    break;
            }
            break;
        case SDL_TEXTINPUT:
            const char* add = event->text.text;
            size_t add_len = add ? strlen(add) : 0;
            if (add_len > 1 ) {
                return;
            }
            size_t old_len = self->str ? strlen(self->str) : 0;
            char* new_str = calloc(old_len + add_len + 1, sizeof(char));
            if (!new_str) {
                error("Failed to allocate memory for InputBox string");
                return;
            }
            if (self->str && old_len > 0) {
                memcpy(new_str, self->str, self->cursor_index);
            }
            if (add_len > 0) {
                memcpy(new_str + self->cursor_index, add, add_len);
            }
            if (self->str && old_len > 0) {
                memcpy(new_str + self->cursor_index + add_len, self->str + self->cursor_index, old_len - self->cursor_index);
            }
            new_str[old_len + add_len] = '\0';
            if (self->str) {
                safe_free((void**)&self->str);
            }
            self->str = new_str;
            Text_setString(self->text, new_str);
            self->cursor_index++;
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Event type %d not recognized", event->type);
            break;
    }
}
