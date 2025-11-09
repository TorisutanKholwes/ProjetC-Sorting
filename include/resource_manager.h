/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @def ASSETS_PATH
 * @brief Base path where asset folders are located.
 *
 * Paths below are joined with asset filenames to locate resources on disk.
 */
#define ASSETS_PATH "../assets/"

/**
 * @def TEXTURE_PATH
 * @brief Directory for image assets.
 */
#define TEXTURE_PATH ASSETS_PATH "images/"

/**
 * @def FONT_PATH
 * @brief Directory for font assets.
 */
#define FONT_PATH    ASSETS_PATH "fonts/"

/**
 * @def SOUND_PATH
 * @brief Directory for sound assets.
 */
#define SOUND_PATH   ASSETS_PATH "sounds/"

/**
 * @def DEFAULT_FONT
 * @brief Filename of the default regular font.
 */
#define DEFAULT_FONT "Inter-Regular.ttf"

/**
 * @def DEFAULT_BOLD_FONT
 * @brief Filename of the default bold font.
 */
#define DEFAULT_BOLD_FONT "Inter-Bold.ttf"

/**
 * @struct ResourceManager
 * @brief Holds renderer and asset caches used by the application.
 *
 * The \c ResourceManager owns maps for textures, fonts and sounds. Keys are
 * stored as duplicated strings (caller must not free them). Values are the
 * platform-specific pointers (SDL_Texture\*, TTF_Font\*, Mix_Chunk\*).
 */
struct ResourceManager {
    SDL_Renderer* renderer; /**< SDL renderer used to create textures. */
    Map* texturesCache;     /**< Map<string, SDL_Texture*> for loaded textures. */
    Map* fontsCache;        /**< Map<string, Map<int, TTF_Font*>> grouping fonts by filename and size. */
    Map* soundsCache;       /**< Map<string, Mix_Chunk*> for loaded sound effects. */
};

/**
 * @brief Create and initialize a new ResourceManager.
 *
 * Allocates internal caches and stores the provided SDL_Renderer pointer.
 *
 * @param renderer Pointer to an initialized SDL_Renderer used when loading textures.
 * @return Pointer to a newly allocated ResourceManager, or NULL on allocation failure.
 */
ResourceManager* ResourceManager_create(SDL_Renderer* renderer);

/**
 * @brief Destroy a ResourceManager and free its resources.
 *
 * This will free cached SDL_Texture, TTF_Font and Mix_Chunk objects, destroy
 * internal maps and free the ResourceManager structure itself.
 *
 * @param self Pointer to the ResourceManager to destroy (may be NULL).
 */
void ResourceManager_destroy(ResourceManager* self);

/**
 * @brief Load or retrieve a cached texture.
 *
 * If the texture identified by \p filename is already cached, the cached
 * SDL_Texture\* is returned. Otherwise the texture is loaded from
 * TEXTURE_PATH + filename, cached and returned.
 *
 * @param self Resource manager instance.
 * @param filename Basename of the texture file (UTF-8 C-string).
 * @return SDL_Texture\* on success, or NULL on failure.
 */
SDL_Texture* ResourceManager_getTexture(ResourceManager* self, const char* filename);

/**
 * @brief Load or retrieve a cached font of a given size.
 *
 * Fonts are cached per filename and size. If a font of the requested size is
 * already cached it is returned; otherwise it is loaded from FONT_PATH and
 * stored in the cache.
 *
 * @param self Resource manager instance.
 * @param filename Basename of the font file.
 * @param size Font point size to load.
 * @return TTF_Font\* on success, or NULL on failure.
 */
TTF_Font* ResourceManager_getFont(ResourceManager* self, const char* filename, int size);

/**
 * @brief Load or retrieve a cached sound effect.
 *
 * Sounds are loaded from SOUND_PATH + filename and cached as Mix_Chunk\*.
 *
 * @param self Resource manager instance.
 * @param filename Basename of the WAV file.
 * @return Mix_Chunk\* on success, or NULL on failure.
 */
Mix_Chunk* ResourceManager_getSound(ResourceManager* self, const char* filename);

/**
 * @brief Convenience accessor for the application's default font.
 *
 * Delegates to ResourceManager_getFont with DEFAULT_FONT.
 *
 * @param self Resource manager instance.
 * @param size Desired font size.
 * @return TTF_Font\* on success, or NULL on failure.
 */
TTF_Font* ResourceManager_getDefaultFont(ResourceManager* self, int size);

/**
 * @brief Convenience accessor for the application's default bold font.
 *
 * Delegates to ResourceManager_getFont with DEFAULT_BOLD_FONT.
 *
 * @param self Resource manager instance.
 * @param size Desired font size.
 * @return TTF_Font\* on success, or NULL on failure.
 */
TTF_Font* ResourceManager_getDefaultBoldFont(ResourceManager* self, int size);
