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

    ColumnGraphStyle graph_style;

    ColumnGraphBar* hoveredBar;
    ColumnsHoverFunc onHover;
    ColumnsHoverFunc offHover;
};

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, Input* input, void* parent, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover);
void ColumnGraph_destroy(ColumnGraph* graph);
void ColumnGraph_initBars(ColumnGraph* graph, int bars_count, const int* values, ColumnGraphStyle style);
void ColumnGraph_initBarsIncrement(ColumnGraph* graph, int bars_count, ColumnGraphStyle style);
void ColumnGraph_shuffleBars(ColumnGraph* graph);
void ColumnGraph_sortGraph(ColumnGraph* graph, ListSortType sort_type);
void ColumnGraph_removeHovering(ColumnGraph* graph);
int* ColumnGraph_getValues(ColumnGraph* graph, int* out_len);
void ColumnGraph_resetBars(ColumnGraph* graph);

struct ColumnGraphBar {
    int value;
    Element* element;
    Color* color;
};

ColumnGraphBar* ColumnGraphBar_new(int value, Color* color, float height, float max_value);
void ColumnGraphBar_destroy(ColumnGraphBar* bar);
void ColumnGraphBar_setValue(ColumnGraphBar* bar, float value);
int ColumnGraphBar_compare(const void* a, const void* b);

Color** ColumnGraph_getColors(ColumnGraphStyle style, int* out_count);
char* ColumnGraph_getStyleName(ColumnGraphStyle style);
Color* ColumnGraph_getHoverColor(ColumnGraphStyle style);