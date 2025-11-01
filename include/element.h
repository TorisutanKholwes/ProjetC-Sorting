/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

enum ElementType {
    ELEMENT_TYPE_BUTTON,
    ELEMENT_TYPE_TEXT,
    ELEMENT_TYPE_INPUT,
    ELEMENT_TYPE_SELECT,

    ELEMENT_TYPE_BOX,
    ELEMENT_TYPE_CIRCLE,
    ELEMENT_TYPE_POLYGON,

    ELEMENT_TYPE_IMAGE,

    ELEMENT_TYPE_CONTAINER,
    ELEMENT_TYPE_CHECKBOX
};

/**
 * @brief A generic UI element that can represent various types of UI components.
 *
 * Used to store and manage different UI elements in a unified way.
 * Each element has a type and a unique identifier.
 * Used for rendering, updating, focusing, and unfocusing UI components event if they are of different types.
 */
struct Element {
    /**
     * @brief The type of the element (e.g., button, text, input box, etc.).
     *
     * The type determines which member of the union is valid.
     * If you select an invalid type, only God knows what will happen.
     */
    ElementType type;

    /**
     * @brief The unique identifier for the element.
     *
     * May be NULL if the element does not require an identifier.
     */
    char* id;

    /**
     * @brief The data associated with the element.
     *
     * Must be accessed based on the element type.
     */
    union {
        Button* button;
        Text* text;
        InputBox* input_box;
        Box* box;
        Circle* circle;
        Polygon* polygon;
        Image* image;
        Container* container;
        Select* select;
        Checkbox* checkbox;
    } data;
};

/**
 * @brief Creates an Element from a Button.
 *
 * @param button - The Button to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromButton(Button* button, const char* id);

/**
 * @brief Creates an Element from a Text.
 *
 * @param text - The Text to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromText(Text* text, const char* id);

/**
 * @brief Creates an Element from an InputBox.
 *
 * @param input - The InputBox to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromInput(InputBox* input, const char* id);

/**
 * @brief Creates an Element from a Box.
 *
 * @param box - The Box to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromBox(Box* box, const char* id);

/**
 * @brief Creates an Element from a Circle.
 *
 * @param circle - The Circle to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromCircle(Circle* circle, const char* id);

/**
 * @brief Creates an Element from a Polygon.
 *
 * @param polygon - The Polygon to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromPolygon(Polygon* polygon, const char* id);

/**
 * @brief Creates an Element from an Image.
 *
 * @param image - The Image to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromImage(Image* image, const char* id);

/**
 * @brief Creates an Element from a Container.
 *
 * @param container - The Container to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromContainer(Container* container, const char* id);

/**
 * @brief Creates an Element from a Select.
 *
 * @param select - The Select to wrap.
 * @param id - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromSelect(Select* select, const char* id);

/**
 * @brief Creates an Element from a Checkbox.
 *
 * @param checkbox - The Checkbox to wrap.
 * @param id  - The unique identifier for the element or NULL if not needed.
 *
 * @return A pointer to the created Element.
 */
Element* Element_fromCheckbox(Checkbox* checkbox, const char* id);

/**
 * @brief Destroys an Element and frees its resources.
 * Destroy also the underlying UI component based on its type.
 *
 * @param element - The Element to destroy.
 */
void Element_destroy(Element* element);

/**
 * @brief Destroys a list of Elements and frees their resources.
 *
 * Call Element_destroy on each Element in the list.
 *
 * @param list - The list of Elements to destroy.
 */
void Element_destroyList(List* list);

/**
 * @brief Renders the Element using the provided SDL_Renderer.
 *
 * @param element - The Element to render.
 * @param renderer - The SDL_Renderer to use for rendering.
 */
void Element_render(Element* element, SDL_Renderer* renderer);

/**
 * @brief Updates the Element's state.
 *
 * @param element - The Element to update.
 */
void Element_update(Element* element);

/**
 * @brief Focus the element if applicable.
 *
 * @param element - The Element to focus.
 */
void Element_focus(Element* element);

/**
 * @brief Unfocus the element if applicable.
 *
 * @param element - The Element to unfocus.
 */
void Element_unfocus(Element* element);

/**
 * @brief Renders a list of Elements.
 *
 * Call Element_render on each Element in the list.
 *
 * @param list - The list of Elements to render.
 * @param renderer - The SDL_Renderer to use for rendering.
 */
void Element_renderList(List* list, SDL_Renderer* renderer);

/**
 * @brief Updates a list of Elements.
 *
 * Call Element_update on each Element in the list.
 *
 * @param list - The list of Elements to update.
 */
void Element_updateList(List* list);

/**
 * @brief Focuses all focusable elements in the list.
 *
 * Call Element_focus on each Element in the list.
 *
 * @param list - The list of Elements to focus.
 */
void Element_focusList(List* list);

/**
 * @brief Unfocuses all focusable elements in the list.
 *
 * @param list - The list of Elements to unfocus.
 */
void Element_unfocusList(List* list);

/**
 * @brief Retrieves an Element from the list by its unique identifier.
 *
 * If multiple elements have the same id, the first one found is returned.
 *
 * @param list - The list of Elements to search.
 * @param id - The unique identifier of the Element to find.
 * @return A pointer to the Element with the specified id, or NULL if not found.
 */
Element* Element_getById(List* list, const char* id);

/**
 * @brief Converts an ElementType to its string representation.
 *
 * @param type - The ElementType to convert.
 * @return A string representing the ElementType.
 */
char* ElementType_toString(ElementType type);

/**
 * @brief Sets the position of the Element.
 *
 * @param element - The Element to set the position for.
 * @param x - The x coordinate.
 * @param y - The y coordinate.
 */
void Element_setPosition(Element *element, float x, float y);

/**
 * @brief Gets the position of the Element.
 *
 * @param element - The Element to get the position from.
 * @param x - A pointer to store the x coordinate.
 * @param y - A pointer to store the y coordinate.
 */
void Element_getPosition(Element* element, float *x, float *y);