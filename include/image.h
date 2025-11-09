/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"


/**
 * @struct Image
 * @brief Represents a drawable image backed by an SDL_Texture.
 *
 * The Image structure stores an SDL_Texture pointer along with position,
 * size, scaling ratio and flags controlling rendering origin and custom sizing.
 */
struct Image {
    /** @brief SDL texture used for rendering. */
    SDL_Texture* texture;

    /** @brief Position of the image (top-left or center depending on from_center). */
    Position* position;

    /** @brief Native or custom size of the image in pixels. */
    Size size;

    /**
     * @brief Scaling ratio applied to the native texture size when custom_size is false.
     *
     * Final rendered size = size * ratio when custom_size is false.
     */
    float ratio;

    /**
     * @brief If true, position is treated as the texture center; otherwise top-left.
     */
    bool from_center;

    /**
     * @brief If true, size is treated as a custom explicit size and ratio is ignored.
     */
    bool custom_size;
};

/**
 * @brief Create a new Image from an existing SDL_Texture.
 *
 * The returned Image takes ownership of metadata only; texture lifetime is
 * managed externally (see implementation semantics).
 *
 * @param texture Pointer to an SDL_Texture (must be valid).
 * @param position Pointer to a Position struct (ownership per implementation).
 * @param from_center If true, position is used as the texture center.
 * @return Pointer to a newly allocated Image or NULL on allocation failure.
 */
Image* Image_new(SDL_Texture* texture, Position* position, bool from_center);

/**
 * @brief Load an image texture via the App's resource manager and create an Image.
 *
 * Uses ResourceManager_getTexture to obtain the SDL_Texture for the provided path.
 *
 * @param app Pointer to the App instance (provides resource manager).
 * @param path Filesystem or resource path to load the texture from.
 * @param position Pointer to a Position struct to place the image.
 * @param from_center If true, position is used as the texture center.
 * @return Pointer to a newly allocated Image or NULL on failure.
 */
Image* Image_load(App* app, const char* path, Position* position, bool from_center);

/**
 * @brief Destroy an Image and free its associated resources.
 *
 * Does not necessarily destroy the underlying SDL_Texture (implementation defined).
 * Safe to call with NULL.
 *
 * @param self Pointer to the Image to destroy.
 */
void Image_destroy(Image* self);

/**
 * @brief Render the image using the provided SDL_Renderer.
 *
 * Respects the from_center and custom_size flags, applying ratio when appropriate.
 * If any required pointer is NULL (self, renderer, or texture), the call is ignored.
 *
 * @param self Pointer to the Image to render.
 * @param renderer SDL_Renderer used to draw the texture.
 */
void Image_render(Image* self, SDL_Renderer* renderer);

/**
 * @brief Replace the SDL_Texture for an existing Image and update stored size.
 *
 * Queries the new texture's width and height to update the Image size and sets
 * texture scaling mode to SDL_ScaleModeBest.
 *
 * @param self Pointer to the Image to update.
 * @param texture New SDL_Texture pointer.
 */
void Image_setTexture(Image* self, SDL_Texture* texture);

/**
 * @brief Change the Image texture by loading a new path via the App's resource manager.
 *
 * On failure the image remains unchanged.
 *
 * @param self Pointer to the Image to update.
 * @param app Pointer to the App instance providing the resource manager.
 * @param path Resource path to load the new texture from.
 */
void Image_changePath(Image* self, App* app, const char* path);

/**
 * @brief Set an explicit custom size for the image.
 *
 * After calling this, the image will render using the provided width/height
 * regardless of the original texture size; ratio is ignored while custom_size is true.
 *
 * @param self Pointer to the Image.
 * @param width Desired rendered width in pixels.
 * @param height Desired rendered height in pixels.
 */
void Image_setSize(Image* self, float width, float height);

/**
 * @brief Get the current rendering size of the Image.
 *
 * If custom_size is false, returns the native size multiplied by ratio.
 * If self is NULL, returns {-1, -1}.
 *
 * @param self Pointer to the Image.
 * @return Size struct with width and height to be used for rendering.
 */
Size Image_getSize(Image* self);

/**
 * @brief Set or update the image position.
 *
 * If the Image has no Position allocated, a new Position may be created by the
 * implementation; otherwise the existing Position is updated.
 *
 * @param self Pointer to the Image.
 * @param x X coordinate (top-left or center depending on from_center).
 * @param y Y coordinate.
 */
void Image_setPosition(Image* self, float x, float y);

/**
 * @brief Set the scaling ratio applied when custom_size is false.
 *
 * @param self Pointer to the Image.
 * @param ratio Scaling ratio (1.0 = native size).
 */
void Image_setRatio(Image* self, float ratio);
