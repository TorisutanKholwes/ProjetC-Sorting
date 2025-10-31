/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "layout.h"

#include "button.h"
#include "element.h"
#include "geometry.h"
#include "input_box.h"
#include "list.h"
#include "logger.h"
#include "style.h"
#include "text.h"
#include "utils.h"

static void FlexItem_getElementSize(FlexItem *item, float *width, float *height) {
    Element *element = item->element;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON: {
            Button *button = element->data.button;
            EdgeInsets *paddings = button->style->paddings;
            *width = button->rect.w + (button->style->border_width * 2) + (paddings->left + paddings->right);
            *height = button->rect.h + (button->style->border_width * 2) + (paddings->top + paddings->bottom);
            break;
        }
        case ELEMENT_TYPE_TEXT: {
            Text *text = element->data.text;
            Size size = Text_getSize(text);
            *width = size.width;
            *height = size.height;
            break;
        }
        case ELEMENT_TYPE_INPUT: {
            InputBox *input = element->data.input_box;
            *width = input->rect.w;
            *height = input->rect.h;
            break;
        }
        case ELEMENT_TYPE_BOX: {
            Box *box = element->data.box;
            *width = box->size.width;
            *height = box->size.height;
            break;
        }
        case ELEMENT_TYPE_CIRCLE: {
            Circle *circle = element->data.circle;
            *width = circle->radius * 2;
            *height = circle->radius * 2;
            break;
        }
        default:
            *width = 0;
            *height = 0;
            break;
    }
}

static void FlexItem_getElementPosition(FlexItem *item, float *x, float *y) {
    if (!item || !item->element) {
        *x = 0;
        *y = 0;
        return;
    }

    Element *element = item->element;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button *button = element->data.button;
            *x = button->rect.x;
            *y = button->rect.y;
            break;
        case ELEMENT_TYPE_TEXT:
            Text *text = element->data.text;
            *x = text->position->x;
            *y = text->position->y;
            break;
        case ELEMENT_TYPE_INPUT: {
            InputBox *input = element->data.input_box;
            *x = input->rect.x;
            *y = input->rect.y;
            break;
        }
        case ELEMENT_TYPE_BOX: {
            Box *box = element->data.box;
            if (box->position) {
                *x = box->position->x;
                *y = box->position->y;
            } else {
                *x = 0;
                *y = 0;
            }
            break;
        }
        case ELEMENT_TYPE_CIRCLE: {
            Circle *circle = element->data.circle;
            if (circle->center) {
                *x = circle->center->x;
                *y = circle->center->y;
            } else {
                *x = 0;
                *y = 0;
            }
            break;
        }
        default:
            *x = 0;
            *y = 0;
            break;
    }
}

static void FlexItem_setElementPosition(FlexItem *item, float x, float y) {
    if (!item || !item->element) return;

    Element *element = item->element;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_setPosition(element->data.button, x, y);
            break;
        case ELEMENT_TYPE_TEXT:
            Text_setPosition(element->data.text, x, y);
            break;
        case ELEMENT_TYPE_INPUT: {
            InputBox *input = element->data.input_box;
            input->rect.x = x == -1 ? input->rect.x : x;
            input->rect.y = y == -1 ? input->rect.y : y;
            break;
        }
        case ELEMENT_TYPE_BOX: {
            Box *box = element->data.box;
            if (box->position) {
                box->position->x = x == -1 ? box->position->x : x;
                box->position->y = y == -1 ? box->position->y : y;
            }
            break;
        }
        case ELEMENT_TYPE_CIRCLE: {
            Circle *circle = element->data.circle;
            if (circle->center) {
                circle->center->x = x == -1 ? circle->center->x : x;
                circle->center->y = y == -1 ? circle->center->y : y;
            }
            break;
        }
        default:
            break;
    }
}

static void FlexItem_setElementSize(FlexItem *item, float width, float height) {
    if (!item || !item->element) return;

    Element *element = item->element;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_setSize(element->data.button, width, height);
            break;
        case ELEMENT_TYPE_TEXT:
            Text_setSize(element->data.text, width, height);
            break;
        case ELEMENT_TYPE_INPUT: {
            InputBox *input = element->data.input_box;
            input->rect.w = width;
            input->rect.h = height;
            break;
        }
        case ELEMENT_TYPE_BOX: {
            Box *box = element->data.box;
            box->size.width = width;
            box->size.height = height;
            break;
        }
        case ELEMENT_TYPE_CIRCLE: {
            Circle *circle = element->data.circle;
            circle->radius = (int) (width / 2);
            break;
        }
        default:
            break;
    }
}

