/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct EdgeInsets
 * @brief Rectangular padding values (top, bottom, left, right).
 *
 * Values are expressed in floating point and are used for layout calculations.
 */
struct EdgeInsets {
    float top, bottom, left, right;
};

/**
 * @def EdgeInsets_zero
 * @brief Create an EdgeInsets with all zeros.
 */
#define EdgeInsets_zero() EdgeInsets_new(0, 0, 0, 0)

/**
 * @def EdgeInsets_newAll
 * @brief Create an EdgeInsets with the same value on all sides.
 */
#define EdgeInsets_newAll(value) EdgeInsets_new(value, value, value, value)

/**
 * @def EdgeInsets_newSymmetric
 * @brief Create an EdgeInsets with symmetric vertical and horizontal values.
 *
 * The first parameter maps to top and bottom, the second to left and right.
 */
#define EdgeInsets_newSymmetric(vertical, horizontal) EdgeInsets_new(vertical, vertical, horizontal, horizontal)

/**
 * @brief Allocate and initialize an EdgeInsets instance.
 *
 * @param top Top padding.
 * @param bottom Bottom padding.
 * @param left Left padding.
 * @param right Right padding.
 * @return Pointer to newly allocated EdgeInsets or NULL on failure.
 */
EdgeInsets* EdgeInsets_new(float top, float bottom, float left, float right);

/**
 * @brief Free an EdgeInsets instance.
 *
 * Safe to call with NULL.
 *
 * @param insets Pointer to EdgeInsets to destroy.
 */
void EdgeInsets_destroy(EdgeInsets* insets);

/**
 * @struct TextStyle
 * @brief Describes font, size, color and rendering style for text.
 */
struct TextStyle {
    TTF_Font* font; /**< Font pointer (not owned by this struct in all cases). */
    int size;       /**< Font size in points. */
    Color* color;   /**< Text color (owned). */
    int style;      /**< SDL_ttf style flags (e.g. TTF_STYLE_BOLD). */
};

/**
 * @brief Create a new TextStyle.
 *
 * The created TextStyle owns the provided Color pointer.
 *
 * @param font Font to use.
 * @param size Font size.
 * @param color Color object (ownership transferred).
 * @param style SDL_ttf style flags.
 * @return Newly allocated TextStyle or NULL on allocation failure.
 */
TextStyle* TextStyle_new(TTF_Font* font, int size, Color* color, int style);

/**
 * @brief Destroy a TextStyle and free owned resources.
 *
 * Safe to call with NULL.
 *
 * @param style Pointer to TextStyle to destroy.
 */
void TextStyle_destroy(TextStyle* style);

/**
 * @brief Create a default TextStyle using the provided ResourceManager.
 *
 * @param resource_manager Resource manager used to obtain default fonts.
 * @return Newly allocated TextStyle or NULL on failure.
 */
TextStyle* TextStyle_default(ResourceManager* resource_manager);

/**
 * @brief Create a TextStyle adapted from a Theme.
 *
 * @param theme Theme providing color information.
 * @param resource_manager Resource manager used to obtain default fonts.
 * @return Newly allocated TextStyle or NULL on failure.
 */
TextStyle* TextStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager);

/**
 * @brief Deep copy a TextStyle.
 *
 * Returns a newly allocated TextStyle with a copied Color.
 *
 * @param style Source TextStyle to copy.
 * @return Newly allocated TextStyle copy or NULL if style is NULL.
 */
TextStyle* TextStyle_deepCopy(TextStyle* style);

/**
 * @struct FullStyleColors
 * @brief Group of three colors used by many controls: background, border and text.
 */
struct FullStyleColors {
    Color* background;
    Color* border;
    Color* text;
};

/**
 * @brief Create a FullStyleColors instance.
 *
 * Ownership of the provided Color pointers is transferred to the returned object.
 *
 * @param background Background color (owned).
 * @param border Border color (owned).
 * @param text Text color (owned).
 * @return Newly allocated FullStyleColors or NULL on failure.
 */
FullStyleColors* FullStyleColors_new(Color* background, Color* border, Color* text);

/**
 * @brief Destroy a FullStyleColors instance and free owned colors.
 *
 * Safe to call with NULL.
 *
 * @param colors Pointer to FullStyleColors to destroy.
 */
void FullStyleColors_destroy(FullStyleColors* colors);

/**
 * @struct ButtonStyle
 * @brief Visual configuration for buttons and similar controls.
 *
 * Contains color grouping, border width, text font/size/style and paddings.
 */
struct ButtonStyle {
    FullStyleColors* colors; /**< Owned color group. */
    int border_width;
    TTF_Font* text_font;     /**< Font used for the button label (not always owned). */
    int text_style;
    int text_size;
    EdgeInsets* paddings;    /**< Owned paddings object. */
};

