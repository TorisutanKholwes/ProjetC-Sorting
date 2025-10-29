/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "resource_manager.h"

#include "logger.h"
#include "utils.h"
#include "map.h"

ResourceManager* ResourceManager_create(SDL_Renderer* renderer) {
    ResourceManager* self = calloc(1, sizeof(ResourceManager));
    if (!self) {
        error("Failed to allocate ResourceManager");
        return NULL;
    }
    self->renderer = renderer;
    self->texturesCache = Map_create(true);
    self->fontsCache = Map_create(true);
    self->soundsCache = Map_create(true);
    return self;
}

void ResourceManager_destroy(ResourceManager* self) {
    if (!self) return;

    if (self->texturesCache) {
        MapIterator* it = MapIterator_new(self->texturesCache);
        while (MapIterator_hasNext(it)) {
            MapIterator_next(it);
            void* key = MapIterator_key(it);
            void* value = MapIterator_value(it);
            SDL_DestroyTexture((SDL_Texture*)value);
            safe_free(&key);
        }
        MapIterator_destroy(it);
        Map_destroy(self->texturesCache);
    }

    if (self->fontsCache) {
        MapIterator* it = MapIterator_new(self->fontsCache);
        while (MapIterator_hasNext(it)) {
            MapIterator_next(it);
            void* key = MapIterator_key(it);
            Map* sizeMap = (Map*)MapIterator_value(it);
            if (sizeMap && Map_size(sizeMap) > 0) {
                MapIterator* sizeIt = MapIterator_new(sizeMap);
                while (MapIterator_hasNext(sizeIt)) {
                    MapIterator_next(sizeIt);
                    TTF_Font* font = MapIterator_value(sizeIt);
                    if (font) {
                        TTF_CloseFont(font);
                    }
                }
                MapIterator_destroy(sizeIt);
                Map_destroy(sizeMap);
            }
            safe_free(&key);
        }
        MapIterator_destroy(it);
        Map_destroy(self->fontsCache);
    }

    if (self->soundsCache) {
        MapIterator* it = MapIterator_new(self->soundsCache);
        while (MapIterator_hasNext(it)) {
            MapIterator_next(it);
            void* key = MapIterator_key(it);
            void* value = MapIterator_value(it);
            Mix_FreeChunk((Mix_Chunk*)value);
            safe_free(&key);
        }
        MapIterator_destroy(it);
        Map_destroy(self->soundsCache);
    }
    safe_free((void**)&self);
}

SDL_Texture* ResourceManager_getTexture(ResourceManager* self, const char* filename) {
    if (!self || !self->texturesCache || !filename) return NULL;

    if (Map_containsKey(self->texturesCache, (void*)filename)) {
        return Map_get(self->texturesCache, (void*)filename);
    }

    char* path = malloc(strlen(TEXTURE_PATH) + strlen(filename) + 1);
    sprintf(path, "%s%s", TEXTURE_PATH, filename);
    SDL_Texture* texture = IMG_LoadTexture(self->renderer, path);
    if (!texture) {
        error("Failed to load texture %s", path);
        safe_free((void**)&path);
        return NULL;
    }
    Map_put(self->texturesCache, Strdup(filename), texture);
    log_message(LOG_LEVEL_INFO, "Loaded new texture from %s", path);
    safe_free((void**)&path);
    return texture;
}

TTF_Font* ResourceManager_getFont(ResourceManager* self, const char* filename, int size) {
    if (!self || !self->fontsCache || !filename) return NULL;

    if (Map_containsKey(self->fontsCache, (void*)filename)) {
        Map* sizeMap = Map_get(self->fontsCache, (void*)filename);
        if (Map_containsKey(sizeMap, (void*)(long)size)) {
            return Map_get(sizeMap, (void*)(long)size);
        }
    }

    char* path = malloc(strlen(FONT_PATH) + strlen(filename) + 1);
    sprintf(path, "%s%s", FONT_PATH, filename);
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        error("Failed to load font %s", path);
        safe_free((void**)&path);
        return NULL;
    }

    if (Map_containsKey(self->fontsCache, (void*)filename)) {
        Map* existingFont = Map_get(self->fontsCache, (void*)filename);
        Map_put(existingFont, (void*)(long)size, font);
    } else {
        Map* sizeMap = Map_create(false);
        Map_put(sizeMap, (void*)(long)size, font);
        Map_put(self->fontsCache, Strdup(filename), sizeMap);
        log_message(LOG_LEVEL_INFO, "Loaded new font from %s", path);
    }
    safe_free((void**)&path);
    return font;
}

Mix_Chunk* ResourceManager_getSound(ResourceManager* self, const char* filename) {
    if (!self || !self->soundsCache || !filename) return NULL;

    if (Map_containsKey(self->soundsCache, (void*)filename)) {
        return Map_get(self->soundsCache, (void*)filename);
    }

    char* path = malloc(strlen(SOUND_PATH) + strlen(filename) + 1);
    sprintf(path, "%s%s", SOUND_PATH, filename);
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (!sound) {
        error("Failed to load sound %s", path);
        safe_free((void**)&path);
        return NULL;
    }
    Map_put(self->soundsCache, Strdup(filename), sound);
    log_message(LOG_LEVEL_INFO, "Loaded new sound from %s", path);
    safe_free((void**)&path);
    return sound;
}

TTF_Font* ResourceManager_getDefaultFont(ResourceManager* self, int size) {
    return ResourceManager_getFont(self, DEFAULT_FONT, size);
}

TTF_Font* ResourceManager_getDefaultBoldFont(ResourceManager* self, int size) {
    return ResourceManager_getFont(self, DEFAULT_BOLD_FONT, size);
}
