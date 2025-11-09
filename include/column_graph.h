/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#pragma once

#include "Settings.h"

/**
 * @struct ColumnGraph
 * @brief Represents a column/bar graph UI component.
 *
 * Stores layout, rendering and interaction state for a single graph:
 * size and position, list of bars, container used for layout, input/app
 * references, sorting and statistics state, style and callbacks.
 */
struct ColumnGraph {
    /** @brief Width and height of the graph area. */
    Size size;

    /** @brief Top-left position of the graph in screen coordinates. */
    Position* position;

    /** @brief Number of bars currently present in the graph. */
    int bars_count;

    /** @brief List of ColumnGraphBar\* contained by the graph. */
    List* bars;

    /** @brief Flex container used to layout bar elements. */
    FlexContainer* container;

    /** @brief Optional user-defined parent pointer (owner context). */
    void* parent;

    /** @brief Index of this graph when used in multi-graph layouts. */
    int graph_index;

    /** @brief Pseudo-random number generator used for shuffling, optional. */
    PRNG* prng;

    /** @brief Reference to the application instance (renderer, input, resources). */
    App* app;

    /** @brief Whether the mouse is currently hovering a bar. */
    bool hovered;

    /** @brief Whether the graph is paused (affects sorting/animation). */
    bool paused;

    /** @brief Whether a sort operation is currently in progress. */
    bool sort_in_progress;

    /** @brief Timer used to measure sort time. */
    Timer* sort_timer;

    /** @brief Statistics collected while sorting (comparisons, swaps, etc.). */
    GraphStats* stats;

    /** @brief Container used to render the graph statistics UI. */
    Container* stats_container;

    /** @brief Current list sort algorithm used for sorting the bars. */
    ListSortType sort_type;

    /** @brief Data type of the graph values (integer or string). */
    ColumnGraphType type;

    /** @brief Visual style used to color bars and hover states. */
    ColumnGraphStyle graph_style;

    /** @brief Pointer to the currently hovered bar, if any. */
    ColumnGraphBar* hoveredBar;

    /**
     * @brief Callback invoked when the user hovers a column.
     *
     * Signature: ColumnsHoverFunc(void* parent, void* value, ColumnGraphType type)
     */
    ColumnsHoverFunc onHover;

    /**
     * @brief Callback invoked when the hover is removed from a column.
     *
     * Signature: ColumnsHoverFunc(void* parent, void* value, ColumnGraphType type)
     */
    ColumnsHoverFunc offHover;
};

/**
 * @brief Create a new ColumnGraph instance.
 *
 * @param width Graph width in pixels.
 * @param height Graph height in pixels.
 * @param position Pointer to initial position (owned by caller or graph as noted in implementation).
 * @param app Pointer to the App instance (renderer, input manager, resources).
 * @param parent Optional user-defined parent pointer.
 * @param type ColumnGraphType indicating value type (e.g. GRAPH_TYPE_INT).
 * @param onHover Callback invoked when hovering a bar.
 * @param offHover Callback invoked when hover ends.
 * @param index Graph index used for labeling or identification.
 * @param seed PRNG seed; if negative, PRNG may be disabled.
 * @return Pointer to newly allocated ColumnGraph, or NULL on failure.
 */
ColumnGraph* ColumnGraph_new(float width, float height, Position* position, App* app, void* parent, ColumnGraphType type, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover, int index, int seed);

/**
 * @brief Destroy a ColumnGraph and free all owned resources.
 *
 * Unregisters input handlers, destroys bars and containers and frees memory.
 *
 * @param graph Pointer to the ColumnGraph to destroy. Safe to pass NULL.
 */
void ColumnGraph_destroy(ColumnGraph* graph);

/**
 * @brief Update per-frame graph state.
 *
 * Typically updates statistics, timers and any dynamic UI elements.
 *
 * @param graph Pointer to the ColumnGraph to update.
 */
void ColumnGraph_update(ColumnGraph* graph);

