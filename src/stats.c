/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */
#include "stats.h"

#include "logger.h"
#include "utils.h"

GraphStats* GraphStats_new() {
    GraphStats* stats = calloc(1, sizeof(GraphStats));
    if (!stats) {
        error("Failed to allocate memory for GraphStats");
        return NULL;
    }
    return stats;
}

void GraphStats_destroy(GraphStats* stats) {
    if (!stats) return;
    safe_free((void**)&stats);
}

void GraphStats_reset(GraphStats* stats) {
    if (!stats) return;

    stats->sort_time = 0;
    stats->comparisons = 0;
    stats->swaps = 0;
    stats->access_memory = 0;
}

void GraphStats_incrementComparisons(GraphStats* stats) {
    if (!stats) return;
    stats->comparisons++;
}

void GraphStats_incrementSwaps(GraphStats* stats) {
    if (!stats) return;
    stats->swaps++;
}

void GraphStats_incrementAccessMemory(GraphStats* stats, int amount) {
    if (!stats) return;
    stats->access_memory += amount;
}

Uint32 GraphStats_getSortTime(GraphStats* stats) {
    if (!stats) return 0;
    return stats->sort_time;
}

void GraphStats_setSortTime(GraphStats* stats, Uint32 time) {
    if (!stats) return;
    stats->sort_time = time;
}

int GraphStats_getComparisons(GraphStats* stats) {
    if (!stats) return 0;
    return stats->comparisons;
}

int GraphStats_getSwaps(GraphStats* stats) {
    if (!stats) return 0;
    return stats->swaps;
}

int GraphStats_getAccessMemory(GraphStats* stats) {
    if (!stats) return 0;
    return stats->access_memory;
}
