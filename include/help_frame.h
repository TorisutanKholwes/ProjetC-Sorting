//
// Created by bapti on 10/29/25.
//

#pragma once

#include "Settings.h"

/**
 * @struct HelpFrame
 * @brief Represents a paginated help view containing UI elements.
 *
 * The HelpFrame contains a list of UI elements that make up the help pages,
 * a reference to the application context, and simple pagination state.
 */
struct HelpFrame {
    /** @brief List of elements composing the help pages (List\* of Element\*). */
    List* elements;

    /** @brief Pointer to the global application context (renderer, input, resources). */
    App *app;

    /** @brief Current visible page index (0-based). */
    int curr_page;

    /** @brief Maximum number of pages (one past the last valid index). */
    int max_page;
};

/**
 * @brief Allocate and initialize a new HelpFrame.
 *
 * Creates a HelpFrame instance and populates initial elements as required by the
 * implementation. The returned instance is owned by the caller and must be
 * destroyed with HelpFrame_destroy().
 *
 * @param app Pointer to the App instance used by the help frame.
 * @return Pointer to a newly allocated HelpFrame, or NULL on failure.
 */
HelpFrame* HelpFrame_new(App* app);

/**
 * @brief Destroy a HelpFrame and free its resources.
 *
 * Destroys contained elements and frees memory associated with the frame.
 * Safe to call with NULL.
 *
 * @param self HelpFrame instance to destroy.
 */
void HelpFrame_destroy(HelpFrame* self);

/**
 * @brief Give focus to the help frame.
 *
 * Typically registers input handlers and enables page navigation.
 *
 * @param self HelpFrame instance to focus.
 */
void HelpFrame_focus(HelpFrame* self);

/**
 * @brief Remove focus from the help frame.
 *
 * Unregisters input handlers and stops reacting to navigation input.
 *
 * @param self HelpFrame instance to unfocus.
 */
void HelpFrame_unfocus(HelpFrame* self);

/**
 * @brief Render the help frame to the provided renderer.
 *
 * Draws the currently visible page and any surrounding UI chrome.
 *
 * @param renderer SDL_Renderer used for drawing.
 * @param self HelpFrame instance to render.
 */
void HelpFrame_render(SDL_Renderer* renderer, HelpFrame* self);

/**
 * @brief Update per-frame logic for the help frame.
 *
 * Handles animations, page transitions and input polling when focused.
 *
 * @param self HelpFrame instance to update.
 */
void HelpFrame_update(HelpFrame* self);

/**
 * @brief Retrieve the underlying Frame element representing the help UI.
 *
 * Returns a pointer to the Frame used by the help view, if any.
 *
 * @param self HelpFrame instance.
 * @return Pointer to Frame or NULL if not available.
 */
Frame* HelpFrame_getFrame(HelpFrame* self);
