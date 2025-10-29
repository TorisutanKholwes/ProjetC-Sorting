/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct App {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Input* input;
    List* stack;
    Theme* theme;
    ResourceManager* manager;

    bool running;
    bool frameChanged;
};

App* App_create(SDL_Window* window, SDL_Renderer* renderer);
void App_destroy(App* app);
void App_quit(const App *app);
void App_addFrame(App* app, Frame* frame);
void App_frameBack(App* app);
Frame* App_getCurrentFrame(const App* app);
void App_getCurrentSize(const App* app, int* w, int* h);