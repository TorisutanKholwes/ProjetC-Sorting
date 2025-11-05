/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"


/**
 * @struct Button
 * @brief Represents a clickable UI button.
 *
 * This structure holds rendering, input and state information for a UI
 * button, including text content, layout rectangle, visual styles,
 * input handling and event callbacks.
 */
struct Button {
    /**
     * @brief Pointer to the text object rendered inside the button
     *
     * May be NULL if the button has no label.
     */
    Text* text;

    /**
     * @brief Floating-point rectangle defining the button's position and size
     */
    SDL_FRect rect;

    /**
     * @brief Pointer to the active visual style for the button.
     *
     * Defines colors, padding, border, etc. May be shared between widgets.
     */
    ButtonStyle* style;

    /**
     * @brief Pointer to a copy of the button's style.
     *
     * Used to temporarily override visual properties (hover/pressed/focus)
     * and restore the original appearance later.
     */
    ButtonStyle* copy_style;

    /**
     * @brief Pointer to the input handling structure
     *
     * Used for link event handler callbacks to the main input system.
     */
    Input* input;

    /**
     * @brief State flags indicating interaction states of the button
     */
    bool hovered;
    bool pressed;
    bool focused;

    /**
     * @brief Indicates if the button's position is defined from its center
     *
     * If true, the rect.x and rect.y represent the center of the button.
     * If false, they represent the top-left corner.
     */
    bool from_center;

    /**
     * @brief Pointer to the parent object of the button
     *
     * Used for hierarchical UI structures. May be NULL if no parent.
     * Can also be used for going back references in event handlers.
     */
    void* parent;

    /**
     * @brief Event handler callbacks for button interactions
     *
     * onClick: Called when the button is clicked.
     * onHover: Called when the button is hovered over.
     * onHoverEnd: Called when the hover ends.
     */
    EventHandlerFunc onClick;
    EventHandlerFunc onHover;
    EventHandlerFunc onHoverEnd;
};

/**
 * @brief Create a new Button instance
 *
 * @param app The main application instance
 * @param position The position of the button
 * @param from_center Whether the position args is the center of the button or the top-left corner
 * @param style The visual style to apply to the button
 * @param parent The parent object of the button
 * @param label The text label of the button
 *
 * @return A pointer to the newly created Button instance
 */
Button* Button_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* label);

/**
 * @brief Create a new Button instance with formatted label
 *
 * Used to create a button with a label generated from a format string.
 *
* @param app The main application instance
 * @param position The position of the button
 * @param from_center Whether the position args is the center of the button or the top-left corner
 * @param style The visual style to apply to the button
 * @param parent The parent object of the button
 * @param format The format string for the button label
 * @param ... The variable arguments for the format string
 *
 * @return A pointer to the newly created Button instance
 */
Button* Button_newf(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, const char* format, ...);

/**
 * @brief Destroy a Button instance
 *
 * @param button The Button instance to be destroyed
 */
void Button_destroy(Button* button);

/**
 * @brief Render the button using the given SDL renderer
 *
 * This function is usually called every frame when rendering the UI.
 *
 * @param button The Button instance to be rendered
 * @param renderer The SDL renderer to use for rendering
 */
void Button_render(Button* button, SDL_Renderer* renderer);

/**
 * @brief Set the button's label text with a plain string
 *
 * @param button The Button instance
 * @param str The new label string
 */
void Button_setString(Button* button, const char* str);

/**
 * @brief Set the button's label text with a formatted string
 *
 * @param button The Button instance
 * @param format The format string
 * @param ... The variable arguments for the format string
 */
void Button_setStringf(Button* button, const char* format, ...);

/**
 * @brief Set the button's position
 *
 * @param button The Button instance
 * @param x the x coordinate
 * @param y the y coordinate
 */
void Button_setPosition(Button* button, float x, float y);

/**
 * @brief Set the button's size
 *
 * @param button The Button instance
 * @param w  the width
 * @param h  the height
 */
void Button_setSize(Button* button, float w, float h);

/**
 * @brief Set the parent object of the button
 *
 * @param button The Button instance
 * @param parent The parent object
 */
void Button_setParent(Button* button, void* parent);

/**
 * @brief Update the button's state based on input
 *
 * This functon is usually called every frame when updating the UI.
 *
 * @param button The Button instance
 */
void Button_update(Button* button);

/**
 * @brief Remove focus from the button
 *
 * This function is usually called when the Frame where the button is located loses focus.
 * Used to remove all event listeners linked to this button.
 *
 * @param button The Button instance
 */
void Button_unFocus(Button* button);

/**
 * @brief Set focus to the button
 *
 * This function is usually called when the Frame where the button is located gains focus.
 * Used to set up all event listeners linked to this button.
 *
 * @param button The Button instance
 */
void Button_focus(Button* button);

/**
 * @brief Link the event handler of the button click event to a function
 *
 * @param button The Button instance
 * @param func The function to be called on click event
 */
void Button_onClick(Button* button, EventHandlerFunc func);

/**
 * @brief Link the event handler of the button hover event to a function
 *
 * @param button The Button instance
 * @param func The function to be called on hover event
 */
void Button_onHover(Button* button, EventHandlerFunc func);

/**
 * @brief Link the event handler of the button hover end event to a function
 * @param button The Button instance
 * @param func The function to be called on hover end event
 */
void Button_onHoverEnd(Button* button, EventHandlerFunc func);

/**
 * @brief Get the size of the button
 *
 * @param button The Button instance
 * @return Size The size of the button
 */
Size Button_getSize(Button* button);

/**
 * @brief Get the text label of the button
 *
 * @param button The Button instance
 * @return char* The text label of the button
 */
char* Button_getText(Button* button);