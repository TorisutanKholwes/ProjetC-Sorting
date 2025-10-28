/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "element.h"

#include "logger.h"
#include "button.h"
#include "container.h"
#include "geometry.h"
#include "image.h"
#include "input_box.h"
#include "list.h"
#include "text.h"
#include "utils.h"

Element* Element_fromButton(Button* button, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromButton: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_BUTTON;
    element->id = Strdup(id);
    element->data.button = button;
    return element;
}

Element* Element_fromText(Text* text, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_TEXT;
    element->id = Strdup(id);
    element->data.text = text;
    return element;
}

Element* Element_fromInput(InputBox* input, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromInput: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_INPUT;
    element->id = Strdup(id);
    element->data.input_box = input;
    return element;
}

Element* Element_fromBox(Box* box, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_BOX;
    element->id = Strdup(id);
    element->data.box = box;
    return element;
}

Element* Element_fromCircle(Circle* circle, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_CIRCLE;
    element->id = Strdup(id);
    element->data.circle = circle;
    return element;
}

Element* Element_fromPolygon(Polygon* polygon, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_POLYGON;
    element->id = Strdup(id);
    element->data.polygon = polygon;
    return element;
}

Element* Element_fromImage(Image* image, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_IMAGE;
    element->id = Strdup(id);
    element->data.image = image;
    return element;
}

Element* Element_fromContainer(Container* container, const char* id) {
    Element* element = calloc(1, sizeof(Element));
    if (!element) {
        error("Element_fromText: Failed to allocate memory for Element");
        return NULL;
    }
    element->type = ELEMENT_TYPE_CONTAINER;
    element->id = Strdup(id);
    element->data.container = container;
    return element;
}

void Element_destroy(Element* element) {
    if (!element) return;

    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_destroy(element->data.button);
            break;
        case ELEMENT_TYPE_TEXT:
            Text_destroy(element->data.text);
            break;
        case ELEMENT_TYPE_BOX:
            Box_destroy(element->data.box);
            break;
        case ELEMENT_TYPE_CIRCLE:
            Circle_destroy(element->data.circle);
            break;
        case ELEMENT_TYPE_POLYGON:
            Polygon_destroy(element->data.polygon);
            break;
        case ELEMENT_TYPE_INPUT:
            InputBox_destroy(element->data.input_box);
            break;
        case ELEMENT_TYPE_IMAGE:
            Image_destroy(element->data.image);
            break;
        case ELEMENT_TYPE_CONTAINER:
            Container_destroy(element->data.container);
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Element_destroy: Unknown element type %d", element->type);
            break;
    }

    safe_free((void**)&element->id);
    safe_free((void**)&element);
}

void Element_destroyList(List* list) {
    if (!list) return;
    List_destroyWitValues(list, (DestroyFunc) Element_destroy);
}


void Element_render(Element* element, SDL_Renderer* renderer) {
    if (!element || !renderer) return;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_render(element->data.button, renderer);
            break;
        case ELEMENT_TYPE_TEXT:
            Text_render(element->data.text);
            break;
        case ELEMENT_TYPE_BOX:
            Box_render(element->data.box, renderer);
            break;
        case ELEMENT_TYPE_CIRCLE:
            Circle_render(element->data.circle, renderer);
            break;
        case ELEMENT_TYPE_POLYGON:
            Polygon_render(element->data.polygon, renderer);
            break;
        case ELEMENT_TYPE_INPUT:
            InputBox_render(element->data.input_box, renderer);
            break;
        case ELEMENT_TYPE_IMAGE:
            Image_render(element->data.image, renderer);
            break;
        case ELEMENT_TYPE_CONTAINER:
            Container_render(element->data.container, renderer);
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Element_render: Unknown element type %d", element->type);
            break;
    }
}

