/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "column_graph.h"

#include "app.h"
#include "color.h"
#include "container.h"
#include "element.h"
#include "geometry.h"
#include "input.h"
#include "layout.h"
#include "list.h"
#include "logger.h"
#include "resource_manager.h"
#include "sort.h"
#include "stats.h"
#include "style.h"
#include "text.h"
#include "timer.h"
#include "utils.h"

static void ColumnGraph_handleMouseMotion(Input* input, SDL_Event* evt, ColumnGraph* graph);
static void ColumnGraph_initGraphStatsContainer(ColumnGraph* graph);

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, App* app, void* parent, ColumnGraphType type, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover, int index) {
    ColumnGraph* graph = calloc(1, sizeof(ColumnGraph));
    if (!graph) {
        error("Failed to allocate memory for ColumnGraph");
        return NULL;
    }
    graph->size.width = width;
    graph->size.height = height;
    graph->position = position;
    graph->app = app;
    graph->type = type;
    graph->graph_index = index;
    graph->bars_count = 0;
    graph->parent = parent;
    graph->stats = GraphStats_new();
    graph->sort_timer = Timer_new();
    graph->sort_type = LIST_SORT_TYPE_BUBBLE;
    graph->bars = List_create();
    graph->container = FlexContainer_new(position->x, position->y, width, height);
    graph->onHover = onHover;
    graph->offHover = offHover;
    FlexContainer_setAlignItems(graph->container, NO_FLEX_ALIGN);

    Input_addEventHandler(app->input, SDL_MOUSEMOTION, (EventHandlerFunc) ColumnGraph_handleMouseMotion, graph);
    ColumnGraph_initGraphStatsContainer(graph);
    return graph;
}

static void ColumnGraph_initGraphStatsContainer(ColumnGraph* graph) {
    if (graph->stats_container) {
        Container_destroy(graph->stats_container);
    }
    graph->stats_container = Container_new(graph->position->x, graph->position->y, fminf(275.f, graph->size.width / 2.5f), fminf(150.f, graph->size.height / 2.f), false, Color_rgba(0, 0, 0, 150), graph);
    bool max_width = graph->stats_container->box->size.width == 250.f;
    UNUSED(max_width);
    bool max_height = graph->stats_container->box->size.height == 150.f;
    int y = graph->position->y + 10;
    Size text_size;
    if (max_height) {
        Text* title = Text_newf(graph->app->renderer,
            TextStyle_new(ResourceManager_getDefaultBoldFont(graph->app->manager, 16), 16, COLOR_WHITE, TTF_STYLE_NORMAL),
            Position_new(graph->position->x + 10, y),
            false,
            "Graph %d Stats (%s) :", graph->graph_index + 1, ListSortType_toString(graph->sort_type));
        Container_addChild(graph->stats_container, Element_fromText(title, NULL));
        text_size = Text_getSize(title);
        if (text_size.width > graph->stats_container->box->size.width - 20) {
            Text_setStringf(title, "Graph %d Stats :", graph->graph_index + 1);
        }
        y += text_size.height + 10;
    }
    int y_offset = max_height ? 10 : graph->stats_container->box->size.height < 100.f ? 0 : 5;
    int font_size = max_height ? 14 : graph->stats_container->box->size.height < 100.f ? 12 : 13;
    TextStyle* stat_text_style = TextStyle_new(ResourceManager_getDefaultFont(graph->app->manager, font_size), font_size, COLOR_WHITE, TTF_STYLE_NORMAL);
    Text* sort_time_text = Text_newf(graph->app->renderer,
        stat_text_style,
        Position_new(graph->position->x + 10, y),
        false,
        "Sort Time: %.2fs", (float) graph->stats->sort_time / 1000.f);
    text_size = Text_getSize(sort_time_text);
    y += text_size.height + y_offset;

    Text* comparisons_text = Text_newf(graph->app->renderer,
        TextStyle_deepCopy(stat_text_style),
        Position_new(graph->position->x + 10, y),
        false,
        "Comparisons: %d", graph->stats->comparisons);
    text_size = Text_getSize(comparisons_text);
    y += text_size.height + y_offset;

    Text* swaps_text = Text_newf(graph->app->renderer,
        TextStyle_deepCopy(stat_text_style),
        Position_new(graph->position->x + 10, y),
        false,
        "Swaps: %d", graph->stats->swaps);
    text_size = Text_getSize(swaps_text);
    y += text_size.height + y_offset;

    Text* access_memory_text = Text_newf(graph->app->renderer,
        TextStyle_deepCopy(stat_text_style),
        Position_new(graph->position->x + 10, y),
        false,
        "Memory Access: %d", graph->stats->access_memory);

    Container_addChild(graph->stats_container, Element_fromText(sort_time_text, NULL));
    Container_addChild(graph->stats_container, Element_fromText(comparisons_text, NULL));
    Container_addChild(graph->stats_container, Element_fromText(swaps_text, NULL));
    Container_addChild(graph->stats_container, Element_fromText(access_memory_text, NULL));
}

void ColumnGraph_destroy(ColumnGraph* graph) {
    if (!graph) return;
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        ColumnGraphBar_destroy(bar);
    }
    Input_removeOneEventHandler(graph->app->input, SDL_MOUSEMOTION, graph);
    ListIterator_destroy(it);
    List_destroy(graph->bars);
    FlexContainer_destroy(graph->container);
    safe_free((void**)&graph->position);
    safe_free((void**)&graph);
}

void ColumnGraph_update(ColumnGraph* graph) {
    if (!graph) return;
    if (graph->sort_in_progress) {
        GraphStats_setSortTime(graph->stats, Timer_getTicks(graph->sort_timer));
    }
    ColumnGraph_initGraphStatsContainer(graph);
}

