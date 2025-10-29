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

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, Input* input, void* parent, ColumnGraphType type, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover) {
    ColumnGraph* graph = calloc(1, sizeof(ColumnGraph));
    if (!graph) {
        error("Failed to allocate memory for ColumnGraph");
        return NULL;
    }
    graph->size.width = width;
    graph->size.height = height;
    graph->position = position;
    graph->input = input;
    graph->type = type;
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

void ColumnGraph_initBars(ColumnGraph* graph, const int bars_count, void** values, ColumnGraphStyle style) {
    if (!graph) return;
    graph->bars_count = bars_count;
    graph->graph_style = style;

    int numColors;
    Color** colors = ColumnGraph_getColors(style, &numColors);

    void* max;
    switch (graph->type) {
        case GRAPH_TYPE_INT:
            max = (void*) arrayMax((void**)values, bars_count);
            break;
        case GRAPH_TYPE_STRING:
            max = (void*) String_max((const char**)values, bars_count);
            break;
        default:
            max = NULL;
            break;
    }
    if (max == NULL) return;

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

        ColumnGraphBar* graph_bar = ColumnGraphBar_new(values[i], grad, graph->size.height, (void*)max, graph);
        List_push(graph->bars, graph_bar);
        FlexContainer_addElement(graph->container, graph_bar->element, 1.f, 1.f, -1.f);
    }
}

void ColumnGraph_initBarsIncrement(ColumnGraph* graph, int bars_count, ColumnGraphStyle style) {
    if (!graph || graph->type != GRAPH_TYPE_INT) return;
    long* values = calloc(bars_count, sizeof(long));
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


void ColumnGraph_setGraphType(ColumnGraph* graph, ColumnGraphType type) {
    if (!graph) return;
    graph->type = type;
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
        graph->offHover(graph->parent, 0, graph->type);
    }
}

void** ColumnGraph_getValues(ColumnGraph* graph, int* out_len) {
    if (!graph) {
        *out_len = 0;
        return NULL;
    }
    int count = List_size(graph->bars);
    void** values = calloc(count, sizeof(void*));
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

static float ColumnGraphBar_calculateBarHeight(void* value, float height, void* max_value, ColumnGraphType type) {
    switch (type) {
        case GRAPH_TYPE_INT:
            return ((int)value) * height / ((int)max_value);
        case GRAPH_TYPE_STRING:
            int len = strlen(value);
            int max_len = strlen(max_value);
            return len * height / max_len;
        default:
            return 0.f;
    }
}

ColumnGraphBar* ColumnGraphBar_new(void* value, Color* color, float height, void* max_value, ColumnGraph* parent) {
    ColumnGraphBar* bar = calloc(1, sizeof(ColumnGraphBar));
    if (!bar) {
        error("Failed to allocate memory for ColumnGraphBar");
        return NULL;
    }
    bar->parent = parent;
    bar->color = Color_copy(color);
    bar->value = value;
    float barHeight = ColumnGraphBar_calculateBarHeight(value, height, max_value, parent->type);
    Box* box = Box_new(0, barHeight, 0, Position_new(0, height - barHeight), Color_copy(color), NULL, false);
    bar->element = Element_fromBox(box, NULL);
    return bar;
}

void ColumnGraphBar_destroy(ColumnGraphBar* bar) {
    if (!bar) return;
    Color_destroy(bar->color);
    safe_free((void**)&bar);
}

void ColumnGraphBar_setValue(ColumnGraphBar* bar, void* value) {
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
                    graph->offHover(graph->parent, 0, graph->type);
                }
            }
            graph->hovered = true;
            bar->element->data.box->background = Color_copy(ColumnGraph_getHoverColor(graph->graph_style));
            graph->hoveredBar = bar;
            if (graph->onHover) {
                if (!graph->onHover(graph->parent, bar->value, graph->type)) {
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
            graph->offHover(graph->parent, 0, graph->type);
        }
    }
    ListIterator_destroy(it);
}

int ColumnGraphBar_compare(const void* a, const void* b) {
    ColumnGraphBar* bar = a;
    ColumnGraphBar* other = b;
    if (bar->parent->type != other->parent->type) {
        return 0;
    }
    switch (bar->parent->type) {
        case GRAPH_TYPE_INT:
            return ((int)bar->value) - ((int)other->value);
        case GRAPH_TYPE_STRING:
            return String_compare((const char*)bar->value, (const char*)other->value);
        default:
            return 0;
    }
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