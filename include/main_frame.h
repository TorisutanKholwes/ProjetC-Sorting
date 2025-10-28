/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct MainFrame {
    List* elements;
    App* app;

    ColumnGraph** graph;
    Container* popup;
    int graph_count;

    bool showSettings;
    bool box_animating;

    float box_anim_progress;
    float box_start_x;
    float box_target_x;
};

MainFrame* MainFrame_new(App* app);
void MainFrame_destroy(MainFrame* self);
void MainFrame_render(SDL_Renderer* renderer, MainFrame* self);
void MainFrame_update(MainFrame* self);
void MainFrame_focus(MainFrame* self);
void MainFrame_unfocus(MainFrame* self);

Frame* MainFrame_getFrame(MainFrame* self);