/**
 * @brief Create a ButtonStyle instance.
 *
 * Ownership of colors and paddings is transferred to the returned object.
 *
 * @param colors FullStyleColors pointer (owned).
 * @param border_width Border thickness in pixels.
 * @param text_font Font used for labels.
 * @param text_style Font rendering style flags.
 * @param text_size Font size in points.
 * @param paddings EdgeInsets pointer (owned).
 * @return Newly allocated ButtonStyle or NULL on allocation failure.
 */
ButtonStyle* ButtonStyle_new(FullStyleColors* colors, int border_width, TTF_Font* text_font, int text_style, int text_size, EdgeInsets* paddings);

/**
 * @brief Destroy a ButtonStyle and free owned resources.
 *
 * Safe to call with NULL.
 *
 * @param style ButtonStyle to destroy.
 */
void ButtonStyle_destroy(ButtonStyle* style);

/**
 * @brief Create a default ButtonStyle using the ResourceManager.
 *
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated ButtonStyle or NULL on failure.
 */
ButtonStyle* ButtonStyle_default(ResourceManager* resource_manager);

/**
 * @brief Create a ButtonStyle adapted from a Theme.
 *
 * @param theme Theme providing color information.
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated ButtonStyle or NULL on failure.
 */
ButtonStyle* ButtonStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager);

/**
 * @brief Deep copy a ButtonStyle (copies colors and paddings).
 *
 * @param style Source ButtonStyle to copy.
 * @return Newly allocated ButtonStyle copy or NULL on failure.
 */
ButtonStyle* ButtonStyle_deepCopy(ButtonStyle* style);

/**
 * @brief Default style used by Select controls.
 *
 * Convenience factory similar to ButtonStyle_default but tuned for select widgets.
 *
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated ButtonStyle for Select controls.
 */
ButtonStyle* SelectStyle_default(ResourceManager* resource_manager);

/**
 * @brief Default Select style derived from a Theme.
 *
 * @param theme Theme providing colors.
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated ButtonStyle for Select controls.
 */
ButtonStyle* SelectStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager);

/**
 * @struct InputBoxStyle
 * @brief Style configuration for input boxes (text fields).
 */
struct InputBoxStyle {
    TTF_Font* font;           /**< Font used for input text. */
    int text_size;
    int style;                /**< Font rendering style flags. */
    FullStyleColors* colors;  /**< Owned color group for input box. */
};

/**
 * @brief Create a new InputBoxStyle.
 *
 * Ownership of colors is transferred to the returned object.
 *
 * @param font Font to use.
 * @param text_size Font size.
 * @param style Font style flags.
 * @param colors FullStyleColors pointer (owned).
 * @return Newly allocated InputBoxStyle or NULL on failure.
 */
InputBoxStyle* InputBoxStyle_new(TTF_Font* font, int text_size, int style, FullStyleColors* colors);

/**
 * @brief Destroy an InputBoxStyle and free owned resources.
 *
 * Safe to call with NULL.
 *
 * @param style InputBoxStyle to destroy.
 */
void InputBoxStyle_destroy(InputBoxStyle* style);

/**
 * @brief Create a default InputBoxStyle using the ResourceManager.
 *
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated InputBoxStyle or NULL on failure.
 */
InputBoxStyle* InputBoxStyle_default(ResourceManager* resource_manager);

/**
 * @brief Create an InputBoxStyle adapted from a Theme.
 *
 * @param theme Theme providing color information.
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated InputBoxStyle or NULL on failure.
 */
InputBoxStyle* InputBoxStyle_defaultFromTheme(Theme* theme, ResourceManager* resource_manager);

/**
 * @struct Theme
 * @brief Top-level theme grouping colors and common styles used by the UI.
 *
 * The Theme object typically owns its contained styles and colors.
 */
struct Theme {
    Color* background;
    Color* primary;
    Color* secondary;

    TextStyle* title_style;
    TextStyle* body_style;

    ButtonStyle* button_style;
};

/**
 * @brief Create a new Theme instance.
 *
 * Ownership of provided pointers is transferred to the Theme.
 *
 * @param background Background color (owned).
 * @param primary Primary color (owned).
 * @param secondary Secondary color (owned).
 * @param title_style Title TextStyle (owned).
 * @param body_style Body TextStyle (owned).
 * @param button_style ButtonStyle (owned).
 * @return Newly allocated Theme or NULL on allocation failure.
 */
Theme* Theme_new(Color* background, Color* primary, Color* secondary, TextStyle* title_style, TextStyle* body_style, ButtonStyle* button_style);

/**
 * @brief Destroy a Theme and free all owned resources.
 *
 * Safe to call with NULL.
 *
 * @param theme Theme to destroy.
 */
void Theme_destroy(Theme* theme);

/**
 * @brief Create a default Theme populated with sensible defaults.
 *
 * Uses the ResourceManager to acquire default fonts.
 *
 * @param resource_manager Resource manager used to obtain fonts.
 * @return Newly allocated Theme or NULL on failure.
 */
Theme* Theme_default(ResourceManager* resource_manager);
