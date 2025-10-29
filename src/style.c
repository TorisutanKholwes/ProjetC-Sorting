/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "style.h"

#include "color.h"
#include "logger.h"
#include "resource_manager.h"
#include "utils.h"

EdgeInsets* EdgeInsets_new(const float top, const float bottom, const float left, const float right) {
    EdgeInsets* insets = calloc(1, sizeof(EdgeInsets));
    if (!insets) {
        error("Failed to allocate memory for EdgeInsets");
        return NULL;
    }
    insets->top = top;
    insets->bottom = bottom;
    insets->left = left;
    insets->right = right;
    return insets;
}

void EdgeInsets_destroy(EdgeInsets* insets) {
    if (!insets) return;
    safe_free((void**)&insets);
}

TextStyle* TextStyle_new(TTF_Font* font, int size, Color* color, int style) {
    TextStyle* text_style = calloc(1, sizeof(TextStyle));
    if (!text_style) {
        error("Failed to allocate memory for TextStyle");
        return NULL;
    }
    text_style->font = font;
    text_style->size = size;
    text_style->color = color;
    text_style->style = style;
    return text_style;
}

void TextStyle_destroy(TextStyle* style) {
    if (!style) return;
    safe_free((void**)&style->color);
    safe_free((void**)&style);
}

TextStyle* TextStyle_default(ResourceManager* resource_manager) {
    TextStyle* style = calloc(1, sizeof(TextStyle));
    if (!style) {
        error("Failed to allocate memory for default TextStyle");
        return NULL;
    }
    style->size = 32;
    style->font = ResourceManager_getDefaultFont(resource_manager, 32);
    style->color = Color_rgb(255, 255, 255);
    style->style = TTF_STYLE_NORMAL;
    return style;
}

TextStyle* TextStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager) {
    TextStyle* style = calloc(1, sizeof(TextStyle));
    if (!style) {
        error("Failed to allocate memory for default TextStyle from Theme");
        return NULL;
    }
    style->size = 32;
    style->font = ResourceManager_getDefaultFont(resource_manager, style->size);
    style->color = theme->primary;
    style->style = TTF_STYLE_NORMAL;
    return style;
}

FullStyleColors* FullStyleColors_new(Color* background, Color* border, Color* text) {
    FullStyleColors* colors = calloc(1, sizeof(FullStyleColors));
    if (!colors) {
        error("Failed to allocate memory for FullStyleColors");
        return NULL;
    }
    colors->background = background;
    colors->border = border;
    colors->text = text;
    return colors;
}

void FullStyleColors_destroy(FullStyleColors* colors) {
    if (!colors) return;
    if (colors->background) {
        safe_free((void**)&colors->background);
    }
    if (colors->border) {
        safe_free((void**)&colors->border);
    }
    if (colors->text) {
        safe_free((void**)&colors->text);
    }
    safe_free((void**)&colors);
}

ButtonStyle* ButtonStyle_new(FullStyleColors* colors, int border_width, TTF_Font* text_font, int text_style, int text_size, EdgeInsets* paddings) {
    ButtonStyle* style = calloc(1, sizeof(ButtonStyle));
    if (!style) {
        error("Failed to allocate memory for ButtonStyle");
        return NULL;
    }
    style->colors = colors;
    style->border_width = border_width;
    style->text_font = text_font;
    style->text_style = text_style;
    style->text_size = text_size;
    style->paddings = paddings;
    return style;
}

void ButtonStyle_destroy(ButtonStyle* style) {
    if (!style) return;
    FullStyleColors_destroy(style->colors);
    EdgeInsets_destroy(style->paddings);
    safe_free((void**)&style);
}

ButtonStyle* ButtonStyle_default(ResourceManager* resource_manager) {
    ButtonStyle* style = calloc(1, sizeof(ButtonStyle));
    if (!style) {
        error("Failed to allocate memory for default ButtonStyle");
        return NULL;
    }
    style->border_width = 2;
    style->text_size = 24;
    style->text_font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->text_style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
        Color_rgb(220, 220, 220),
        Color_rgb(100, 100, 100),
        Color_rgb(0, 0, 0));
    style->paddings = EdgeInsets_newSymmetric(10, 20);
    return style;
}

ButtonStyle* ButtonStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager) {
    ButtonStyle* style = calloc(1, sizeof(ButtonStyle));
    if (!style) {
        error("Failed to allocate memory for default ButtonStyle from Theme");
        return NULL;
    }
    style->border_width = 2;
    style->text_size = 32;
    style->text_font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->text_style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
        theme->secondary,
        theme->primary,
        theme->background);
    style->paddings = EdgeInsets_newSymmetric(10, 20);
    return style;
}

