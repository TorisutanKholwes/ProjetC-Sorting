/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Text
 * @brief Holds text content, rendering resources and layout information.
 *
 * - text: Owned NUL-terminated string representing the current content.
 * - texture: SDL texture used for rendering the text.
 * - renderer: SDL_Renderer used to create and draw the texture.
 * - position: Position of the text; when fromCenter is true the position is the center.
 * - style: TextStyle describing font, color and style.
 * - fromCenter: If true the position is treated as the center of the rendered area.
 * - size: Current measured/rendered size of the texture.
 * - custom_size: When true the size has been manually set and won't be overwritten by texture metrics.
 */
struct Text {
    char* text;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    Position* position;
    TextStyle* style;
    bool fromCenter;
    Size size;
    bool custom_size;
};

/**
 * @brief Create a new Text object from a C-string.
 *
 * The returned Text is allocated on the heap and must be destroyed with Text_destroy.
 *
 * @param renderer SDL_Renderer used to create textures.
 * @param style TextStyle describing font and color (ownership typically transferred to the Text).
 * @param position Initial position (ownership typically transferred).
 * @param fromCenter If true the provided position is interpreted as the center.
 * @param str NUL-terminated C-string to render.
 * @return Pointer to a newly created Text or NULL on allocation failure.
 */
Text* Text_new(SDL_Renderer* renderer, TextStyle* style, Position* position, bool fromCenter, const char* str);

/**
 * @brief Create a new Text object using printf-style formatting.
 *
 * Works like Text_new but formats the initial string using the provided format and args.
 *
 * @param renderer SDL_Renderer used to create textures.
 * @param style TextStyle describing font and color.
 * @param position Initial position.
 * @param fromCenter If true the provided position is interpreted as the center.
 * @param format printf-style format string followed by arguments.
 * @return Pointer to a newly created Text or NULL on allocation failure.
 */
Text* Text_newf(SDL_Renderer* renderer, TextStyle* style, Position* position, bool fromCenter, const char* format, ...);

/**
 * @brief Destroy a Text object and free associated resources.
 *
 * Safe to call with NULL. This will destroy the SDL_Texture, free the stored
 * string and destroy owned style/position objects if applicable.
 *
 * @param self Text instance to destroy.
 */
void Text_destroy(Text* self);

/**
 * @brief Replace the text content and refresh the texture.
 *
 * The provided string is duplicated. If the content is unchanged the function returns early.
 *
 * @param self Text instance to update.
 * @param str New NUL-terminated string.
 */
void Text_setString(Text* self, const char* str);

/**
 * @brief Format and set the text content using printf-style formatting.
 *
 * @param self Text instance to update.
 * @param format printf-style format string followed by arguments.
 */
void Text_setStringf(Text* self, const char* format, ...);

/**
 * @brief Set the text color and refresh the texture.
 *
 * Ownership semantics for Color follow the project's conventions (the implementation
 * may transfer ownership or copy as needed).
 *
 * @param self Text instance to update.
 * @param color Color to apply to the text.
 */
void Text_setColor(Text* self, Color* color);

/**
 * @brief Set or update the Text position.
 *
 * If the Text has no Position the function will allocate a new one.
 *
 * @param self Text instance to update.
 * @param x New x coordinate.
 * @param y New y coordinate.
 */
void Text_setPosition(Text* self, float x, float y);

/**
 * @brief Render the Text using its SDL_Texture.
 *
 * If fromCenter is true the texture is drawn centered on the position.
 *
 * @param self Text instance to render.
 */
void Text_render(Text* self);

/**
 * @brief Get the current measured size of the Text.
 *
 * @param self Text instance to query.
 * @return Size structure with width and height, or {-1, -1} if self is NULL.
 */
Size Text_getSize(Text* self);

/**
 * @brief Manually set the rendered size of the Text.
 *
 * When a custom size is set the internal texture metrics won't override it.
 *
 * @param self Text instance to update.
 * @param width Desired width.
 * @param height Desired height.
 */
void Text_setSize(Text* self, float width, float height);
