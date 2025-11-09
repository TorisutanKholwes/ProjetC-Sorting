/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"


/**
 * @enum FlexDirection
 * @brief Main axis and flow direction for a FlexContainer.
 *
 * - FLEX_DIRECTION_ROW: left-to-right main axis.
 * - FLEX_DIRECTION_COLUMN: top-to-bottom main axis.
 * - FLEX_DIRECTION_ROW_REVERSE: right-to-left main axis.
 * - FLEX_DIRECTION_COLUMN_REVERSE: bottom-to-top main axis.
 */
enum FlexDirection {
    FLEX_DIRECTION_ROW,
    FLEX_DIRECTION_COLUMN,
    FLEX_DIRECTION_ROW_REVERSE,
    FLEX_DIRECTION_COLUMN_REVERSE
};

/**
 * @enum FlexJustify
 * @brief Distribution of free space along the main axis.
 *
 * Controls starting offset and spacing between items.
 */
enum FlexJustify {
    FLEX_JUSTIFY_START,
    FLEX_JUSTIFY_END,
    FLEX_JUSTIFY_CENTER,
    FLEX_JUSTIFY_SPACE_BETWEEN,
    FLEX_JUSTIFY_SPACE_AROUND,
    FLEX_JUSTIFY_SPACE_EVENLY
};

/**
 * @enum FlexAlign
 * @brief Alignment of items on the cross axis.
 *
 * - FLEX_ALIGN_START: align to cross start.
 * - FLEX_ALIGN_END: align to cross end.
 * - FLEX_ALIGN_CENTER: center on cross axis.
 * - FLEX_ALIGN_STRETCH: stretch items to fill cross size.
 * - NO_FLEX_ALIGN: keep element's own position on cross axis.
 */
enum FlexAlign {
    FLEX_ALIGN_START,
    FLEX_ALIGN_END,
    FLEX_ALIGN_CENTER,
    FLEX_ALIGN_STRETCH,
    NO_FLEX_ALIGN
};

/**
 * @struct FlexItem
 * @brief Represents one child entry inside a FlexContainer.
 *
 * - element: pointer to the Element placed by this item.
 * - flex_grow: factor used to grow when extra space is available.
 * - flex_shrink: factor used to shrink when space is constrained.
 * - flex_basis: preferred base size along the main axis (if >= 0).
 * - width / height: computed size used during layout.
 */
struct FlexItem {
    Element* element;
    float flex_grow;
    float flex_shrink;
    float flex_basis;
    float width;
    float height;
};

/**
 * @struct FlexContainer
 * @brief Container that lays out multiple FlexItem entries.
 *
 * - items: List\* of FlexItem\* elements.
 * - direction: main axis direction.
 * - justify_content: distribution along the main axis.
 * - align_items: alignment along the cross axis.
 * - gap: space between consecutive items along the main axis.
 * - x, y: origin position of the container.
 * - width, height: available size for layout.
 */
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

/**
 * @brief Allocate and initialize a new FlexContainer.
 *
 * @param x Container X origin.
 * @param y Container Y origin.
 * @param width Container width in pixels.
 * @param height Container height in pixels.
 * @return Pointer to newly allocated FlexContainer or NULL on failure.
 */
FlexContainer* FlexContainer_new(float x, float y, float width, float height);

/**
 * @brief Destroy a FlexContainer and free its resources.
 *
 * Also frees internal FlexItem instances stored in the items list.
 *
 * @param container Pointer to FlexContainer to destroy.
 */
void FlexContainer_destroy(FlexContainer* container);

/**
 * @brief Set the main axis direction for the container.
 *
 * @param container Pointer to FlexContainer.
 * @param direction Desired FlexDirection.
 */
void FlexContainer_setDirection(FlexContainer* container, FlexDirection direction);

/**
 * @brief Set how items are justified along the main axis.
 *
 * @param container Pointer to FlexContainer.
 * @param justify Desired FlexJustify policy.
 */
void FlexContainer_setJustifyContent(FlexContainer* container, FlexJustify justify);

/**
 * @brief Set alignment of items on the cross axis.
 *
 * @param container Pointer to FlexContainer.
 * @param align Desired FlexAlign policy.
 */
void FlexContainer_setAlignItems(FlexContainer* container, FlexAlign align);

/**
 * @brief Set the gap (spacing) between consecutive items.
 *
 * @param container Pointer to FlexContainer.
 * @param gap Gap in pixels.
 */
void FlexContainer_setGap(FlexContainer* container, float gap);

/**
 * @brief Add an Element to the container with flex parameters.
 *
 * The container takes ownership of the created FlexItem (but not the Element).
 *
 * @param container Pointer to FlexContainer.
 * @param element Element pointer to add.
 * @param flex_grow Grow factor when space is available.
 * @param flex_shrink Shrink factor when space is constrained.
 * @param flex_basis Preferred base size along the main axis (use -1 to ignore).
 */
void FlexContainer_addElement(FlexContainer* container, Element* element, float flex_grow, float flex_shrink, float flex_basis);

/**
 * @brief Perform layout: compute sizes and positions for all items/elements.
 *
 * Uses the container's direction, justify and align settings as well as
 * each item's flex parameters to set element positions/sizes.
 *
 * @param container Pointer to FlexContainer to layout.
 */
void FlexContainer_layout(FlexContainer* container);

/**
 * @brief Remove and free all FlexItem entries from the container.
 *
 * Does not destroy the Element instances.
 *
 * @param container Pointer to FlexContainer.
 */
void FlexContainer_clear(FlexContainer* container);

/**
 * @brief Update the container's available size for subsequent layouts.
 *
 * @param container Pointer to FlexContainer.
 * @param width New width in pixels.
 * @param height New height in pixels.
 */
void FlexContainer_setSize(FlexContainer* container, float width, float height);
