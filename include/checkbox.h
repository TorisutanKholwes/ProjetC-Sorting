/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Checkbox
 * @brief Represents a checkbox UI element.
 *
 * The Checkbox stores its visual properties (position, size, colors),
 * input handling reference, current checked/focused state, an optional
 * parent pointer, and a callback invoked when the checked state changes.
 */
struct Checkbox {
    /**
     * @brief Pointer to the position object (x, y).
     *
     * Owned externally; typically created with Position_new and freed elsewhere.
     */
    Position* position;

    /**
     * @brief Dimensions of the checkbox (width and height).
     */
    Size size;

    /**
     * @brief Color used to draw the checkbox border.
     *
     * Pointer to a Color object.
     */
    Color* color;

    /**
     * @brief Background color used to fill the checkbox when checked.
     *
     * Pointer to a Color object.
     */
    Color* bg_color;

    /**
     * @brief Reference to the input manager used for event handling.
     */
    Input* input;

    /**
     * @brief Whether the checkbox is currently checked.
     */
    bool checked;

    /**
     * @brief Whether the checkbox is currently focused (listening for events).
     */
    bool focused;

    /**
     * @brief Optional parent pointer (user-defined).
     *
     * Can be used to associate the checkbox with containing UI or data.
     */
    void* parent;

    /**
     * @brief Callback invoked when the checked state changes.
     *
     * Signature: void (*EventHandlerFunc)(Input* input, SDL_Event* evt, Checkbox* checkbox)
     * The callback receives the input manager, the triggering SDL event (may be NULL),
     * and the checkbox instance.
     */
    EventHandlerFunc onChange;
};

/**
 * @brief Create a new Checkbox instance.
 *
 * @param x Initial x position.
 * @param y Initial y position.
 * @param size Size (width and height) of the checkbox (square).
 * @param color Pointer to the border color.
 * @param bg_color Pointer to the fill color when checked.
 * @param default_checked Initial checked state.
 * @param input Pointer to the Input manager used for event handling.
 * @param parent Optional parent pointer.
 * @return Pointer to the newly allocated Checkbox, or NULL on allocation failure.
 */
Checkbox* Checkbox_new(float x, float y, float size, Color* color, Color* bg_color, bool default_checked, Input* input, void* parent);

/**
 * @brief Destroy a Checkbox and free its resources.
 *
 * Removes event handlers and frees memory allocated for the checkbox structure.
 *
 * @param checkbox Pointer to the Checkbox to destroy. Safe to pass NULL.
 */
void Checkbox_destroy(Checkbox* checkbox);

/**
 * @brief Set the checked state of the checkbox.
 *
 * @param checkbox Pointer to the Checkbox.
 * @param checked New checked state.
 */
void Checkbox_setChecked(Checkbox* checkbox, bool checked);

/**
 * @brief Query whether the checkbox is checked.
 *
 * @param checkbox Pointer to the Checkbox.
 * @return true if checked, false otherwise or if checkbox is NULL.
 */
bool Checkbox_isChecked(Checkbox* checkbox);

/**
 * @brief Toggle the checked state of the checkbox.
 *
 * @param checkbox Pointer to the Checkbox.
 */
void Checkbox_toggle(Checkbox* checkbox);

/**
 * @brief Render the checkbox using the provided SDL_Renderer.
 *
 * Draws the checkbox border and, if checked, fills the interior.
 *
 * @param checkbox Pointer to the Checkbox.
 * @param renderer SDL_Renderer used for drawing. If NULL, the call is ignored.
 */
void Checkbox_render(Checkbox* checkbox, SDL_Renderer* renderer);

/**
 * @brief Update the checkbox state each frame.
 *
 * Typically used to ensure the checkbox has focus and is ready to handle input.
 *
 * @param checkbox Pointer to the Checkbox.
 */
void Checkbox_update(Checkbox* checkbox);

/**
 * @brief Give focus to the checkbox and register its event handler.
 *
 * When focused, the checkbox listens for mouse events to toggle its state.
 *
 * @param checkbox Pointer to the Checkbox.
 */
void Checkbox_focus(Checkbox* checkbox);

/**
 * @brief Remove focus from the checkbox and unregister its event handler.
 *
 * @param checkbox Pointer to the Checkbox.
 */
void Checkbox_unFocus(Checkbox* checkbox);

/**
 * @brief Set the checkbox position.
 *
 * @param checkbox Pointer to the Checkbox.
 * @param x New x coordinate.
 * @param y New y coordinate.
 */
void Checkbox_setPosition(Checkbox* checkbox, float x, float y);

/**
 * @brief Get the Position pointer of the checkbox.
 *
 * @param checkbox Pointer to the Checkbox.
 * @return Position pointer or NULL if checkbox is NULL.
 */
Position* Checkbox_getPosition(Checkbox* checkbox);

/**
 * @brief Get the size of the checkbox.
 *
 * @param checkbox Pointer to the Checkbox.
 * @return Size struct representing width and height. Returns {0,0} if checkbox is NULL.
 */
Size Checkbox_getSize(Checkbox* checkbox);

/**
 * @brief Set the onChange event handler for the checkbox.
 *
 * The handler is invoked whenever the checkbox changes state due to user interaction.
 *
 * @param checkbox Pointer to the Checkbox.
 * @param handler Event handler function to call on state change.
 */
void Checkbox_onChange(Checkbox* checkbox, EventHandlerFunc handler);
