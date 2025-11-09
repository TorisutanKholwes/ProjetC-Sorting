/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct MapNode
 * @brief Node stored in the doubly-linked list backing the map.
 *
 * This node stores a key/value pair and pointers to the next and previous nodes.
 * Keys and values are stored as void\* and interpreted by the caller.
 */
struct MapNode {
    void* key;    /**< Pointer to the key (or C-string if map->is_key_string is true). */
    void* value;  /**< Pointer to the associated value. */
    MapNode* next; /**< Next node in the circular list. */
    MapNode* prev; /**< Previous node in the circular list. */
};

/**
 * @struct Map
 * @brief Map container implemented as a circular doubly-linked list with a root sentinel.
 *
 * The map does not perform deep copies of keys or values; the caller owns the memory.
 * The is_key_string flag enables string equality checks for keys instead of pointer equality.
 */
struct Map {
    MapNode* root;       /**< Sentinel root node. root->next == first, root->prev == last. */
    size_t size;         /**< Number of key/value entries currently stored. */
    bool is_key_string;  /**< If true, keys are compared as C-strings. */
};

/**
 * @struct MapIterator
 * @brief Simple iterator for traversing a Map in forward direction.
 *
 * The iterator maintains a reference to the map's root sentinel and the current node.
 */
struct MapIterator {
    MapNode* root;    /**< The map sentinel node used to detect end of iteration. */
    MapNode* current; /**< Current node; starts at root and advances with MapIterator_next. */
};

/**
 * @brief Create a new empty Map.
 *
 * @param is_key_string If true, keys are compared using string equality functions.
 * @return Pointer to newly allocated Map or NULL on allocation failure.
 */
Map* Map_create(bool is_key_string);

/**
 * @brief Destroy a Map and free associated memory.
 *
 * The function frees all internal nodes and the Map structure itself. It does not
 * free keys or values stored in the map; the caller is responsible for that if needed.
 *
 * @param map Pointer to the Map to destroy (may be NULL).
 */
void Map_destroy(Map* map);

/**
 * @brief Remove all entries from the Map.
 *
 * After this call the map is empty and ready for reuse. Keys/values are not freed.
 *
 * @param map Pointer to the Map to clear.
 */
void Map_clear(Map* map);

/**
 * @brief Insert or replace a key/value pair in the Map.
 *
 * If the key already exists (according to the map's key comparison mode), the value
 * is replaced. Otherwise a new node is appended to the list.
 *
 * @param map Pointer to the Map.
 * @param key Pointer to the key (or C-string if is_key_string is true).
 * @param value Pointer to the associated value.
 */
void Map_put(Map* map, void* key, void* value);

/**
 * @brief Retrieve the value associated with a key.
 *
 * @param map Pointer to the Map.
 * @param key Key to look up.
 * @return Pointer to the value if found, or NULL if the key is not present.
 */
void* Map_get(Map* map, void* key);

/**
 * @brief Find the MapNode containing the given key.
 *
 * @param map Pointer to the Map.
 * @param key Key to locate.
 * @return Pointer to the MapNode if found, otherwise NULL.
 */
MapNode* Map_find(Map* map, void* key);

/**
 * @brief Check whether a key exists in the Map.
 *
 * @param map Pointer to the Map.
 * @param key Key to test for.
 * @return true if the key exists, false otherwise.
 */
bool Map_containsKey(Map* map, void* key);

/**
 * @brief Check whether a value exists in the Map.
 *
 * Value comparison is pointer equality.
 *
 * @param map Pointer to the Map.
 * @param value Value to test for.
 * @return true if the value exists, false otherwise.
 */
bool Map_containsValue(Map* map, void* value);

/**
 * @brief Remove the entry with the specified key from the Map.
 *
 * The node is freed but the key and value pointers are not freed.
 *
 * @param map Pointer to the Map.
 * @param key Key of the entry to remove.
 */
void Map_remove(Map* map, void* key);

/**
 * @brief Get the number of entries in the Map.
 *
 * @param map Pointer to the Map.
 * @return Number of stored entries.
 */
size_t Map_size(Map* map);

/**
 * @brief Test whether the Map is empty.
 *
 * @param map Pointer to the Map.
 * @return true if the map is empty, false otherwise.
 */
bool Map_isEmpty(Map* map);

/**
 * @brief Produce a string representation of the Map.
 *
 * The returned string is heap-allocated and must be freed by the caller.
 * The function accepts printf-style format strings for keys and values and
 * optional formatter callbacks that transform key/value pointers for printing.
 *
 * @param map Pointer to the Map.
 * @param keyFormat printf-style format for keys (e.g. "%s").
 * @param valueFormat printf-style format for values (e.g. "%d", "%f").
 * @param formatKeyFunc Optional function to convert the key pointer to a printable pointer.
 * @param formatValueFunc Optional function to convert the value pointer to a printable pointer.
 * @return Newly allocated null-terminated string representation of the map.
 */
char* Map_toString(Map* map, const char* keyFormat, const char* valueFormat, void* (*formatKeyFunc)(void* key), void* (*formatValueFunc)(void* value));

/**
 * @brief Create a new iterator for a Map.
 *
 * The iterator must be destroyed with MapIterator_destroy when no longer needed.
 *
 * @param map Pointer to the Map to iterate.
 * @return Pointer to a new MapIterator or NULL on allocation failure.
 */
MapIterator* MapIterator_new(Map* map);

/**
 * @brief Destroy a MapIterator.
 *
 * @param iterator Pointer to the iterator to destroy (may be NULL).
 */
void MapIterator_destroy(MapIterator* iterator);

/**
 * @brief Check if the iterator can advance to the next element.
 *
 * @param iterator Pointer to the MapIterator.
 * @return true if there is a next element, false otherwise.
 */
bool MapIterator_hasNext(MapIterator* iterator);

/**
 * @brief Advance the iterator to the next element.
 *
 * Does nothing if there is no next element.
 *
 * @param iterator Pointer to the MapIterator.
 */
void MapIterator_next(MapIterator* iterator);

/**
 * @brief Get the key at the iterator's current position.
 *
 * @param iterator Pointer to the MapIterator.
 * @return Pointer to the key at the current node.
 */
void* MapIterator_key(MapIterator* iterator);

/**
 * @brief Get the value at the iterator's current position.
 *
 * @param iterator Pointer to the MapIterator.
 * @return Pointer to the value at the current node.
 */
void* MapIterator_value(MapIterator* iterator);

/**
 * @def Map_put_int
 * @brief Convenience macro to store integer keys/values by casting to void\*.
 *
 * WARNING: This macro performs pointer casts from integer types to void\* and
 * back. It is suitable only for storing small integer values on platforms where
 * such casts are safe and intended. Use with caution.
 */
#define Map_put_int(map, key, value) Map_put(map, (void*)key, (void*)value)