FlexContainer *FlexContainer_new(const float x, const float y, const float width, const float height) {
    FlexContainer *container = calloc(1, sizeof(FlexContainer));
    if (!container) {
        error("FlexContainer_new: Failed to allocate memory for FlexContainer");
        return NULL;
    }

    container->items = List_create();
    if (!container->items) {
        error("FlexContainer_new: Failed to create items list");
        safe_free((void **) &container);
        return NULL;
    }

    container->direction = FLEX_DIRECTION_ROW;
    container->justify_content = FLEX_JUSTIFY_START;
    container->align_items = FLEX_ALIGN_START;
    container->gap = 0.0f;
    container->x = x;
    container->y = y;
    container->width = width;
    container->height = height;

    return container;
}

void FlexContainer_destroy(FlexContainer *container) {
    if (!container) return;

    if (container->items) {
        ListIterator *it = ListIterator_new(container->items);
        while (ListIterator_hasNext(it)) {
            FlexItem *item = ListIterator_next(it);
            safe_free((void **) &item);
        }
        ListIterator_destroy(it);
        List_destroy(container->items);
    }

    safe_free((void **) &container);
}

void FlexContainer_setDirection(FlexContainer *container, const FlexDirection direction) {
    if (!container) return;
    container->direction = direction;
}

void FlexContainer_setJustifyContent(FlexContainer *container, const FlexJustify justify) {
    if (!container) return;
    container->justify_content = justify;
}

void FlexContainer_setAlignItems(FlexContainer *container, const FlexAlign align) {
    if (!container) return;
    container->align_items = align;
}

void FlexContainer_setGap(FlexContainer *container, const float gap) {
    if (!container) return;
    container->gap = gap;
}

void FlexContainer_addElement(FlexContainer *container, Element *element, float flex_grow, float flex_shrink,
                              float flex_basis) {
    if (!container || !element) return;

    FlexItem *item = calloc(1, sizeof(FlexItem));
    if (!item) {
        error("FlexContainer_addElement: Failed to allocate memory for FlexItem");
        return;
    }
    item->element = element;
    item->flex_grow = flex_grow;
    item->flex_shrink = flex_shrink;
    item->flex_basis = flex_basis;

    FlexItem_getElementSize(item, &item->width, &item->height);

    if (flex_basis >= 0) {
        if (container->direction == FLEX_DIRECTION_ROW || container->direction == FLEX_DIRECTION_ROW_REVERSE) {
            item->width = flex_basis;
        } else {
            item->height = flex_basis;
        }
    }

    List_push(container->items, item);
}

