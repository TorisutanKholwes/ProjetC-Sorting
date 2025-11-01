/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

struct GraphStats {
    Uint32 sort_time;
    int comparisons;
    int swaps;
    int access_memory;
};

GraphStats* GraphStats_new();
void GraphStats_destroy(GraphStats* stats);
void GraphStats_reset(GraphStats* stats);
void GraphStats_incrementComparisons(GraphStats* stats);
void GraphStats_incrementSwaps(GraphStats* stats);
void GraphStats_incrementAccessMemory(GraphStats* stats, int amount);
Uint32 GraphStats_getSortTime(GraphStats* stats);
void GraphStats_setSortTime(GraphStats* stats, Uint32 time);
int GraphStats_getComparisons(GraphStats* stats);
int GraphStats_getSwaps(GraphStats* stats);
int GraphStats_getAccessMemory(GraphStats* stats);