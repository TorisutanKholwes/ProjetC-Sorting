/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#pragma once

#include "Settings.h"

struct ColumnGraph {
    Size size;
    Position* position;
    int bars_count;
    List* bars;
    FlexContainer* container;
    void* parent;

    Input* input;
    bool hovered;

    ColumnGraphType type;

    ColumnGraphStyle graph_style;

    ColumnGraphBar* hoveredBar;
    ColumnsHoverFunc onHover;
    ColumnsHoverFunc offHover;
};

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, Input* input, void* parent, ColumnGraphType type, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover);
void ColumnGraph_destroy(ColumnGraph* graph);
void ColumnGraph_initBars(ColumnGraph* graph, int bars_count, void** values, ColumnGraphStyle style);
void ColumnGraph_initBarsIncrement(ColumnGraph* graph, int bars_count, ColumnGraphStyle style);
void ColumnGraph_setGraphType(ColumnGraph* graph, ColumnGraphType type);
void ColumnGraph_shuffleBars(ColumnGraph* graph);
void ColumnGraph_sortGraph(ColumnGraph* graph, ListSortType sort_type, DelayFunc delay_func, MainFrame* main_frame);
void ColumnGraph_removeHovering(ColumnGraph* graph);
void ColumnGraph_renderBar(ColumnGraph* graph, int w, int h);
void** ColumnGraph_getValues(ColumnGraph* graph, int* out_len);
void ColumnGraph_resetBars(ColumnGraph* graph);
void ColumnGraph_resetContainer(ColumnGraph* graph);

struct ColumnGraphBar {
    void* value;
    Element* element;
    Color* color;
    ColumnGraph* parent;
};

ColumnGraphBar* ColumnGraphBar_new(void* value, Color* color, float height, void* max_value, ColumnGraph* parent);
void ColumnGraphBar_destroy(ColumnGraphBar* bar);
void ColumnGraphBar_setValue(ColumnGraphBar* bar, void* value);
int ColumnGraphBar_compare(const void* a, const void* b);

Color** ColumnGraph_getColors(ColumnGraphStyle style, int* out_count);
char* ColumnGraph_getStyleName(ColumnGraphStyle style);
Color* ColumnGraph_getHoverColor(ColumnGraphStyle style);

char* ColumnGraph_getTypeName(ColumnGraphType type);