/**
 * @brief Initialize graph bars from raw values and a style.
 *
 * Allocates and configures bars based on provided values array.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param bars_count Number of elements in \p values.
 * @param values Array of void\* values (type depends on graph->type).
 * @param style Visual style for bar coloring.
 */
void ColumnGraph_initBars(ColumnGraph* graph, int bars_count, void** values, ColumnGraphStyle style);

/**
 * @brief Initialize graph bars using provided color array.
 *
 * Each bar receives the corresponding color from the \p colors array.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param bars_count Number of bars / colors.
 * @param values Array of void\* values.
 * @param colors Array of Color\* of length \p bars_count (ownership as per implementation).
 */
void ColumnGraph_initBarsColored(ColumnGraph* graph, int bars_count, void** values, Color** colors);

/**
 * @brief Initialize graph bars with incremental integer values (1..n).
 *
 * Only valid when graph->type == GRAPH_TYPE_INT.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param bars_count Number of bars to create.
 * @param style Visual style for bar coloring.
 */
void ColumnGraph_initBarsIncrement(ColumnGraph* graph, int bars_count, ColumnGraphStyle style);

/**
 * @brief Set the value type of the graph (integer or string).
 *
 * @param graph Pointer to the ColumnGraph.
 * @param type ColumnGraphType to set.
 */
void ColumnGraph_setGraphType(ColumnGraph* graph, ColumnGraphType type);

/**
 * @brief Shuffle the order of bars randomly.
 *
 * Uses the graph's PRNG if available, updates container layout afterwards.
 *
 * @param graph Pointer to the ColumnGraph.
 */
void ColumnGraph_shuffleBars(ColumnGraph* graph);

/**
 * @brief Sort the graph's bars using the configured sort type.
 *
 * This function updates graph statistics and timing. Sorting may use a mutex
 * and a delay callback for animated sorting operations.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param gm Optional SDL_mutex used by the sort implementation.
 * @param delay_func DelayFunc used to introduce animation delays.
 * @param main_frame Pointer to MainFrame for UI updates during sorting.
 */
void ColumnGraph_sortGraph(ColumnGraph* graph, SDL_mutex* gm, DelayFunc delay_func, MainFrame* main_frame);

/**
 * @brief Remove current hovering state and restore bar visual to normal.
 *
 * Calls the offHover callback if set.
 *
 * @param graph Pointer to the ColumnGraph.
 */
void ColumnGraph_removeHovering(ColumnGraph* graph);

/**
 * @brief Recreate/render bars to fit a new width / height.
 *
 * Re-reads current values and rebuilds the bar elements sized to \p w x \p h.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param w New width in pixels.
 * @param h New height in pixels.
 */
void ColumnGraph_renderBar(ColumnGraph* graph, int w, int h);

/**
 * @brief Render the graph bars and statistics container to the provided renderer.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param renderer SDL_Renderer used for drawing. Must be non-NULL.
 */
void ColumnGraph_render(ColumnGraph* graph, SDL_Renderer* renderer);

/**
 * @brief Return a newly allocated array of values currently represented by bars.
 *
 * Caller is responsible for freeing the returned array with free().
 *
 * @param graph Pointer to the ColumnGraph.
 * @param out_len Pointer to int where the number of returned values will be stored.
 * @return Newly allocated void\*\* array of length *out_len, or NULL on failure.
 */
void** ColumnGraph_getValues(ColumnGraph* graph, int* out_len);

/**
 * @brief Return a newly allocated array of Color\* representing each bar's color.
 *
 * Caller must free the returned array and may need to free individual Color\* per ownership rules.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param out_len Pointer to int where the number of returned colors will be stored.
 * @return Newly allocated Color\*\* array of length *out_len, or NULL on failure.
 */
Color** ColumnGraph_getColors(ColumnGraph* graph, int* out_len);

