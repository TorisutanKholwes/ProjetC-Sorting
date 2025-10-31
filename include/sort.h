/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

int List_defaultCompare(const void* a, const void* b);
void List_sort(List* list, ListSortType sortType, CompareFunc compare_func, SDL_mutex* gm, DelayFunc, MainFrame* mainframe, ColumnGraph* column_graph);
bool List_isSorted(List* list, CompareFunc compare_func);
void List_sortBubble(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortQuick(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortMerge(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortInsertion(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortBitonic(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortBogo(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
void List_sortSelection(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
const char* ListSortType_toString(ListSortType sortType);