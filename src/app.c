/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "app.h"

#include "frame.h"
#include "logger.h"
#include "utils.h"
#include "input.h"
#include "list.h"
#include "resource_manager.h"
#include "style.h"

App* App_create(SDL_Window* window, SDL_Renderer* renderer) {
    App* app = calloc(1, sizeof(App));
    if (!app) {
        error("Failed to allocate memory for App");
        return NULL;
    }
    app->window = window;
    app->renderer = renderer;
    app->stack = List_create();
    if (!app->stack) {
        error("Failed to create stack list for App");
        safe_free((void**)&app);
        return NULL;
    }
    app->input = Input_create();
    if (!app->input) {
        error("Failed to create Input for App");
        List_destroy(app->stack);
        safe_free((void**)&app);
        return NULL;
    }
    app->manager = ResourceManager_create(renderer);
    if (!app->manager) {
        error("Failed to create ResourceManager for App");
        Input_destroy(app->input);
        List_destroy(app->stack);
        safe_free((void**)&app);
        return NULL;
    }
    app->running = true;
    return app;
}

void App_destroy(App* app) {
    if (!app) return;
    Input_destroy(app->input);
    List_destroy(app->stack);
    Theme_destroy(app->theme);
    safe_free((void**)&app);
}

void App_quit(const App *app) {
    Mix_Quit();
    TTF_Quit();
    if (app->window)
        SDL_DestroyWindow(app->window);
    if (app->renderer)
        SDL_DestroyRenderer(app->renderer);
    SDL_Quit();
}

static void App_logFrameChange(Frame* from, Frame* to) {
    const char* from_title = from ? from->title ? from->title : "Unknow" : "None";
    const char* to_title = to ? to->title ? to->title : "Unknow" : "None";
    log_message(LOG_LEVEL_DEBUG, "Frame changed from '%s' to '%s'", from_title, to_title);
}

void App_addFrame(App* app, Frame* frame) {
    Frame* curr = NULL;
    if (List_size(app->stack) > 0) {
        curr = App_getCurrentFrame(app);
        if (curr && curr->func_unfocus) {
            curr->func_unfocus(curr->element);
        }
    }
    app->frameChanged = true;
    List_push(app->stack, frame);
    if (frame->func_focus) {
        frame->func_focus(frame->element);
    }
    App_logFrameChange(curr, frame);
}

void App_frameBack(App* app) {
    Frame* frame = List_popLast(app->stack);
    if (frame && frame->func_unfocus) {
        frame->func_unfocus(frame->element);
    }
    app->frameChanged = true;
    Frame* curr = App_getCurrentFrame(app);
    if (curr && curr->func_focus) {
        curr->func_focus(curr->element);
    }
    App_logFrameChange(frame, curr);
    Frame_destroy(frame);
}

Frame* App_getCurrentFrame(const App* app) {
    return List_getLast(app->stack);
}

void App_getCurrentSize(const App* app, int* w, int* h) {
    SDL_GetWindowSize(app->window, w, h);
}