void FlexContainer_layout(FlexContainer *container) {
    if (!container || !container->items) return;

    int item_count = List_size(container->items);
    if (item_count == 0) return;

    bool is_row = container->direction == FLEX_DIRECTION_ROW || container->direction == FLEX_DIRECTION_ROW_REVERSE;
    bool is_reverse = container->direction == FLEX_DIRECTION_ROW_REVERSE || container->direction ==
                      FLEX_DIRECTION_COLUMN_REVERSE;

    float total_main_size = 0;
    float total_flex_grow = 0;
    float total_flex_shrink = 0;
    float total_gap = container->gap * (item_count - 1);

    ListIterator *it = ListIterator_new(container->items);
    while (ListIterator_hasNext(it)) {
        FlexItem *item = ListIterator_next(it);
        total_main_size += is_row ? item->width : item->height;
        if (item->element->type == ELEMENT_TYPE_BUTTON) {
            Button *btn = item->element->data.button;
            total_main_size += is_row
                                   ? btn->style->paddings->left + btn->style->paddings->right
                                   : btn->style->paddings->top + btn->style->paddings->bottom;
        }
        total_flex_grow += item->flex_grow;
        total_flex_shrink += item->flex_shrink;
    }
    ListIterator_destroy(it);

    float available_space = (is_row ? container->width : container->height) - total_main_size - total_gap;

    if (available_space > 0 && total_flex_grow > 0) {
        it = ListIterator_new(container->items);
        while (ListIterator_hasNext(it)) {
            FlexItem *item = ListIterator_next(it);
            if (item->flex_grow > 0) {
                float extra = (available_space * item->flex_grow) / total_flex_grow;
                if (is_row) {
                    item->width += extra;
                } else {
                    item->height += extra;
                }
                //log_message(LOG_LEVEL_DEBUG, "Width : %f Height : %f", item->width, item->height);
            }
        }
        ListIterator_destroy(it);
        available_space = 0;
    } else if (available_space < 0 && total_flex_shrink > 0) {
        float shrink_amount = -available_space;
        it = ListIterator_new(container->items);
        while (ListIterator_hasNext(it)) {
            FlexItem *item = ListIterator_next(it);
            if (item->flex_shrink > 0) {
                float reduction = (shrink_amount * item->flex_shrink) / total_flex_shrink;
                if (is_row) {
                    item->width = fmaxf(0, item->width - reduction);
                } else {
                    item->height = fmaxf(0, item->height - reduction);
                }
            }
        }
        ListIterator_destroy(it);
        available_space = 0;
    }


    float main_start = 0;
    float item_spacing = 0;

    switch (container->justify_content) {
        case FLEX_JUSTIFY_START:
            main_start = 0;
            break;
        case FLEX_JUSTIFY_END:
            main_start = available_space;
            break;
        case FLEX_JUSTIFY_CENTER:
            main_start = available_space / 2;
            break;
        case FLEX_JUSTIFY_SPACE_BETWEEN:
            main_start = 0;
            item_spacing = (item_count > 1) ? available_space / (item_count - 1) : 0;
            break;
        case FLEX_JUSTIFY_SPACE_AROUND:
            item_spacing = available_space / item_count;
            main_start = item_spacing / 2;
            break;
        case FLEX_JUSTIFY_SPACE_EVENLY:
            item_spacing = available_space / (item_count + 1);
            main_start = item_spacing;
            break;
    }

    float current_main = main_start;

    it = ListIterator_new(container->items);
    while (ListIterator_hasNext(it)) {
        FlexItem *item = ListIterator_next(it);
        float eltX, eltY;
        FlexItem_getElementPosition(item, &eltX, &eltY);

        float cross_pos = 0;
        switch (container->align_items) {
            case FLEX_ALIGN_START:
                cross_pos = 0;
                break;
            case FLEX_ALIGN_END:
                cross_pos = (is_row ? container->height - item->height : container->width - item->width);
                break;
            case FLEX_ALIGN_CENTER:
                cross_pos = (is_row ? (container->height - item->height) / 2 : (container->width - item->width) / 2);
                break;
            case FLEX_ALIGN_STRETCH:
                cross_pos = 0;
                if (is_row) {
                    item->height = container->height;
                } else {
                    item->width = container->width;
                }
                break;
            case NO_FLEX_ALIGN:
                cross_pos = is_row ? eltY : eltX;
                while (cross_pos >= (is_row ? container->height : container->width)) {
                    cross_pos -= (is_row ? container->height : container->width);
                }
                break;
        }

        FlexItem_setElementSize(item, item->width, item->height);

        float x, y;
        if (is_row) {
            x = container->x + (is_reverse ? container->width - current_main - item->width : current_main);
            y = container->y + cross_pos;
        } else {
            x = container->x + cross_pos;
            y = container->y + (is_reverse ? container->height - current_main - item->height : current_main);
        }

        FlexItem_setElementPosition(item, x, y);

        float main_size = is_row ? item->width : item->height;
        if (item->element->type == ELEMENT_TYPE_BUTTON) {
            Button *btn = item->element->data.button;
            float main_size_add = 0;
            switch (container->direction) {
                case FLEX_DIRECTION_ROW:
                    main_size_add += btn->style->paddings->right;
                    break;
                case FLEX_DIRECTION_ROW_REVERSE:
                    main_size_add += btn->style->paddings->left;
                    break;
                case FLEX_DIRECTION_COLUMN:
                    main_size_add += btn->style->paddings->bottom;
                    break;
                case FLEX_DIRECTION_COLUMN_REVERSE:
                    main_size_add += btn->style->paddings->top;
                    break;
                default:
                    break;
            }
            main_size += main_size_add;
        }
        current_main += main_size + container->gap + item_spacing;
    }
    ListIterator_destroy(it);
}

void FlexContainer_clear(FlexContainer* container) {
    if (!container) return;

    ListIterator* it = ListIterator_new(container->items);
    while (ListIterator_hasNext(it)) {
        FlexItem* item = ListIterator_next(it);
        safe_free((void**)&item);
    }
    ListIterator_destroy(it);
    List_clear(container->items);
}

void FlexContainer_setSize(FlexContainer* container, float width, float height) {
    if (!container) return;
    container->width = width;
    container->height = height;
}