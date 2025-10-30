/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct MapNode {
    void* key;
    void* value;
    MapNode* next;
    MapNode* prev;
};

struct Map {
    MapNode* root;
    size_t size;
    bool is_key_string;
};

struct MapIterator {
    MapNode* root;
    MapNode* current;
};

Map* Map_create(bool is_key_string);
void Map_destroy(Map* map);
void Map_clear(Map* map);
void Map_put(Map* map, void* key, void* value);
void* Map_get(Map* map, void* key);
MapNode* Map_find(Map* map, void* key);
bool Map_containsKey(Map* map, void* key);
bool Map_containsValue(Map* map, void* key);
void Map_remove(Map* map, void* key);
size_t Map_size(Map* map);
bool Map_isEmpty(Map* map);
char* Map_toString(Map* map, const char* keyFormat, const char* valueFormat, void* (*formatKeyFunc)(void* key), void* (*formatValueFunc)(void* value));

MapIterator* MapIterator_new(Map* map);
void MapIterator_destroy(MapIterator* iterator);
bool MapIterator_hasNext(MapIterator* iterator);
void MapIterator_next(MapIterator* iterator);
void* MapIterator_key(MapIterator* iterator);
void* MapIterator_value(MapIterator* iterator);

#define Map_put_int(map, key, value) Map_put(map, (void*)key, (void*)value)