/**
 * @brief Set the sorting algorithm used for graph sorts.
 *
 * @param graph Pointer to the ColumnGraph.
 * @param sort_type ListSortType to apply.
 */
void ColumnGraph_setSortType(ColumnGraph* graph, ListSortType sort_type);

/**
 * @brief Remove and destroy all bars from the graph.
 *
 * Leaves the graph in an empty state; does not free the graph itself.
 *
 * @param graph Pointer to the ColumnGraph.
 */
void ColumnGraph_resetBars(ColumnGraph* graph);

/**
 * @brief Reset the internal flex container by re-adding current bars.
 *
 * Useful after operations that change layout or ordering.
 *
 * @param graph Pointer to the ColumnGraph.
 */
void ColumnGraph_resetContainer(ColumnGraph* graph);

/**
 * @struct ColumnGraphBar
 * @brief Represents a single bar inside a ColumnGraph.
 *
 * Stores the value, the UI element used for rendering and the bar color.
 */
struct ColumnGraphBar {
    /** @brief Raw value associated with this bar (type depends on parent graph). */
    void* value;

    /** @brief Element used to render the bar (box/visual). */
    Element* element;

    /** @brief Color used to draw the bar background. */
    Color* color;

    /** @brief Pointer to parent ColumnGraph that owns this bar. */
    ColumnGraph* parent;
};

/**
 * @brief Create a new ColumnGraphBar instance.
 *
 * @param value Raw value for the bar.
 * @param color Color used to render the bar.
 * @param height Maximum graph height used to compute bar height.
 * @param max_value Maximum value in the dataset (for normalization).
 * @param parent Parent ColumnGraph pointer.
 * @return Pointer to a newly allocated ColumnGraphBar, or NULL on allocation failure.
 */
ColumnGraphBar* ColumnGraphBar_new(void* value, Color* color, float height, void* max_value, ColumnGraph* parent);

/**
 * @brief Destroy a ColumnGraphBar and free its resources.
 *
 * @param bar Pointer to the ColumnGraphBar to destroy. Safe to pass NULL.
 */
void ColumnGraphBar_destroy(ColumnGraphBar* bar);

/**
 * @brief Update the stored value for a bar.
 *
 * @param bar Pointer to the ColumnGraphBar.
 * @param value New value to set.
 */
void ColumnGraphBar_setValue(ColumnGraphBar* bar, void* value);

/**
 * @brief Compare two ColumnGraphBar instances for sorting.
 *
 * Comparison behavior depends on the parent graph's type (integer or string).
 *
 * @param a Pointer to first ColumnGraphBar.
 * @param b Pointer to second ColumnGraphBar.
 * @return Negative if a < b, zero if equal, positive if a > b.
 */
int ColumnGraphBar_compare(const void* a, const void* b);

/**
 * @brief Return an array of default colors for the given graph style.
 *
 * The returned array is newly allocated and must be freed by the caller.
 *
 * @param style ColumnGraphStyle for which to obtain default colors.
 * @param out_count Pointer to int where the number of returned colors is stored.
 * @return Newly allocated Color\*\* array or NULL on allocation failure.
 */
Color** ColumnGraph_getDefaultColors(ColumnGraphStyle style, int* out_count);

/**
 * @brief Return a human-readable name for the given graph style.
 *
 * @param style ColumnGraphStyle to describe.
 * @return Pointer to a string literal describing the style.
 */
char* ColumnGraph_getStyleName(ColumnGraphStyle style);

/**
 * @brief Return the hover color associated with the given style.
 *
 * @param style ColumnGraphStyle to query.
 * @return Color\* used to highlight hovered bars (ownership per implementation).
 */
Color* ColumnGraph_getHoverColor(ColumnGraphStyle style);

/**
 * @brief Return a human-readable name for the given graph type.
 *
 * @param type ColumnGraphType to describe.
 * @return Pointer to a string literal describing the type.
 */
char* ColumnGraph_getTypeName(ColumnGraphType type);