void ColumnGraph_initBars(ColumnGraph* graph, const int bars_count, void** values, ColumnGraphStyle style) {
    if (!graph) return;
    graph->bars_count = bars_count;
    graph->graph_style = style;

    int numColors;
    Color** colors = ColumnGraph_getDefaultColors(style, &numColors);

    void* max;
    switch (graph->type) {
        case GRAPH_TYPE_INT:
            max = (void*) arrayMax(voidToLongArray(values, bars_count), bars_count);
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
    FlexContainer_layout(graph->container);
    for (int i = 0; i < numColors; i++) {
        Color_destroy(colors[i]);
    }
    safe_free((void**)&colors);
}

void ColumnGraph_initBarsColored(ColumnGraph* graph, int bars_count, void** values, Color** colors) {
    if (!graph) return;
    graph->bars_count = bars_count;

    void* max;
    switch (graph->type) {
        case GRAPH_TYPE_INT:
            max = (void*) arrayMax(voidToLongArray(values, bars_count), bars_count);
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
        ColumnGraphBar* graph_bar = ColumnGraphBar_new(values[i], colors[i], graph->size.height, (void*)max, graph);
        List_push(graph->bars, graph_bar);
        FlexContainer_addElement(graph->container, graph_bar->element, 1.f, 1.f, -1.f);
    }
    FlexContainer_layout(graph->container);
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
    ColumnGraph_initBars(graph, bars_count, longToVoidArray(values, bars_count), style);
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
    FlexContainer_layout(graph->container);
    ListIterator_destroy(it);
}

void ColumnGraph_resetContainer(ColumnGraph* graph) {
    if (!graph) return;
    FlexContainer_clear(graph->container);
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        if (bar) {
            FlexContainer_addElement(graph->container, bar->element, 1.f, 1.f, -1.f);
        }
    }
    FlexContainer_layout(graph->container);
    ListIterator_destroy(it);
}

void ColumnGraph_sortGraph(ColumnGraph* graph, SDL_mutex* gm, DelayFunc delay_func, MainFrame* main_frame) {
    if (!graph) return;
    GraphStats_reset(graph->stats);
    Timer_start(graph->sort_timer);
    List_sort(graph->bars, graph->sort_type, ColumnGraphBar_compare, gm, delay_func, main_frame, graph);
    Timer_stop(graph->sort_timer);
    FlexContainer_clear(graph->container);
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        FlexContainer_addElement(graph->container, bar->element, 1.f, 1.f, -1.f);
    }
    FlexContainer_layout(graph->container);
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

void ColumnGraph_renderBar(ColumnGraph* graph, int w, int h) {
    if (!graph) return;
    FlexContainer_clear(graph->container);
    FlexContainer_setSize(graph->container, w, h);
    graph->size.width = w;
    graph->size.height = h;
    graph->hovered = false;
    if (graph->hoveredBar) {
        graph->hoveredBar = NULL;
    }

    int val_count;
    void** values = ColumnGraph_getValues(graph, &val_count);
    if (!values) return;

    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar_destroy(ListIterator_next(it));
    }
    ListIterator_destroy(it);
    List_clear(graph->bars);
    ColumnGraph_initBars(graph, val_count, values, graph->graph_style);

    safe_free((void**)&values);
}

void ColumnGraph_render(ColumnGraph* graph, SDL_Renderer* renderer) {
    if (!graph || !renderer) return;
    ListIterator* it = ListIterator_new(graph->bars);
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        if (!bar) continue;
        Element_render(bar->element, renderer);
    }
    ListIterator_destroy(it);
    Container_render(graph->stats_container, renderer);
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

Color** ColumnGraph_getColors(ColumnGraph* graph, int* out_len) {
    if (!graph) {
        *out_len = 0;
        return NULL;
    };
    int count = List_size(graph->bars);
    Color** colors = calloc(count, sizeof(Color*));
    if (!colors) {
        error("Failed to allocate memory for ColumnGraph colors");
        *out_len = 0;
        return NULL;
    }
    ListIterator* it = ListIterator_new(graph->bars);
    int index = 0;
    while (ListIterator_hasNext(it)) {
        ColumnGraphBar* bar = (ColumnGraphBar*)ListIterator_next(it);
        colors[index++] = Color_copy(bar->color);
    }
    ListIterator_destroy(it);
    *out_len = count;

    return colors;
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

void ColumnGraph_setSortType(ColumnGraph* graph, ListSortType sort_type) {
    if (!graph) return;
    graph->sort_type = sort_type;
}

static float ColumnGraphBar_calculateBarHeight(void* value, float height, void* max_value, ColumnGraphType type) {
    switch (type) {
        case GRAPH_TYPE_INT:
            return ((int)(long)value) * height / ((int)(long)max_value);
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
    if (!graph || graph->sort_in_progress) return;
    UNUSED(evt);
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
    ColumnGraphBar* bar = (ColumnGraphBar*)a;
    ColumnGraphBar* other = (ColumnGraphBar*)b;
    if (bar->parent->type != other->parent->type) {
        return 0;
    }
    switch (bar->parent->type) {
        case GRAPH_TYPE_INT:
            return ((int)(long)bar->value) - ((int)(long)other->value);
        case GRAPH_TYPE_STRING:
            return String_compare((const char*)bar->value, (const char*)other->value);
        default:
            return 0;
    }
}

Color** ColumnGraph_getDefaultColors(ColumnGraphStyle style, int* out_count) {
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

char* ColumnGraph_getTypeName(ColumnGraphType type) {
    switch (type) {
        case GRAPH_TYPE_INT:
            return "Integer";
        case GRAPH_TYPE_STRING:
            return "String";
        default:
            return "Unknow";
    }
}