/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "color.h"
#include "column_graphic.h"
#include "element.h"
#include "geometry.h"
#include "input.h"
#include "layout.h"
#include "list.h"
#include "logger.h"
#include "utils.h"

static void ColumnGraph_handleMouseMotion(Input* input, SDL_Event* evt, ColumnGraph* graph);

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, Input* input, void* parent, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover) {
    ColumnGraph* graph = calloc(1, sizeof(ColumnGraph));
    if (!graph) {
        error("Failed to allocate memory for ColumnGraph");
        return NULL;
    }
    graph->size.width = width;
    graph->size.height = height;
    graph->position = position;
    graph->input = input;
    graph->bars_count = 0;
    graph->parent = parent;
    graph->bars = List_create();
    graph->container = FlexContainer_new(position->x, position->y, width, height);
    graph->onHover = onHover;
    graph->offHover = offHover;
    FlexContainer_setAlignItems(graph->container, NO_FLEX_ALIGN);

    Input_addEventHandler(input, SDL_EVENT_MOUSE_MOTION, (EventHandlerFunc) ColumnGraph_handleMouseMotion, graph);
    return graph;
}

void ColumnGraph_destroy(ColumnGraph* graph) {
    if (!graph) return;
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        ColumnGraphBar_destroy(bar);
    }
    Input_removeOneEventHandler(graph->input, SDL_EVENT_MOUSE_MOTION, graph);
    ListIterator_destroy(it);
    List_destroy(graph->bars);
    FlexContainer_destroy(graph->container);
    safe_free((void**)&graph->position);
    safe_free((void**)&graph);
}

void ColumnGraph_initBars(ColumnGraph* graph, int bars_count) {
    if (!graph) return;
    graph->bars_count = bars_count;

    Color* rainbow[] = {
        COLOR_RED,
        COLOR_ORANGE,
        COLOR_YELLOW,
        COLOR_GREEN,
        COLOR_BLUE,
        COLOR_INDIGO,
        COLOR_MAGENTA
    };
    int numColors = sizeof(rainbow) / sizeof(rainbow[0]);

    for (int i = 0; i < bars_count; i++) {
        float t = (bars_count <= 1) ? 0.0f : (float)i / (float)(bars_count - 1);
        float pos = t * (numColors - 1);
        int idx = (int)floorf(pos);
        if (idx < 0) idx = 0;
        Color* grad;
        if (idx >= numColors - 1) {
            grad = Color_copy(rainbow[numColors - 1]);
        } else {
            float mix = pos - (float)idx;
            grad = interpolateColor(rainbow[idx], rainbow[idx + 1], mix);
        }

        ColumnGraphBar* graph_bar = ColumnGraphBar_new((float)(i + 1), grad, graph->size.height, bars_count);
        List_push(graph->bars, graph_bar);
        FlexContainer_addElement(graph->container, graph_bar->element, 1.f, 1.f, -1.f);
    }
}

void ColumnGraph_shuffleBars(ColumnGraph* graph) {
    if (!graph) return;
    List_shuffle(graph->bars);
    FlexContainer_clear(graph->container);
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        FlexContainer_addElement(graph->container, bar->element, 1.f, 1.f, -1.f);
    }
    ListIterator_destroy(it);
}

void ColumnGraph_sortGraph(ColumnGraph* graph, ListSortType sort_type) {
    if (!graph) return;
    List_sort(graph->bars, sort_type);
    FlexContainer_clear(graph->container);
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        FlexContainer_addElement(graph->container, bar->element, 1.f, 1.f, -1.f);
    }
    ListIterator_destroy(it);
}

void ColumnGraph_removeHovering(ColumnGraph* graph) {
    if (!graph || !graph->hoveredBar) return;
    graph->hoveredBar->element->data.box->background = Color_copy(graph->hoveredBar->color);
    graph->hovered = false;
    graph->hoveredBar = NULL;
    if (graph->offHover) {
        graph->offHover(graph->parent, 0);
    }
}

ColumnGraphBar* ColumnGraphBar_new(float value, Color* color, float height, float max_value) {
    ColumnGraphBar* bar = calloc(1, sizeof(ColumnGraphBar));
    if (!bar) {
        error("Failed to allocate memory for ColumnGraphBar");
        return NULL;
    }
    bar->color = Color_copy(color);
    bar->value = value;
    Box* box = Box_new(0, value * height / max_value, 0, Position_new(0, height - (value * height / max_value)), Color_copy(color), NULL, false);
    bar->element = Element_fromBox(box, NULL);
    return bar;
}

void ColumnGraphBar_destroy(ColumnGraphBar* bar) {
    if (!bar) return;
    Color_destroy(bar->color);
    safe_free((void**)&bar);
}

void ColumnGraphBar_setValue(ColumnGraphBar* bar, float value) {
    if (!bar) return;
    bar->value = value;
}

static void ColumnGraph_handleMouseMotion(Input* input, SDL_Event* evt, ColumnGraph* graph) {
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        Box* box = bar->element->data.box;
        SDL_FRect rect = { box->position->x, box->position->y, box->size.width, box->size.height };
        if (Input_mouseInRect(input, rect)) {
            if (graph->hovered) {
                graph->hovered = false;
                graph->hoveredBar->element->data.box->background = Color_copy(graph->hoveredBar->color);
                graph->hoveredBar = NULL;
                if (graph->offHover) {
                    graph->offHover(graph->parent, 0);
                }
            }
            graph->hovered = true;
            bar->element->data.box->background = COLOR_WHITE;
            graph->hoveredBar = bar;
            if (graph->onHover) {
                if (!graph->onHover(graph->parent, bar->value)) {
                    bar->element->data.box->background = Color_copy(bar->color);
                    graph->hovered = false;
                    graph->hoveredBar = NULL;
                }
            }
            ListIterator_destroy(it);
            return;
        }
    }
    if (graph->hovered) {
        graph->hovered = false;
        graph->hoveredBar->element->data.box->background = Color_copy(graph->hoveredBar->color);
        graph->hoveredBar = NULL;
        if (graph->offHover) {
            graph->offHover(graph->parent, 0);
        }
    }
    ListIterator_destroy(it);
}