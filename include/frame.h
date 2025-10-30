/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @brief Function pointer type for rendering a frame.
 *
 * Used to regroup all frames that have different structures into one structure.
 * Used in main.c and app.c to call render, update, focus and unfocus functions of different frames.
 */
struct Frame {
    /**
     * @brief Pointer to the element associated with the frame.
     *
     * This element can be of any type but will be given to the function pointers when called.
     */
    void* element;

    /**
     * @brief The title of the frame.
     *
     * Used for log when frame changes. Can be NULL if not set.
     */
    char* title;

    /**
     * @brief Function pointer for rendering the frame.
     *
     * Will be called with the element and the app SDL_Renderer as arguments.
     */
    FrameRenderFunc func_render;

    /**
     * @brief Function pointer for updating the frame.
     *
     * Will be called with the element as argument.
     */
    FrameUpdateFunc func_update;

    /**
     * @brief Function pointer for focusing the frame.
     *
     * Will be called with the element as argument.
     */
    FrameFocusFunc func_focus;

    /**
     * @brief Function pointer for unfocusing the frame.
     *
     * Will be called with the element as argument.
     */
    FrameFocusFunc func_unfocus;

    /**
     * @brief Function pointer for destroying the frame.
     *
     * Will be called with the element as argument.
     */
    DestroyFunc func_destroy;
};

/**
 * @brief Creates a new Frame instance.
 *
 * Allocates memory for a Frame and initializes its members with the provided arguments.
 * Will not copy the element, just store the pointer.
 * Will not automatically add the frame to the app stack, that must be done manually.
 *
 * @param element - Pointer to the element associated with the frame.
 * @param func_render - Function pointer for rendering the frame.
 * @param func_update - Function pointer for updating the frame.
 * @param func_focus - Function pointer for focusing the frame.
 * @param func_unfocus - Function pointer for unfocusing the frame.
 * @param func_destroy - Function pointer for destroying the frame.
 *
 * @return - Pointer to the newly created Frame instance.
 */
Frame* Frame_new(void* element, FrameRenderFunc func_render,FrameUpdateFunc func_update, FrameFocusFunc func_focus, FrameFocusFunc func_unfocus, DestroyFunc func_destroy);

/**
 * @brief Sets the title of the frame.
 *
 * If a title was already set, it will be freed.
 *
 * @param frame - Pointer to the Frame instance.
 * @param title - Title string to set. Will be copied.
 */
void Frame_setTitle(Frame* frame, const char* title);

/**
 * @brief Destroys a Frame instance.
 *
 * Calls the destroy function pointer on the element if set,
 * frees the title if set, and frees the Frame itself.
 *
 * @param frame - Pointer to the Frame instance to destroy.
 */
void Frame_destroy(Frame* frame);

/** @brief Renders the frame using its render function pointer.
 *
 * Calls the render function pointer with the element and the provided SDL_Renderer.
 *
 * @param frame - Pointer to the Frame instance.
 * @param renderer - Pointer to the SDL_Renderer to use for rendering.
 */
void Frame_render(Frame* frame, SDL_Renderer* renderer);

/**
 * @brief Updates the frame using its update function pointer.
 *
 * Calls the update function pointer with the element.
 *
 * @param frame - Pointer to the Frame instance.
 */
void Frame_update(Frame* frame);