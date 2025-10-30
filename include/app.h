/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct App
 * @brief Principal application structure
 *
 * This structure holds the main components of the application,
 * including the SDL window and renderer, input handling, frame stack,
 * theme management, and resource management.
 */
struct App {
    SDL_Window* window; /**The current SDL window */
    SDL_Renderer* renderer; /**The current SDL renderer */
    Input* input; /** Input handling structure */
    List* stack; /** A stack of frames */
    Theme* theme; /** The style theme of the app */
    ResourceManager* manager; /** The resource manager */

    bool running; /** Application running state */
    bool frameChanged; /** Flag indicating if the frame has changed */
};

/** @brief Create a new App instance
 *
 * @param window The SDL window to be used by the application
 * @param renderer The SDL renderer to be used by the application
 * @return A pointer to the newly created App instance
 */
App* App_create(SDL_Window* window, SDL_Renderer* renderer);

/** @brief Destroy an App instance
 *
 * @param app The App instance to be destroyed
 */
void App_destroy(App* app);

/** @brief Quit the application
 *
 * @param app The App instance to quit
 */
void App_quit(const App *app);

/** @brief Add a new frame to the application and make it the current frame
 *
 * @param app The App instance
 * @param frame The Frame to be added
 */
void App_addFrame(App* app, Frame* frame);

/** @brief Go back to the previous frame in the stack
 *
 * @param app The App instance
 */
void App_frameBack(App* app);

/** @brief Get the current frame of the application
 *
 * @param app The App instance
 * @return The current Frame
 */
Frame* App_getCurrentFrame(const App* app);