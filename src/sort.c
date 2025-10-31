/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */
#include "sort.h"

#include "list.h"
#include "logger.h"

static ListNode* partitionQS(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void quickSortRec(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);


int List_defaultCompare(const void* a, const void* b) {
    return (long)a - (long)b;
}

bool List_isSorted(List* list, CompareFunc compare_func) {
    if (!list || list->size < 2) return true;
    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    ListNode* node = list->head->next;
    while (node->next != list->head) {
        if (compare_func(node->value, node->next->value) > 0) {
            return false;
        }
        node = node->next;
    }
    return true;
}


void List_sortBubble(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;
    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    bool swapped;
    do {
        swapped = false;
        ListNode* node = list->head->next;
        while (node->next != list->head) {
            ListNode* next = node->next;
            void* a = node->value;
            void* b = next->value;
            if (compare_func(a, b) > 0) {
                SDL_LockMutex(gm);
                void* temp = node->value;
                node->value = next->value;
                next->value = temp;
                SDL_UnlockMutex(gm);
                if (delay_func) {
                    delay_func(mainframe, column_graph, a);
                }
                swapped = true;
            }
            node = next;
        }
    } while (swapped);
}

static ListNode* partitionQS(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    void* pivot = high->value;
    ListNode* i = low ? low->prev : NULL;
    for (ListNode* j = low; j != high; j = j->next) {
        if (compare_func(j->value, pivot) <= 0) {
            i = (i == NULL) ? low : i->next;
            SDL_LockMutex(gm);
            void* tmp = i->value;
            i->value = j->value;
            j->value = tmp;
            SDL_UnlockMutex(gm);
            if (delay_func) {
                delay_func(mainframe, column_graph, j->value);
            }
        }
    }
    i = (i == NULL) ? low : i->next;
    SDL_LockMutex(gm);
    void* tmp = i->value;
    i->value = high->value;
    high->value = tmp;
    SDL_UnlockMutex(gm);
    if (delay_func) {
        delay_func(mainframe, column_graph, i->value);
    }
    return i;
}

static void quickSortRec(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!low || !high) return;
    if (low != high && low != high->next) {
        ListNode* p = partitionQS(low, high, compare_func, gm, delay_func, mainframe, column_graph);
        quickSortRec(low, p->prev, compare_func, gm, delay_func, mainframe, column_graph);
        quickSortRec(p->next, high, compare_func, gm, delay_func, mainframe, column_graph);
    }
}

void List_sortQuick(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;

    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    ListNode* first = list->head->next;
    ListNode* last = list->head->prev;

    first->prev = NULL;
    last->next = NULL;

    quickSortRec(first, last, compare_func, gm, delay_func, mainframe, column_graph);

    ListNode* new_first = first;
    while (new_first->prev) {
        new_first = new_first->prev;
    }
    ListNode* new_last = last;
    while (new_last->next) {
        new_last = new_last->next;
    }
    new_first->prev = list->head;
    new_last->next = list->head;
    list->head->next = new_first;
    list->head->prev = new_last;

}

static void merge(List* list, void** temp_values, int left, int mid, int right, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    for (int i = left; i <= right; i++) {
        temp_values[i] = List_get(list, i);
    }

    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (compare_func(temp_values[i], temp_values[j]) <= 0) {
            SDL_LockMutex(gm);
            List_set(list, k, temp_values[i]);
            SDL_UnlockMutex(gm);
            if (delay_func) delay_func(mainframe, column_graph, temp_values[i]);
            i++;
        } else {
            SDL_LockMutex(gm);
            List_set(list, k, temp_values[j]);
            SDL_UnlockMutex(gm);
            if (delay_func) delay_func(mainframe, column_graph, temp_values[j]);
            j++;
        }
        k++;
    }

    while (i <= mid) {
        SDL_LockMutex(gm);
        List_set(list, k, temp_values[i]);
        SDL_UnlockMutex(gm);
        if (delay_func) delay_func(mainframe, column_graph, temp_values[i]);
        i++;
        k++;
    }

    while (j <= right) {
        SDL_LockMutex(gm);
        List_set(list, k, temp_values[j]);
        SDL_UnlockMutex(gm);
        if (delay_func) delay_func(mainframe, column_graph, temp_values[j]);
        j++;
        k++;
    }
}

void mergeSortRec(List* list, void** temp_values, int left, int right, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortRec(list, temp_values, left, mid, compare_func, gm, delay_func, mainframe, column_graph);
        mergeSortRec(list, temp_values, mid + 1, right, compare_func, gm, delay_func, mainframe, column_graph);
        merge(list, temp_values, left, mid, right, compare_func, gm, delay_func, mainframe, column_graph);
    }
}

