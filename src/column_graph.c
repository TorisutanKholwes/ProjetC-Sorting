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

void ColumnGraph_initBars(ColumnGraph* graph, const int bars_count, const int* values, ColumnGraphStyle style) {
    if (!graph) return;
    graph->bars_count = bars_count;
    graph->graph_style = style;
/*
    Color* rainbow[] = {
        COLOR_RED,
        COLOR_ORANGE,
        COLOR_YELLOW,
        COLOR_GREEN,
        COLOR_BLUE,
        COLOR_INDIGO,
        COLOR_MAGENTA
    };
    int numColors = sizeof(rainbow) / sizeof(rainbow[0]);*/

    int numColors;
    Color** colors = ColumnGraph_getColors(style, &numColors);

    int max = arrayMax(values, bars_count);

    for (int i = 0; i < bars_count; i++) {
        float t = (bars_count <= 1) ? 0.0f : (float)i / (float)(bars_count - 1);
        float pos = t * (numColors - 1);
        int idx = (int)floorf(pos);
        if (idx < 0) idx = 0;
        Color* grad;
        if (idx >= numColors - 1) {
            grad = Color_copy(colors[numColors - 1]);
        } else {
            float mix = pos - (float)idx;
            grad = interpolateColor(colors[idx], colors[idx + 1], mix);
        }

        ColumnGraphBar* graph_bar = ColumnGraphBar_new(values[i], grad, graph->size.height, max);
        List_push(graph->bars, graph_bar);
        FlexContainer_addElement(graph->container, graph_bar->element, 1.f, 1.f, -1.f);
    }
}

void ColumnGraph_initBarsIncrement(ColumnGraph* graph, int bars_count, ColumnGraphStyle style) {
    int* values = calloc(bars_count, sizeof(int));
    if (!values) {
        error("Failed to allocate memory for ColumnGraphBarsIncrement");
        return;
    }
    for (int i = 0; i < bars_count; i++) {
        values[i] = i + 1;
    }
    ColumnGraph_initBars(graph, bars_count, values, style);
    safe_free((void**)&values);
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
    List_sort(graph->bars, sort_type, ColumnGraphBar_compare);
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

int* ColumnGraph_getValues(ColumnGraph* graph, int* out_len) {
    if (!graph) {
        *out_len = 0;
        return NULL;
    }
    int count = List_size(graph->bars);
    int* values = calloc(count, sizeof(int));
    if (!values) {
        error("Failed to allocate memory for ColumnGraph values");
        *out_len = 0;
        return NULL;
    }
    ListIterator* it = ListIterator_new(graph->bars);
    int index = 0;
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        values[index++] = bar->value;
    }
    ListIterator_destroy(it);
    *out_len = count;
    return values;
}

void ColumnGraph_resetBars(ColumnGraph* graph) {
    if (!graph) return;
    FlexContainer_clear(graph->container);
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar_destroy(ListIterator_next(it));
    }
    ListIterator_destroy(it);
    List_clear(graph->bars);
}

ColumnGraphBar* ColumnGraphBar_new(int value, Color* color, float height, float max_value) {
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
            bar->element->data.box->background = Color_copy(ColumnGraph_getHoverColor(graph->graph_style));
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

int ColumnGraphBar_compare(const void* a, const void* b) {
    return ((ColumnGraphBar*)a)->value - ((ColumnGraphBar*)b)->value;
}

Color** ColumnGraph_getColors(ColumnGraphStyle style, int* out_count) {
    Color* colors[7];
    switch (style) {
        case GRAPH_RAINBOW: {
            colors[0] = COLOR_RED;
            colors[1] = COLOR_ORANGE;
            colors[2] = COLOR_YELLOW;
            colors[3] = COLOR_GREEN;
            colors[4] = COLOR_BLUE;
            colors[5] = COLOR_INDIGO;
            colors[6] = COLOR_MAGENTA;
            *out_count = 7;
            break;
        }
        case GRAPH_WHITE: {
            colors[0] = COLOR_WHITE;
            *out_count = 1;
            break;
        }
        case GRAPH_BLACK: {
            colors[0] = COLOR_BLACK;
            *out_count = 1;
            break;
        }
        case GRAPH_GRAY: {
            colors[0] = COLOR_GRAY(127);
            *out_count = 1;
            break;
        }
        case GRAPH_BLACK_AND_WHITE: {
            colors[0] = COLOR_BLACK;
            colors[1] = COLOR_WHITE;
            *out_count = 2;
            break;
        }
        default: {
            colors[0] = COLOR_WHITE;
            *out_count = 1;
            break;
        }
    }
    Color** result = calloc(*out_count, sizeof(Color*));
    if (!result) {
        error("Failed to allocate memory for ColumnGraph colors");
        *out_count = 0;
        return NULL;
    }
    for (int i = 0; i < *out_count; i++) {
        result[i] = Color_copy(colors[i]);
    }
    return result;
}

char* ColumnGraph_getStyleName(ColumnGraphStyle style) {
    switch (style) {
        case GRAPH_RAINBOW:
            return "Rainbow";
        case GRAPH_WHITE:
            return "White";
        case GRAPH_BLACK:
            return "Black";
        case GRAPH_GRAY:
            return "Gray";
        case GRAPH_BLACK_AND_WHITE:
            return "Black and White";
        default:
            return "Unknown";
    }
}

Color* ColumnGraph_getHoverColor(ColumnGraphStyle style) {
    switch (style) {
        case GRAPH_RAINBOW:
            return COLOR_WHITE;
        case GRAPH_WHITE:
            return COLOR_BLACK;
        case GRAPH_BLACK:
            return COLOR_WHITE;
        case GRAPH_GRAY:
            return COLOR_RED;;
        case GRAPH_BLACK_AND_WHITE:
            return COLOR_BLUE;
        default:
            return COLOR_WHITE;
    }
}