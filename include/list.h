/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct ListNode
 * @brief Node used by the doubly-linked circular List.
 *
 * - prev: pointer to the previous node.
 * - next: pointer to the next node.
 * - value: stored value (opaque pointer).
 */
struct ListNode {
    ListNode* prev;
    ListNode* next;
    void* value;
};

/**
 * @struct List
 * @brief Doubly-linked circular list with a sentinel head node.
 *
 * - head: sentinel node whose next/prev link the list contents.
 * - size: number of elements currently stored in the list.
 */
struct List {
    ListNode* head;
    size_t size;
};

/**
 * @struct ListIterator
 * @brief Forward iterator for traversing a List.
 *
 * - head: sentinel head node of the iterated list.
 * - current: next node to return by the iterator.
 * - index: current index (incremented after returning an element).
 * - size: snapshot of the list size at iterator creation.
 */
struct ListIterator {
    ListNode* head;
    ListNode* current;
    int index;
    size_t size;
};

/**
 * @brief Create and initialize a new empty List.
 *
 * Allocates the list structure and a sentinel head ListNode. The caller owns
 * the returned List and should call List_destroy() when no longer needed.
 *
 * @return Pointer to newly allocated List or NULL on allocation failure.
 */
List* List_create();

/**
 * @brief Destroy a List and free internal structures.
 *
 * Does not call destructors for stored values. Safe to call with NULL.
 *
 * @param list Pointer to List to destroy.
 */
void List_destroy(List* list);

/**
 * @brief Destroy the list and call a destructor for each stored value.
 *
 * Iterates values and calls \p destroyValueFunc on each non-NULL value before
 * freeing list internals.
 *
 * @param list Pointer to List to destroy.
 * @param destroyValueFunc Function called for each stored value (may be NULL).
 */
void List_destroyWitValues(List* list, void (*destroyValueFunc)(void* value));

/**
 * @brief Remove and free all nodes from the list but keep the list itself.
 *
 * Leaves the sentinel head node and resets size to 0.
 *
 * @param list Pointer to List to clear.
 */
void List_clear(List* list);

/**
 * @brief Append a value to the end of the list.
 *
 * The list stores the opaque pointer; ownership semantics are user-defined.
 *
 * @param list Pointer to List.
 * @param value Opaque pointer to store.
 */
void List_push(List* list, void* value);

/**
 * @brief Remove the first node whose value pointer equals \p value.
 *
 * Frees the removed node but does not free the stored value.
 *
 * @param list Pointer to List.
 * @param value Value pointer to remove.
 */
void List_remove(List* list, void* value);

/**
 * @brief Convenience macro to pop the last element.
 *
 * Expands to List_pop(list, list->size - 1).
 */
#define List_popLast(list) List_pop(list, list->size - 1)

/**
 * @brief Remove and return the value at the given index.
 *
 * On success returns the stored pointer and removes the node from the list.
 * If index is out of range NULL is returned.
 *
 * @param list Pointer to List.
 * @param index Zero-based index of element to remove.
 * @return Stored pointer previously at index or NULL on error.
 */
void* List_pop(List* list, size_t index);

/**
 * @brief Return the number of elements contained in the list.
 *
 * @param list Pointer to List.
 * @return Number of elements.
 */
size_t List_size(List* list);

/**
 * @brief Check whether the list is empty.
 *
 * @param list Pointer to List.
 * @return true if size == 0, false otherwise.
 */
bool List_empty(List* list);

/**
 * @brief Check whether the list contains a value.
 *
 * If \p isString is true, values are compared using String_equals; otherwise
 * pointer equality is used.
 *
 * @param list Pointer to List.
 * @param value Value to search for.
 * @param isString Whether to treat values as C strings for comparison.
 * @return true if found, false otherwise.
 */
bool List_contains(List* list, void* value, bool isString);

/**
 * @brief Retrieve the value at a given zero-based index.
 *
 * Returns NULL and logs an error if index is out of bounds.
 *
 * @param list Pointer to List.
 * @param index Index to retrieve.
 * @return Stored pointer or NULL on error.
 */
