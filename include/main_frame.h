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
    ColumnGraphStyle graph_style;
    Container* popup;
    Container* graph_info;

    Element* temp_element;
    Timer* timer;

    int graph_count;
    int bar_count;
    int selected_graph_index;
    bool all_selected;

    bool showSettings;
    bool box_animating;

    float box_anim_progress;
    float box_start_x;
    float box_target_x;

    float settings_width;

    bool hovered_help;

    bool func_run;

    SDL_mutex* ui_mutex;
    SDL_mutex** graph_mutexes;
};

struct SortThreadArg {
    MainFrame* self;
    int graph_index;
};

MainFrame* MainFrame_new(App* app);
void MainFrame_destroy(MainFrame* self);
void MainFrame_render(SDL_Renderer* renderer, MainFrame* self);
void MainFrame_update(MainFrame* self);
void MainFrame_focus(MainFrame* self);
void MainFrame_unfocus(MainFrame* self);

Frame* MainFrame_getFrame(MainFrame* self);