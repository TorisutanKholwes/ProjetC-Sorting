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

    ColumnGraphBar* hoveredBar;
    ColumnsHoverFunc onHover;
    ColumnsHoverFunc offHover;
};

ColumnGraph* ColumnGraph_new(float width, float height, Position* position, Input* input, void* parent, ColumnsHoverFunc onHover, ColumnsHoverFunc offHover);
void ColumnGraph_destroy(ColumnGraph* graph);
void ColumnGraph_initBars(ColumnGraph* graph, int bars_count);
void ColumnGraph_shuffleBars(ColumnGraph* graph);

struct ColumnGraphBar {
    float value;
    Element* element;
    Color* color;
};

ColumnGraphBar* ColumnGraphBar_new(float value, Color* color, float height, float max_value);
void ColumnGraphBar_destroy(ColumnGraphBar* bar);
void ColumnGraphBar_setValue(ColumnGraphBar* bar, float value);
