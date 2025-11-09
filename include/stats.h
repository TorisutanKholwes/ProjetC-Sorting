/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct GraphStats
 * @brief Aggregated statistics produced by sorting visualizations.
 *
 * Fields are stored as primitive types and updated by the sorting routines.
 * The structure does not own any dynamic memory.
 */
struct GraphStats {
    Uint32 sort_time;   /**< Total sort time in milliseconds (or platform-defined units). */
    int comparisons;    /**< Number of element comparisons performed. */
    int swaps;          /**< Number of element swaps performed. */
    int access_memory;  /**< Count of memory access operations recorded. */
};

/**
 * @brief Allocate and initialize a new GraphStats instance.
 *
 * The returned object is zero-initialized and must be freed with GraphStats_destroy.
 *
 * @return Pointer to a newly allocated GraphStats, or NULL on allocation failure.
 */
GraphStats* GraphStats_new();

/**
 * @brief Free a GraphStats instance.
 *
 * Safe to call with a NULL pointer. Does not free any external resources because
 * GraphStats contains only primitive fields.
 *
 * @param stats Pointer to the GraphStats to destroy.
 */
void GraphStats_destroy(GraphStats* stats);

/**
 * @brief Reset all statistics fields to zero.
 *
 * @param stats Pointer to the GraphStats to reset.
 */
void GraphStats_reset(GraphStats* stats);

/**
 * @brief Increment the comparisons counter by one.
 *
 * @param stats Pointer to the GraphStats to update.
 */
void GraphStats_incrementComparisons(GraphStats* stats);

/**
 * @brief Increment the swaps counter by one.
 *
 * @param stats Pointer to the GraphStats to update.
 */
void GraphStats_incrementSwaps(GraphStats* stats);

/**
 * @brief Add a given amount to the memory access counter.
 *
 * @param stats Pointer to the GraphStats to update.
 * @param amount Number of memory accesses to add (may be negative to subtract).
 */
void GraphStats_incrementAccessMemory(GraphStats* stats, int amount);

/**
 * @brief Get the recorded sort time.
 *
 * @param stats Pointer to the GraphStats to query.
 * @return Recorded sort time, or 0 if stats is NULL.
 */
Uint32 GraphStats_getSortTime(GraphStats* stats);

/**
 * @brief Set the recorded sort time.
 *
 * @param stats Pointer to the GraphStats to update.
 * @param time Time value to store.
 */
void GraphStats_setSortTime(GraphStats* stats, Uint32 time);

/**
 * @brief Get the number of comparisons recorded.
 *
 * @param stats Pointer to the GraphStats to query.
 * @return Number of comparisons, or 0 if stats is NULL.
 */
int GraphStats_getComparisons(GraphStats* stats);

/**
 * @brief Get the number of swaps recorded.
 *
 * @param stats Pointer to the GraphStats to query.
 * @return Number of swaps, or 0 if stats is NULL.
 */
int GraphStats_getSwaps(GraphStats* stats);

/**
 * @brief Get the total memory access count recorded.
 *
 * @param stats Pointer to the GraphStats to query.
 * @return Memory access count, or 0 if stats is NULL.
 */
int GraphStats_getAccessMemory(GraphStats* stats);
