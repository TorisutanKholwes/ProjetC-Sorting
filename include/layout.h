/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

enum FlexDirection {
    FLEX_DIRECTION_ROW,
    FLEX_DIRECTION_COLUMN,
    FLEX_DIRECTION_ROW_REVERSE,
    FLEX_DIRECTION_COLUMN_REVERSE
};


enum FlexJustify {
    FLEX_JUSTIFY_START,
    FLEX_JUSTIFY_END,
    FLEX_JUSTIFY_CENTER,
    FLEX_JUSTIFY_SPACE_BETWEEN,
    FLEX_JUSTIFY_SPACE_AROUND,
    FLEX_JUSTIFY_SPACE_EVENLY
};


enum FlexAlign {
    FLEX_ALIGN_START,
    FLEX_ALIGN_END,
    FLEX_ALIGN_CENTER,
    FLEX_ALIGN_STRETCH,
    NO_FLEX_ALIGN
};

struct FlexItem {
    Element* element;
    float flex_grow;
    float flex_shrink;
    float flex_basis;
    float width;
    float height;
};

struct FlexContainer {
    List* items;
    FlexDirection direction;
    FlexJustify justify_content;
    FlexAlign align_items;
    float gap;
    float x;
    float y;
    float width;
    float height;
};

FlexContainer* FlexContainer_new(float x, float y, float width, float height);
void FlexContainer_destroy(FlexContainer* container);
void FlexContainer_setDirection(FlexContainer* container, FlexDirection direction);
void FlexContainer_setJustifyContent(FlexContainer* container, FlexJustify justify);
void FlexContainer_setAlignItems(FlexContainer* container, FlexAlign align);
void FlexContainer_setGap(FlexContainer* container, float gap);
void FlexContainer_addElement(FlexContainer* container, Element* element, float flex_grow, float flex_shrink, float flex_basis);
void FlexContainer_layout(FlexContainer* container);
void FlexContainer_clear(FlexContainer* container);
void FlexContainer_setSize(FlexContainer* container, float width, float height);