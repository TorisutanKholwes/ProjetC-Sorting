/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

// TODO use sort args struct to reduce parameters
/**
 * @struct SortArgs
 * @brief Convenience container grouping common sorting callback/lock parameters.
 *
 * This struct can be used to reduce the number of parameters passed to sort functions
 * when spawning threads or storing per-sort context.
 *
 * @field gm Mutex protecting concurrent access to the list being sorted.
 * @field delay_func Visualization callback invoked to produce an animation delay.
 * @field main_frame Pointer to the MainFrame used by the UI/delay callback.
 * @field column_graph Pointer to the ColumnGraph being visualized (may be NULL).
 */
struct SortArgs {
    SDL_mutex* gm;
    DelayFunc delay_func;
    MainFrame* main_frame;
    ColumnGraph* column_graph;
};

/**
 * @brief Default comparison function used when none is provided.
 *
 * Interprets the provided pointers as integer values and returns a negative,
 * zero or positive value following the same contract as standard C compare functions.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 * @return <0 if a < b, 0 if a == b, >0 if a > b.
 */
int List_defaultCompare(const void* a, const void* b);

/**
 * @brief Dispatch function selecting and running the requested sort algorithm.
 *
 * The implementation will call the specific algorithm identified by \p sortType.
 *
 * @param list Pointer to the List to sort (must not be NULL).
 * @param sortType Enum value selecting the sort algorithm.
 * @param compare_func Comparison callback or NULL to use List_defaultCompare.
 * @param gm Pointer to an SDL_mutex used to protect list modifications (may be NULL if not required).
 * @param delay_func Callback used to produce delays/visual updates during sorting (may be NULL).
 * @param mainframe Pointer to the MainFrame used by delay callbacks (may be NULL).
 * @param column_graph Pointer to the ColumnGraph associated with the list (may be NULL).
 */
void List_sort(List* list, ListSortType sortType, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Test whether a list is sorted according to the provided comparator.
 *
 * @param list Pointer to the List to test.
 * @param compare_func Comparison function to use or NULL to use List_defaultCompare.
 * @return true if the list is sorted or contains fewer than two elements.
 */
bool List_isSorted(List* list, CompareFunc compare_func);

/**
 * @brief Bubble sort implementation operating on a List.
 *
 * Thread-safe modifications must be guarded by the provided \p gm mutex. Visual
 * updates are emitted via \p delay_func when provided.
 */
void List_sortBubble(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Quick sort implementation for doubly-linked lists.
 *
 * The routine temporarily breaks circular links to perform recursive partitioning
 * and restores list invariants before returning.
 */
void List_sortQuick(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Merge sort implementation using an auxiliary array.
 *
 * Allocates a temporary buffer of size list->size; on allocation failure the
 * function logs an error and returns without modifying the list.
 */
void List_sortMerge(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Insertion sort implementation.
 */
void List_sortInsertion(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Bitonic sort implementation.
 *
 * Requires the list size to be a power of two; otherwise the function logs an error
 * and returns without sorting.
 */
void List_sortBitonic(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Bogo sort (random shuffles until sorted). Intended for demonstration only.
 */
void List_sortBogo(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Selection sort implementation.
 */
void List_sortSelection(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Radix sort implementation for integer graphs.
 *
 * Only supports lists whose associated ColumnGraph has type GRAPH_TYPE_INT.
 * Values are treated as non-negative integers stored as pointer-sized integers.
 */
void List_sortRadix(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);

/**
 * @brief Convert a ListSortType enum value to a human-readable string.
 *
 * @param sortType Enum value to convert.
 * @return Nul-terminated string describing the sort type.
 */
const char* ListSortType_toString(ListSortType sortType);

/**
 * @brief Parse a string and return the corresponding ListSortType enum value.
 *
 * Returns LIST_SORT_TYPE_COUNT if the string does not match any known sort name.
 *
 * @param str Nul-terminated C-string containing the sort name.
 * @return Corresponding ListSortType or LIST_SORT_TYPE_COUNT on unknown input.
 */
ListSortType ListSortType_fromString(const char* str);
