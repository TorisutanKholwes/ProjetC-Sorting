/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @brief A container element.
 *
 * Containers are a simple Box that can hold other elements inside them.
 * Similar to the <div> element in HTML.
 */
struct Container {
    /**
     * @brief Pointer to the Box representing the container's visual appearance
     */
    Box* box;

    /**
     * @brief List of child elements contained within this container
     *
     * The list can't be NULL, but can be empty.
     * All the elements in the list are of type Element*.
     * The positions of the children are NOT relative to the container's position.
     */
    List* children;

    /**
     * @brief Pointer to the parent object of the container
     *
     * Used for hierarchical UI structures. May be NULL if no parent.
     * Can also be used for going back references in event handlers.
     */
    void* parent;
};

/**
 * @brief Create a new Container instance
 *
 * @param x The top-left x coordinate of the container
 * @param y The top-left y coordinate of the container
 * @param width The width of the container
 * @param height The height of the container
 * @param from_center If true, (x, y) represents the center of the container; otherwise, the top-left corner
 * @param color The background color of the container
 * @param parent The parent object of the container
 *
 * @return A pointer to the newly created Container instance
 */
Container* Container_new(float x, float y, float width, float height, bool from_center, Color* color, void* parent);

/**
 * @brief Destroy a Container instance
 *
 * @param container The Container instance to destroy
 */
void Container_destroy(Container* container);

/**
 * @brief Add a child element to the container
 *
 * @param container The Container instance
 * @param child The Element instance to add as a child
 */
void Container_addChild(Container* container, Element* child);

/**
 * @brief Remove a child element from the container
 *
 * The child is not destroyed after being removed.
 * If you want to destroy it, do it manually.
 *
 * @param container The Container instance
 * @param child
 */
void Container_removeChild(Container* container, Element* child);

/**
 * @brief Render the container and its children
 *
 * Usually called every frame when rendering the UI.
 *
 * @param container - The Container instance
 * @param renderer - The SDL_Renderer to use for rendering
 */
void Container_render(Container* container, SDL_Renderer* renderer);

/**
 * @brief Update the container and its children
 *
 * Usually called every frame when updating the UI.
 *
 * @param container - The Container instance
 */
void Container_update(Container* container);

/**
 * @brief Set focus to the container and its children
 *
 * Usually called when the Frame where the container is located gains focus.
 * If the child doesn't have a focusable function, nothing happens.
 *
 * @param container - The Container instance
 */
void Container_focus(Container* container);

/**
 * @brief Remove focus from the container and its children
 *
 * Usually called when the Frame where the container is located loses focus.
 * If the child doesn't have a unfocusable function, nothing happens.
 *
 * @param container - The Container instance
 */
void Container_unFocus(Container* container);

/**
 * @brief Set the size of the container
 *
 * @param container - The Container instance
 * @param width - The new width of the container
 * @param height  - The new height of the container
 */
void Container_setSize(Container* container, float width, float height);

Position* Container_getPosition(Container* container);
Size Container_getSize(Container* container);