void Element_update(Element* element) {
    if (!element) return;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_update(element->data.button);
            break;
        case ELEMENT_TYPE_INPUT:
            InputBox_update(element->data.input_box);
            break;
        case ELEMENT_TYPE_TEXT:
        case ELEMENT_TYPE_BOX:
        case ELEMENT_TYPE_CIRCLE:
        case ELEMENT_TYPE_POLYGON:
        case ELEMENT_TYPE_IMAGE:
        case ELEMENT_TYPE_CONTAINER:
            // No update needed for text currently
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Element_update: Unknown element type %d", element->type);
            break;
    }
}

void Element_focus(Element* element) {
    if (!element) return;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_focus(element->data.button);
            break;
        case ELEMENT_TYPE_INPUT:
            InputBox_focus(element->data.input_box);
            break;
        case ELEMENT_TYPE_TEXT:
        case ELEMENT_TYPE_BOX:
        case ELEMENT_TYPE_CIRCLE:
        case ELEMENT_TYPE_POLYGON:
        case ELEMENT_TYPE_IMAGE:
        case ELEMENT_TYPE_CONTAINER:
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Element_focus: Unknown element type %d", element->type);
            break;
    }
}

void Element_unfocus(Element* element) {
    if (!element) return;
    switch (element->type) {
        case ELEMENT_TYPE_BUTTON:
            Button_unFocus(element->data.button);
            break;
        case ELEMENT_TYPE_INPUT:
            InputBox_unFocus(element->data.input_box);
            break;
        case ELEMENT_TYPE_TEXT:
        case ELEMENT_TYPE_BOX:
        case ELEMENT_TYPE_CIRCLE:
        case ELEMENT_TYPE_POLYGON:
        case ELEMENT_TYPE_IMAGE:
        case ELEMENT_TYPE_CONTAINER:
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Element_unfocus: Unknown element type %d", element->type);
            break;
    }
}


void Element_renderList(List* list, SDL_Renderer* renderer) {
    ListIterator* iterator = ListIterator_new(list);
    while (ListIterator_hasNext(iterator)) {
        Element* element = ListIterator_next(iterator);
        Element_render(element, renderer);
    }
    ListIterator_destroy(iterator);
}

void Element_updateList(List* list) {
    ListIterator* iterator = ListIterator_new(list);
    while (ListIterator_hasNext(iterator)) {
        Element* element = ListIterator_next(iterator);
        Element_update(element);
    }
    ListIterator_destroy(iterator);
}

void Element_focusList(List* list) {
    ListIterator* iterator = ListIterator_new(list);
    while (ListIterator_hasNext(iterator)) {
        Element* element = ListIterator_next(iterator);
        Element_focus(element);
    }
    ListIterator_destroy(iterator);
}

void Element_unfocusList(List* list) {
    ListIterator* iterator = ListIterator_new(list);
    while (ListIterator_hasNext(iterator)) {
        Element* element = ListIterator_next(iterator);
        Element_unfocus(element);
    }
    ListIterator_destroy(iterator);
}

Element* Element_getById(List* list, const char* id) {
    ListIterator* iterator = ListIterator_new(list);
    while (ListIterator_hasNext(iterator)) {
        Element* element = ListIterator_next(iterator);
        if (String_equals(element->id, id)) {
            ListIterator_destroy(iterator);
            return element;
        }
    }
    ListIterator_destroy(iterator);
    return NULL;
}

char* ElementType_toString(ElementType type) {
    switch (type) {
        case ELEMENT_TYPE_BUTTON:
            return "BUTTON";
        case ELEMENT_TYPE_TEXT:
            return "TEXT";
        case ELEMENT_TYPE_INPUT:
            return "INPUT";
        case ELEMENT_TYPE_BOX:
            return "BOX";
        case ELEMENT_TYPE_CIRCLE:
            return "CIRCLE";
        case ELEMENT_TYPE_POLYGON:
            return "POLYGON";
        case ELEMENT_TYPE_IMAGE:
            return "IMAGE";
        case ELEMENT_TYPE_CONTAINER:
            return "CONTAINER";
        default:
            return "UNKNOWN";
    }
}

void Element_setPosition(Element *element, float x, float y) {
    if (!element) return;

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

void Element_getPosition(Element* element, float *x, float *y) {
    if (!element) {
        *x = 0;
        *y = 0;
        return;
    }

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