ButtonStyle* ButtonStyle_deepCopy(ButtonStyle* style) {
    return ButtonStyle_new(
        FullStyleColors_new(
            Color_copy(style->colors->background),
            Color_copy(style->colors->border),
            Color_copy(style->colors->text)),
        style->border_width,
        style->text_font,
        style->text_style,
        style->text_size,
        EdgeInsets_new(
            style->paddings->top,
            style->paddings->bottom,
            style->paddings->left,
            style->paddings->right));
}

ButtonStyle* SelectStyle_default(ResourceManager* resource_manager) {
    ButtonStyle* style = calloc(1, sizeof(ButtonStyle));
    if (!style) {
        error("Failed to allocate memory for default ButtonStyle");
        return NULL;
    }
    style->border_width = 2;
    style->text_size = 24;
    style->text_font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->text_style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
    Color_fromHex("#1a2a4a"),
    Color_fromHex("#3a4a7a"),
    Color_rgb(255, 255, 255));
    style->paddings = EdgeInsets_newSymmetric(10, 60);
    return style;
}

ButtonStyle* SelectStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager) {
    ButtonStyle* style = calloc(1, sizeof(ButtonStyle));
    if (!style) {
        error("Failed to allocate memory for default ButtonStyle from Theme");
        return NULL;
    }
    style->border_width = 2;
    style->text_size = 32;
    style->text_font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->text_style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
        theme->secondary,
        theme->primary,
        theme->background);
    style->paddings = EdgeInsets_newSymmetric(10, 20);
    return style;
}

InputBoxStyle* InputBoxStyle_new(TTF_Font* font, int text_size, int style, FullStyleColors* colors) {
    InputBoxStyle* self = calloc(1, sizeof(InputBoxStyle));
    if (!self) {
        error("Failed to allocate memory for InputBoxStyle");
        return NULL;
    }
    self->font = font;
    self->text_size = text_size;
    self->style = style;
    self->colors = colors;
    return self;
}

void InputBoxStyle_destroy(InputBoxStyle* style) {
    if (!style) return;
    FullStyleColors_destroy(style->colors);
    safe_free((void**)&style);
}

InputBoxStyle* InputBoxStyle_default(ResourceManager* resource_manager) {
    InputBoxStyle* style = calloc(1, sizeof(InputBoxStyle));
    if (!style) {
        error("Failed to allocate memory for default InputBoxStyle");
        return NULL;
    }
    style->text_size = 32;
    style->font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
        Color_rgb(255, 255, 255),
        Color_rgb(0, 0, 0),
        Color_rgb(0, 0, 0));
    return style;
}

InputBoxStyle* InputBoxStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager) {
    InputBoxStyle* style = calloc(1, sizeof(InputBoxStyle));
    if (!style) {
        error("Failed to allocate memory for default InputBoxStyle from Theme");
        return NULL;
    }
    style->text_size = 32;
    style->font = ResourceManager_getDefaultFont(resource_manager, style->text_size);
    style->style = TTF_STYLE_NORMAL;
    style->colors = FullStyleColors_new(
        theme->background,
        theme->primary,
        theme->primary);
    return style;
}

Theme* Theme_new(Color* background, Color* primary, Color* secondary, TextStyle* title_style, TextStyle* body_style, ButtonStyle* button_style) {
    Theme* theme = calloc(1, sizeof(Theme));
    if (!theme) {
        error("Failed to allocate memory for Theme");
        return NULL;
    }
    theme->background = background;
    theme->primary = primary;
    theme->secondary = secondary;
    theme->title_style = title_style;
    theme->body_style = body_style;
    theme->button_style = button_style;
    return theme;
}

void Theme_destroy(Theme* theme) {
    if (!theme) return;
    safe_free((void**)&theme->background);
    safe_free((void**)&theme->primary);
    safe_free((void**)&theme->secondary);
    TextStyle_destroy(theme->title_style);
    TextStyle_destroy(theme->body_style);
    ButtonStyle_destroy(theme->button_style);
    safe_free((void**)&theme);
}
Theme* Theme_default(ResourceManager* resource_manager) {
    Theme* theme = calloc(1, sizeof(Theme));
    if (!theme) {
        error("Failed to allocate memory for default Theme");
        return NULL;
    }
    theme->background = COLOR_GRAY(50);
    theme->primary = Color_rgb(255, 255, 255);
    theme->secondary = COLOR_GRAY(200);
    theme->title_style = TextStyle_new(ResourceManager_getDefaultBoldFont(resource_manager, 48), 48, Color_rgb(255, 255, 255), TTF_STYLE_UNDERLINE);
    theme->body_style = TextStyle_default(resource_manager);
    theme->button_style = ButtonStyle_default(resource_manager);
    return theme;
}