void List_sortMerge(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;
    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    void** temp_values = malloc(list->size * sizeof(void*));
    if (!temp_values) {
        error("Failed to allocate memory for merge sort temporary array");
        return;
    }

    mergeSortRec(list, temp_values, 0, list->size - 1, compare_func, gm, delay_func, mainframe, column_graph);

    free(temp_values);
}

void List_sortInsertion(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;

    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    for (size_t i = 1; i < list->size; i++) {
        void* key = List_get(list, i);
        int j = i - 1;

        while (j >= 0 && compare_func(List_get(list, j), key) > 0) {
            SDL_LockMutex(gm);
            void* value_to_move = List_get(list, j);
            List_set(list, j + 1, value_to_move);
            SDL_UnlockMutex(gm);
            if (delay_func) {
                delay_func(mainframe, column_graph, value_to_move);
            }
            j = j - 1;
        }
        SDL_LockMutex(gm);
        List_set(list, j + 1, key);
        SDL_UnlockMutex(gm);
        if (delay_func) {
            delay_func(mainframe, column_graph, key);
        }
    }
}

static void bitonicMerge(List* list, int low, int count, int direction, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (count > 1) {
        int k = count / 2;
        for (int i = low; i < low + k; i++) {
            void* val1 = List_get(list, i);
            void* val2 = List_get(list, i + k);
            if ((direction == 1 && compare_func(val1, val2) > 0) || (direction == 0 && compare_func(val1, val2) < 0)) {
                SDL_LockMutex(gm);
                List_swap(list, i, i + k);
                SDL_UnlockMutex(gm);
                if (delay_func) {
                    delay_func(mainframe, column_graph, val1);
                }
            }
        }
        bitonicMerge(list, low, k, direction, compare_func, gm, delay_func, mainframe, column_graph);
        bitonicMerge(list, low + k, k, direction, compare_func, gm, delay_func, mainframe, column_graph);
    }
}

static void bitonicSortRec(List* list, int low, int count, int direction, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (count > 1) {
        int k = count / 2;
        bitonicSortRec(list, low, k, 1, compare_func, gm, delay_func, mainframe, column_graph);
        bitonicSortRec(list, low + k, k, 0, compare_func, gm, delay_func, mainframe, column_graph);
        bitonicMerge(list, low, count, direction, compare_func, gm, delay_func, mainframe, column_graph);
    }
}

void List_sortBitonic(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;
    if ((list->size & (list->size - 1)) != 0) {
        log_message(LOG_LEVEL_ERROR, "Bitonic sort requires the list size to be a power of 2.");
        return;
    }
    if (!compare_func) {
        compare_func = List_defaultCompare;
    }
    bitonicSortRec(list, 0, list->size, 1, compare_func, gm, delay_func, mainframe, column_graph);
}

void List_sortBogo(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (!list || list->size < 2) return;

    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    while (!List_isSorted(list, compare_func)) {
        SDL_LockMutex(gm);
        List_shuffle(list);
        SDL_UnlockMutex(gm);
        if (delay_func) {
            delay_func(mainframe, column_graph, NULL);
        }
    }
}

void List_sort(List* list, ListSortType sortType, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func , MainFrame* mainframe, ColumnGraph* column_graph) {
    switch (sortType) {
        case LIST_SORT_TYPE_BUBBLE:
            List_sortBubble(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        case LIST_SORT_TYPE_QUICK:
            List_sortQuick(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        case LIST_SORT_TYPE_MERGE:
            List_sortMerge(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        case LIST_SORT_TYPE_INSERTION:
            List_sortInsertion(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        case LIST_SORT_TYPE_BITONIC:
            List_sortBitonic(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        case LIST_SORT_TYPE_BOGO:
            List_sortBogo(list, compare_func, gm, delay_func, mainframe, column_graph);
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Unknown ListSortType: %d", sortType);
            break;
    }
}

const char* ListSortType_toString(ListSortType sortType) {
    switch (sortType) {
        case LIST_SORT_TYPE_BUBBLE:
            return "Bubble Sort";
        case LIST_SORT_TYPE_QUICK:
            return "Quick Sort";
        case LIST_SORT_TYPE_MERGE:
            return "Merge Sort";
        case LIST_SORT_TYPE_INSERTION:
            return "Insertion Sort";
        case LIST_SORT_TYPE_BITONIC:
            return "Bitonic Sort";
        case LIST_SORT_TYPE_BOGO:
            return "Bogo Sort";
        default:
            return "Unknown Sort Type";
    }
}
