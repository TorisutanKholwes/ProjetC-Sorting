/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Select
 * @brief Represents a simple selectable UI control that cycles through a list of options.
 *
 * The Select object owns a Text instance used to render the currently selected option,
 * and references an Input to register mouse events. Options are provided as a List of
 * C-strings. An optional onChange callback is invoked when the selection changes.
 */
struct Select {
    /** Input subsystem used to register event handlers. */
    Input* input;

    /** Text object used to render the current selected option. */
    Text* text;

    /** Rectangle describing the text area and used for layout/hover testing. */
    SDL_FRect rect;

    /** Visual style used to draw the control (borders, paddings, colors, fonts). */
    ButtonStyle* style;

    /**
     * When true, the rect coordinates are treated as the center of the control.
     * When false, rect represents the top-left corner.
     */
    bool from_center;

    /** List of option strings. Options are not deep-copied by this structure. */
    List* options;

    /** Index of the currently selected option (0-based). */
    int selected_index;

    /** Whether the control is currently focused (handlers registered). */
    bool focused;

    /**
     * Optional callback invoked when selection changes.
     * Signature matches EventHandlerFunc and receives (Input*, SDL_Event*, Select*).
     */
    EventHandlerFunc onChange;

    /** Optional parent pointer (UI container or owner) for event routing or context. */
    void* parent;
};

/**
 * @brief Create a new Select control.
 *
 * The returned Select is allocated on the heap and must be freed with Select_destroy.
 *
 * @param app Pointer to the global App context (provides renderer and input).
 * @param position Initial position used to create the control's rect.
 * @param from_center If true, position/rect are interpreted as center-based.
 * @param style Visual ButtonStyle to use. Ownership is transferred to the Select.
 * @param parent Optional parent pointer associated with the control.
 * @param options List of C-string options to cycle through (must remain valid for lifetime).
 * @param default_index Index of the option initially selected.
 * @return Pointer to a newly created Select or NULL on allocation failure.
 */
Select* Select_new(const App* app, Position* position, bool from_center, ButtonStyle* style, void* parent, List* options, int default_index);

/**
 * @brief Destroy a Select and free associated resources.
 *
 * This will unregister any input handlers, destroy the internal Text object,
 * free the provided style and destroy the options list. After this call the
 * pointer becomes invalid.
 *
 * @param select Pointer to the Select to destroy (safe to pass NULL).
 */
void Select_destroy(Select* select);

/**
 * @brief Render the Select control.
 *
 * Draws the control background, border and current text using the control's style.
 *
 * @param select Pointer to the Select to render.
 * @param renderer SDL_Renderer used for rendering.
 */
void Select_render(Select* select, SDL_Renderer* renderer);

/**
 * @brief Periodic update called by the UI system.
 *
 * Ensures the control registers input focus and can be used to advance animations.
 *
 * @param select Pointer to the Select to update.
 */
void Select_update(Select* select);

/**
 * @brief Register the Select's input handlers and mark it focused.
 *
 * @param select Pointer to the Select to focus.
 */
void Select_focus(Select* select);

/**
 * @brief Unregister the Select's input handlers and remove focus.
 *
 * @param select Pointer to the Select to unfocus.
 */
void Select_unFocus(Select* select);

/**
 * @brief Set the position of the Select's rectangle.
 *
 * @param select Pointer to the Select.
 * @param x New x coordinate.
 * @param y New y coordinate.
 */
void Select_setPosition(Select* select, float x, float y);

/**
 * @brief Set the size of the Select's rectangle (text area).
 *
 * @param select Pointer to the Select.
 * @param w New width.
 * @param h New height.
 */
void Select_setSize(Select* select, float w, float h);

/**
 * @brief Associate a parent pointer with the Select control.
 *
 * @param select Pointer to the Select.
 * @param parent Parent pointer (opaque to Select).
 */
void Select_setParent(Select* select, void* parent);

/**
 * @brief Set the onChange callback invoked when the selection changes.
 *
 * @param select Pointer to the Select.
 * @param func EventHandlerFunc to call on changes.
 */
void Select_onChange(Select* select, EventHandlerFunc func);

/**
 * @brief Set the currently selected option by index.
 *
 * Index must be within bounds of the options list.
 *
 * @param select Pointer to the Select.
 * @param index New selected index (0-based).
 */
void Select_setSelectedIndex(Select* select, int index);

/**
 * @brief Get the currently selected index.
 *
 * @param select Pointer to the Select.
 * @return Current selected index or -1 if select is NULL.
 */
int Select_getSelectedIndex(Select* select);

/**
 * @brief Get the currently selected option string.
 *
 * @param select Pointer to the Select.
 * @return C-string of the selected option or NULL if select is NULL.
 */
char* Select_getSelectedOption(Select* select);

/**
 * @brief Compute the total control size including paddings and border.
 *
 * @param select Pointer to the Select.
 * @return Size struct with width and height (both 0 if select is NULL).
 */
Size Select_getSize(Select* select);
