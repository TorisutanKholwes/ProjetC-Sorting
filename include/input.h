/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

#define MAX_KEY_DOWN 256

/**
 * @struct Input
 * @brief Tracks current input state and registered event handlers.
 *
 * - keysDown: list of currently pressed SDL_Scancode values.
 * - lastPressed: last SDL_Scancode pressed.
 * - keyEventHandlers: map from SDL_Scancode to List of EventHandler.
 * - eventHandlers: map from SDL_Event type (Uint32) to List of EventHandler.
 * - mousePos: current mouse position.
 * - mouse_left / mouse_right: current mouse button states.
 * - shift / ctrl / alt: modifier key states.
 * - esc: whether escape is pressed.
 * - quit: whether a quit event was received.
 */
struct Input {
    List* keysDown;
    SDL_Scancode lastPressed;
    Map* keyEventHandlers;
    Map* eventHandlers;
    Position* mousePos;
    bool mouse_left, mouse_right;
    bool shift, ctrl, alt;
    bool esc;
    bool quit;
};

/**
 * @struct EventHandler
 * @brief Wrapper for an input event callback and its user data.
 *
 * - func: callback to invoke when the associated event occurs.
 * - data: user-provided pointer passed to the callback.
 *
 * The callback type is EventHandlerFunc (defined in Settings.h) and typically
 * has the signature: void (*)(Input* input, SDL_Event* evt, void* data).
 */
struct EventHandler {
    EventHandlerFunc func;
    void* data;
};

/**
 * @brief Allocate and initialize a new Input instance.
 *
 * Initializes internal lists and maps and sets default state values.
 *
 * @return Pointer to newly allocated Input, or NULL on allocation failure.
 */
Input* Input_create();

/**
 * @brief Destroy an Input instance and free all associated resources.
 *
 * Also destroys registered event handler lists and their EventHandler entries.
 *
 * @param input Pointer to Input to destroy. Safe to pass NULL.
 */
void Input_destroy(Input* input);

/**
 * @brief Poll SDL events and update the Input state.
 *
 * Processes SDL events, invokes registered event handlers and updates
 * keyboard/mouse/modifier state accordingly.
 *
 * @param input Pointer to Input instance to update.
 */
void Input_update(Input* input);

/**
 * @brief Check whether a given key is currently held down.
 *
 * @param input Pointer to Input instance.
 * @param key SDL_Scancode to query.
 * @return true if the key is in the keysDown list, false otherwise.
 */
bool Input_keyDown(Input* input, SDL_Scancode key);

/**
 * @brief Check whether the mouse is inside a given rectangle.
 *
 * Uses the stored mouse position to test against the provided SDL_FRect.
 *
 * @param input Pointer to Input instance.
 * @param rect Rectangle to test.
 * @return true if mouse position is within rect, false otherwise.
 */
bool Input_mouseInRect(Input* input, SDL_FRect rect);

/**
 * @brief Register a callback for a specific key scancode press events.
 *
 * The handler will be invoked on SDL_KEYDOWN events matching \p key.
 *
 * @param input Pointer to Input instance.
 * @param key SDL_Scancode to listen for.
 * @param func Callback function to invoke.
 * @param data User data passed to the callback.
 */
void Input_addKeyEventHandler(Input* input, SDL_Scancode key, EventHandlerFunc func, void* data);

/**
 * @brief Remove all registered handlers for a specific key scancode.
 *
 * @param input Pointer to Input instance.
 * @param key SDL_Scancode whose handlers should be removed.
 */
void Input_removeKeyEventHandler(Input* input, SDL_Scancode key);

/**
 * @brief Remove a single key event handler associated with given user data.
 *
 * If multiple handlers share the same data pointer, only the first found is removed.
 *
 * @param input Pointer to Input instance.
 * @param key SDL_Scancode to modify.
 * @param data User data pointer used to identify the handler to remove.
 */
void Input_removeOneKeyEventHandler(Input* input, SDL_Scancode key, void* data);

/**
 * @brief Remove all key event handlers.
 *
 * Clears the internal keyEventHandlers map.
 *
 * @param input Pointer to Input instance.
 */
void Input_clearKeyEventHandlers(Input* input);

/**
 * @brief Register a callback for a specific SDL event type.
 *
 * The handler will be invoked for events whose SDL_Event.type matches \p eventType.
 *
 * @param input Pointer to Input instance.
 * @param eventType SDL event type (Uint32) to listen for.
 * @param func Callback function to invoke.
 * @param data User data passed to the callback.
 */
void Input_addEventHandler(Input* input, Uint32 eventType, EventHandlerFunc func, void* data);

/**
 * @brief Remove all registered handlers for a specific SDL event type.
 *
 * @param input Pointer to Input instance.
 * @param eventType SDL event type to remove handlers for.
 */
void Input_removeEventHandler(Input* input, Uint32 eventType);

/**
 * @brief Remove a single event handler for a given SDL event type identified by user data.
 *
 * @param input Pointer to Input instance.
 * @param eventType SDL event type.
 * @param data User data pointer used to identify the handler to remove.
 */
void Input_removeOneEventHandler(Input* input, Uint32 eventType, void* data);

/**
 * @brief Remove all SDL event handlers.
 *
 * Clears the internal eventHandlers map.
 *
 * @param input Pointer to Input instance.
 */
void Input_clearEventHandlers(Input* input);

/**
 * @brief Retrieve the current mouse position stored in the Input.
 *
 * If x or y is NULL, that coordinate is not written.
 *
 * @param input Pointer to Input instance.
 * @param x Output pointer for x coordinate.
 * @param y Output pointer for y coordinate.
 */
void Input_getMousePosition(Input* input, float* x, float* y);
