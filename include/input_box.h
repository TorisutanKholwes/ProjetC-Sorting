/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct InputBox
 * @brief Represents a single-line editable text field.
 *
 * The InputBox stores rendering and input references, current text buffer,
 * cursor state, styling and ownership context. It integrates with the App's
 * Input to receive SDL events and uses a Timer to blink the caret.
 */
struct InputBox {
    /** @brief Pointer to the application context (renderer, input, resources). */
    App* app;

    /** @brief Reference to the global Input subsystem used for event handling. */
    Input* input;

    /** @brief Timer used to blink the input cursor. */
    Timer* timer;

    /** @brief Text helper used to render the box contents. */
    Text* text;

    /** @brief Rectangle (position and size) of the input box in screen coordinates. */
    SDL_FRect rect;

    /** @brief Null-terminated string currently stored in the input box. */
    char* str;

    /** @brief Cursor index (in bytes) inside \p str where new characters are inserted. */
    int cursor_index;

    /** @brief If true, the displayed string is masked (e.g. password entry). */
    bool password_mode;

    /** @brief Pointer to style configuration for this input box. */
    InputBoxStyle* style;

    /** @brief Whether the input box has logical focus (receives update calls). */
    bool focused;

    /** @brief Whether the input box is currently selected for text input. */
    bool selected;

    /** @brief Whether the cursor is currently visible (blinking). */
    bool cursor_visible;

    /** @brief Optional user-defined parent pointer (owner context). */
    void* parent;
};

/**
 * @brief Allocate and initialize a new InputBox instance.
 *
 * @param app Pointer to the App instance (provides renderer and input).
 * @param rect SDL_FRect describing the input box position and size.
 * @param style Pointer to an InputBoxStyle instance (ownership per implementation).
 * @param parent Optional user data / owner pointer.
 * @return Pointer to a newly allocated InputBox, or NULL on allocation failure.
 */
InputBox* InputBox_new(App* app, SDL_FRect rect, InputBoxStyle* style, void* parent);

/**
 * @brief Destroy an InputBox and free associated resources.
 *
 * Stops any running timers, unregisters input handlers and frees memory.
 * Safe to call with NULL.
 *
 * @param input_box Pointer to the InputBox to destroy.
 */
void InputBox_destroy(InputBox* input_box);

/**
 * @brief Render the input box using the provided SDL_Renderer.
 *
 * Draws background, border and the current text (masked if password_mode).
 *
 * @param input_box Pointer to the InputBox to render.
 * @param renderer SDL_Renderer used for drawing.
 */
void InputBox_render(InputBox* input_box, SDL_Renderer* renderer);

/**
 * @brief Update per-frame logic for the input box.
 *
 * Typically handles focus acquisition and cursor blinking.
 *
 * @param input_box Pointer to the InputBox to update.
 */
void InputBox_update(InputBox* input_box);

/**
 * @brief Set the parent (owner) pointer for the input box.
 *
 * @param input_box Pointer to the InputBox.
 * @param parent User-defined parent pointer.
 */
void InputBox_setParent(InputBox* input_box, void* parent);

/**
 * @brief Give logical focus to the input box and register event handlers.
 *
 * Starts the cursor blink timer and registers text/keyboard/mouse handlers.
 *
 * @param input_box Pointer to the InputBox to focus.
 */
void InputBox_focus(InputBox* input_box);

/**
 * @brief Remove focus from the input box and unregister event handlers.
 *
 * Stops text input if the box was selected.
 *
 * @param input_box Pointer to the InputBox to unfocus.
 */
void InputBox_unFocus(InputBox* input_box);

/**
 * @brief Replace the current text content of the input box.
 *
 * Updates the internal buffer and resets the cursor to the end of the string.
 *
 * @param input_box Pointer to the InputBox.
 * @param str Null-terminated C string to set (copied).
 */
void InputBox_setString(InputBox* input_box, const char* str);

/**
 * @brief Format and set the input box string using printf-style formatting.
 *
 * @param input_box Pointer to the InputBox.
 * @param format printf-style format string followed by arguments.
 */
void InputBox_setStringf(InputBox* input_box, const char* format, ...);

/**
 * @brief Retrieve the raw stored string of the input box.
 *
 * @param input_box Pointer to the InputBox.
 * @return Pointer to the internal C string, or NULL if input_box is NULL.
 */
char* InputBox_getString(InputBox* input_box);

/**
 * @brief Return the formatted (possibly masked) string used for display.
 *
 * If password_mode is enabled, this returns a masked copy; otherwise returns
 * the internal string pointer.
 *
 * @param input_box Pointer to the InputBox.
 * @return Pointer to either the internal string or a newly allocated masked string.
 */
char* InputBox_getFormattedString(InputBox* input_box);

/**
 * @brief Enable or disable password masking mode.
 *
 * When enabled, displayed characters are replaced by a mask character.
 *
 * @param input_box Pointer to the InputBox.
 * @param password_mode true to enable masking, false to disable.
 */
void InputBox_setPasswordMode(InputBox* input_box, bool password_mode);
