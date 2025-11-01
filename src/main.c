/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "Settings.h"
#include "app.h"
#include "frame.h"
#include "logger.h"
#include "utils.h"
#include "input.h"
#include "list.h"
#include "main_frame.h"
#include "resource_manager.h"
#include "style.h"

#if 1
int main() {
    srand(time(NULL));
    log_message(LOG_LEVEL_INFO, "Starting up app %s", APP_NAME);
    log_message(LOG_LEVEL_DEBUG, "Debug mode is enabled");

    int exitStatus = init();

    if (exitStatus == EXIT_FAILURE) {
        exit(exitStatus);
    };

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

#if !defined(FULLSCREEN) || (defined(FULLSCREEN) && FULLSCREEN == 1)
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, flags);

    if (!window) {
        error("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        error("Unable to create renderer: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 4096) < 0) {
        error("Unable to initialize SDL_mixer: %s", Mix_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    log_message(LOG_LEVEL_INFO, "Successfully initialized SDL, Window and Renderer. Start looping app...");
    App* app = App_create(window, renderer);

    if (!app) {
        exit(EXIT_FAILURE);
    }

    app->theme = Theme_default(app->manager);

    App_addFrame(app, MainFrame_getFrame(MainFrame_new(app)));

    Uint64 frame_delay = 1000 / FRAME_RATE;

    while (app->running) {
        Uint64 frame_start = SDL_GetTicks();

        Input_update(app->input);

        if (app->input->quit) {
            app->running = false;
            break;
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        Color* background = app->theme->background;
        SDL_SetRenderDrawColor(renderer, background->r, background->g, background->b, background->a);
        SDL_RenderClear(renderer);

        Frame* frame = App_getCurrentFrame(app);

        if (!frame) {
            log_message(LOG_LEVEL_WARN, "No current frame to render.");
            continue;
        }

        Frame_update(frame);
        if (app->frameChanged) {
            frame = App_getCurrentFrame(app);
            app->frameChanged = false;
            if (!frame) {
                log_message(LOG_LEVEL_WARN, "No current frame to render after frame change.");
                continue;
            }
        }
        Frame_render(frame, renderer);

        SDL_RenderPresent(app->renderer);

        Uint64 frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time) {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    while (List_size(app->stack) > 0) {
        Frame* frame = List_popLast(app->stack);
        Frame_destroy(frame);
    }

    // Need to be destroyed before App_quit because it uses SDL3 functions
    ResourceManager_destroy(app->manager);

    App_quit(app);
    App_destroy(app);
    log_message(LOG_LEVEL_INFO, "App has been closed.");
    return EXIT_SUCCESS;
}
#endif

#if 0
int main() {
    FILE* file = tempFile("../assets/images/checkmark.svg");
    if (!file) {
        return EXIT_FAILURE;
    }
    remplaceDataInFile(file, "000000", "FF0000");

    // For testing: write the temp file to disk to verify the changes
    FILE* outFile = fopen("modified_checkmark.svg", "wb");
    if (!outFile) {
        error("Failed to open output file for writing modified SVG");
        fclose(file);
        return EXIT_FAILURE;
    }
    char buffer[8192];
    size_t bytes;
    rewind(file);
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes, outFile);
    }
    fclose(outFile);
    fclose(file);

    return EXIT_SUCCESS;
}
#endif