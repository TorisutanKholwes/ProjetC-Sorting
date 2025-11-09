/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct MainFrame
 * @brief Represents the main application UI frame and state.
 *
 * The MainFrame collects all UI elements (settings panel, graph containers, popups),
 * maintains graph instances and synchronization primitives and exposes runtime state
 * such as currently selected graph, sorting flags and animation parameters.
 */
struct MainFrame {
    /** List of top-level UI Element objects currently rendered. */
    List* elements;

    /** Back-reference to the global App context (renderer, window, input, resources). */
    App* app;

    /** Array of pointers to ColumnGraph instances displayed in the main frame. */
    ColumnGraph** graph;

    /** Graph visual style used to initialize / redraw bars. */
    ColumnGraphStyle graph_style;

    /** Optional popup container shown on hover to display a bar's value. */
    Container* popup;

    /** Optional container showing detailed graph info (modal). */
    Container* graph_info;

    /** Optional container used to set a specific random seed (modal). */
    Container* seed_container;

    /** Optional container used to choose sort types (modal). */
    Container* sort_type_container;

    /** Current seed used for graph randomization; -1 if none. */
    int seed;

    /** Transient Element used to render temporary messages (e.g. "Sort started"). */
    Element* temp_element;

    /** Timer used to automatically remove temporary messages after a delay. */
    Timer* timer;

    /** Number of ColumnGraph instances currently created. */
    int graph_count;

    /** Number of bars per graph. */
    int bar_count;

    /** Index of the currently selected graph (0-based). */
    int selected_graph_index;

    /** When true, actions target all graphs instead of a single selected one. */
    bool all_selected;

    /** Whether the settings panel is visible. */
    bool showSettings;

    /** Whether the settings panel slide animation is in progress. */
    bool box_animating;

    /** Progress of the settings panel animation in range [0..1]. */
    float box_anim_progress;

    /** Starting X position of the animated settings panel. */
    float box_start_x;

    /** Target X position of the animated settings panel. */
    float box_target_x;

    /** Width reserved for the settings panel (pixels). */
    float settings_width;

    /** True when the mouse is hovering the help icon in settings. */
    bool hovered_help;

    /** If true, automatic delay computation is used during visualized sorting. */
    bool automatic_delay;

    /** Manual delay (ms) used when automatic_delay is false. */
    int delay_ms;

    /** Array of booleans indicating whether each graph is currently sorting. */
    bool* graph_sorting;

    /** Mutex protecting UI state used by sorting threads when needed. */
    SDL_mutex* ui_mutex;

    /** Array of mutexes, one per graph, used to synchronize access to graph data. */
    SDL_mutex** graph_mutexes;

    /** Last known window width used to detect and revert invalid resizes during sort. */
    int last_width;

    /** Last known window height used to detect and revert invalid resizes during sort. */
    int last_height;
};

/**
 * @struct SortThreadArg
 * @brief Argument structure passed to a thread that performs sorting on a graph.
 *
 * @field self Pointer to the MainFrame instance owning the graphs.
 * @field graph_index Index of the graph the thread must sort.
 */
struct SortThreadArg {
    MainFrame* self;
    int graph_index;
};

/**
 * @brief Allocate and initialize a new MainFrame instance.
 *
 * The created MainFrame will allocate internal lists and graph structures,
 * initialize mutexes and prepare UI elements. Ownership of the returned
 * pointer belongs to the caller and must be freed with MainFrame_destroy.
 *
 * @param app Pointer to the App context.
 * @return Pointer to a newly created MainFrame or NULL on allocation failure.
 */
MainFrame* MainFrame_new(App* app);

/**
 * @brief Destroy a MainFrame and free all associated resources.
 *
 * This will unregister input handlers, destroy UI elements, graphs, timers
 * and any created mutexes. After calling, the pointer becomes invalid.
 *
 * @param self Pointer to the MainFrame to destroy (may be NULL).
 */
void MainFrame_destroy(MainFrame* self);

/**
 * @brief Render callback used by the application's frame system.
 *
 * Renders all graphs (with proper graph-specific mutex locking), the UI elements
 * list and any active popup container.
 *
 * @param renderer SDL_Renderer to draw with.
 * @param self MainFrame instance to render (must not be NULL).
 */
void MainFrame_render(SDL_Renderer* renderer, MainFrame* self);

/**
 * @brief Update callback used by the application's frame system.
 *
 * Advances animations, updates timers, updates each ColumnGraph (with locking)
 * and updates UI elements.
 *
 * @param self MainFrame instance to update (must not be NULL).
 */
void MainFrame_update(MainFrame* self);

/**
 * @brief Attach event handlers and focus state for the main frame.
 *
 * Registers input callbacks for keyboard and mouse events so the main frame
 * can react to user interactions.
 *
 * @param self MainFrame instance to focus (must not be NULL).
 */
void MainFrame_focus(MainFrame* self);

/**
 * @brief Remove event handlers and unfocus the main frame.
 *
 * Unregisters previously registered input handlers and releases focus for
 * contained elements.
 *
 * @param self MainFrame instance to unfocus (must not be NULL).
 */
void MainFrame_unfocus(MainFrame* self);

/**
 * @brief Create a Frame wrapper representing this MainFrame.
 *
 * The returned Frame holds function pointers to render, update, focus/unfocus
 * and destroy functions implemented by the MainFrame.
 *
 * @param self MainFrame instance to wrap.
 * @return A new Frame instance configured for the MainFrame.
 */
Frame* MainFrame_getFrame(MainFrame* self);