void* List_get(List* list, size_t index);

/**
 * @brief Retrieve the last element's value.
 *
 * Returns NULL and logs an error if the list is empty.
 *
 * @param list Pointer to List.
 * @return Stored pointer or NULL if the list is empty.
 */
void* List_getLast(List* list);

/**
 * @brief Retrieve the first element's value.
 *
 * Returns NULL and logs an error if the list is empty.
 *
 * @param list Pointer to List.
 * @return Stored pointer or NULL if the list is empty.
 */
void* List_getFirst(List* list);

/**
 * @brief Replace the value stored at the given index.
 *
 * Does not free the previously stored value.
 *
 * @param list Pointer to List.
 * @param index Index to update.
 * @param value New value pointer to store.
 */
void List_set(List* list, size_t index, void* value);

/**
 * @brief Create a string representation of the list contents.
 *
 * Uses a StringBuilder internally. The caller must free the returned string.
 *
 * @param list Pointer to List.
 * @param format printf-style format string used to format each value.
 * @param formatValueFunc Optional function to convert a stored value to a printable pointer.
 * @return Newly allocated C string describing the list or NULL on allocation failure.
 */
char* List_toString(List* list, const char* format, void* (*formatValueFunc)(void* value));

/**
 * @brief Swap values at two indices.
 *
 * Indices must be valid; otherwise an error is logged.
 *
 * @param list Pointer to List.
 * @param index1 First index.
 * @param index2 Second index.
 */
void List_swap(List* list, size_t index1, size_t index2);

/**
 * @brief Randomly shuffle the list in-place using an optional PRNG.
 *
 * If \p pr is NULL the standard rand() is used.
 *
 * @param list Pointer to List.
 * @param pr Optional PRNG instance (see PRNG typedef).
 */
void List_shuffle(List* list, PRNG* pr);

/**
 * @brief Return the maximum element according to \p compare_func.
 *
 * compare_func(a, b) should return a negative value if a < b, 0 if equal,
 * positive if a > b.
 *
 * @param list Pointer to List.
 * @param compare_func Comparison function between two stored values.
 * @return Pointer to the maximum value or NULL if list is empty.
 */
void* List_max(List* list, CompareFunc compare_func);

/**
 * @brief Return the minimum element according to \p compare_func.
 *
 * @param list Pointer to List.
 * @param compare_func Comparison function between two stored values.
 * @return Pointer to the minimum value or NULL if list is empty.
 */
void* List_min(List* list, CompareFunc compare_func);

/**
 * @brief Create a new iterator for forward traversal of the list.
 *
 * The iterator captures a snapshot of the list size; it does not prevent
 * concurrent modifications to the list.
 *
 * @param list Pointer to List.
 * @return Newly allocated ListIterator or NULL on failure.
 */
ListIterator* ListIterator_new(List* list);

/**
 * @brief Destroy a ListIterator.
 *
 * @param iterator Pointer to iterator to free. Safe to call with NULL.
 */
void ListIterator_destroy(ListIterator* iterator);

/**
 * @brief Check whether the iterator has more elements.
 *
 * @param iterator Pointer to ListIterator.
 * @return true if a next element is available.
 */
bool ListIterator_hasNext(ListIterator* iterator);

/**
 * @brief Return the next element from the iterator.
 *
 * Advances the iterator and returns the stored value. Returns NULL if no
 * element is available.
 *
 * @param iterator Pointer to ListIterator.
 * @return Next stored value or NULL.
 */
void* ListIterator_next(ListIterator* iterator);

/**
 * @brief Return the index of the last returned element by the iterator.
 *
 * @param iterator Pointer to ListIterator.
 * @return Zero-based index of the last returned element, or -1 if none.
 */
int ListIterator_index(ListIterator* iterator);

/**
 * @brief Convenience macro to push an integer value (stored as pointer).
 *
 * Note: This performs a cast through long; only suitable for small integers
 * that fit in a pointer on the target platform.
 */
#define List_push_int(list, value) List_push(list, (void*)(long)value)
