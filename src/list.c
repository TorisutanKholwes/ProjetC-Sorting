/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "list.h"
#include "logger.h"
#include "utils.h"
#include "string_builder.h"
#include "column_graph.h"

static void List_sortBubble(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void List_sortQuick(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void List_sortMerge(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void List_sortInsertion(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void List_sortBitonic(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static int List_defaultCompare(const void* a, const void* b);
static ListNode* partitionQS(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);
static void quickSortRec(ListNode* low, ListNode* high, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph);


List *List_create() {
    List *list = calloc(1, sizeof(List));
    if (!list) {
        error("Failed to allocate memory for List");
        return NULL;
    }
    list->head = calloc(1, sizeof(ListNode));
    if (!list->head) {
        error("Failed to allocate memory for ListNode");
        safe_free((void **) &list);
        return NULL;
    }
    list->head->prev = list->head;
    list->head->next = list->head;
    list->size = 0;
    return list;
}

void List_destroy(List *list) {
    if (!list) return;
    List_clear(list);
    safe_free((void **) &list->head);
    safe_free((void **) &list);
}

void List_destroyWitValues(List* list, DestroyFunc destroy_func) {
    ListNode *node = list->head->next;
    while (node != list->head) {
        if (destroy_func) {
            destroy_func(node->value);
        }
        node = node->next;
    }
    List_destroy(list);
}

void List_clear(List *list) {
    ListNode *node = list->head->next;
    while (node != list->head) {
        ListNode *next = node->next;
        if (node == list->head && next == list->head) break;
        safe_free((void **) &node);
        node = next;
    }
    list->size = 0;
    list->head->prev = list->head;
    list->head->next = list->head;
}

void List_push(List *list, void *value) {
    ListNode *node = calloc(1, sizeof(ListNode));
    if (!node) {
        error("Failed to allocate memory for ListNode");
        return;
    }
    ListNode *last = list->head->prev;
    list->head->prev = node;
    node->value = value;
    node->prev = last;
    node->next = list->head;
    last->next = node;
    list->size++;
}

void List_remove(List *list, void *value) {
    ListNode *node = list->head->next;
    while (node != list->head) {
        if (node->value == value) {
            ListNode *prev = node->prev;
            ListNode *next = node->next;
            prev->next = next;
            next->prev = prev;
            safe_free((void **) &node);
            list->size--;
            return;
        }
        node = node->next;
    }
}

void *List_pop(List *list, size_t index) {
    if (index >= list->size || list->size == 0) {
        error("Index out of bounds");
        return NULL;
    }
    ListNode *node = list->head->next;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    void *value = node->value;
    ListNode *prev = node->prev;
    ListNode *next = node->next;
    prev->next = next;
    next->prev = prev;
    safe_free((void **) &node);
    list->size--;
    return value;
}

size_t List_size(List *list) {
    return list->size;
}

bool List_empty(List *list) {
    return list->size == 0;
}

bool List_contains(List *list, void *value, bool isString) {
    ListNode *node = list->head->next;
    while (node != list->head) {
        if (isString) {
            if (String_equals(node->value, value)) {
                return true;
            }
        } else {
            if (node->value == value) {
                return true;
            }
        }
        node = node->next;
    }
    return false;
}

void *List_get(List *list, size_t index) {
    if (index >= list->size) {
        error("Index out of bounds");
        return NULL;
    }
    ListNode *node = list->head->next;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    return node->value;
}

void *List_getLast(List *list) {
    if (list->size == 0) {
        error("List is empty");
        return NULL;
    }
    return list->head->prev->value;
}

void* List_getFirst(List* list) {
    if (list->size == 0) {
        error("List is empty");
        return NULL;
    }
    return list->head->next->value;
}

void List_set(List *list, size_t index, void *value) {
    if (index >= list->size) {
        error("Index out of bounds");
        return;
    }
    ListNode *node = list->head->next;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    node->value = value;
}

char *List_toString(List *list, const char *format, void * (*formatValueFunc)(void *value)) {
    StringBuilder *sb = StringBuilder_create(DEFAULT_CAPACITY);
    if (!sb) return NULL;
    StringBuilder_append(sb, "[");
    ListNode *node = list->head->next;
    while (node != list->head) {
        void *value = node->value;
        if (formatValueFunc) {
            value = formatValueFunc(node->value);
        }
        if (strstr(format, "f")) {
            StringBuilder_append_format(sb, format, *(float *) value);
        } else if (strstr(format, "lf")) {
            StringBuilder_append_format(sb, format, *(double *) value);
        } else {
            StringBuilder_append_format(sb, format, value);
        }
        if (node->next != list->head) {
            StringBuilder_append(sb, ", ");
        }
        node = node->next;
    }
    StringBuilder_append(sb, "]");
    char *result = StringBuilder_build(sb);
    StringBuilder_destroy(sb);
    return result;
}

void List_swap(List* list, size_t index1, size_t index2) {
    if (!list || index1 >= list->size || index2 >= list->size) {
        error("Index out of bounds in List_swap");
        return;
    }
    if (index1 == index2) return;
    ListNode *node1 = list->head->next;
    for (size_t i = 0; i < index1; i++) {
        node1 = node1->next;
    }
    ListNode *node2 = list->head->next;
    for (size_t i = 0; i < index2; i++) {
        node2 = node2->next;
    }
    void *temp = node1->value;
    node1->value = node2->value;
    node2->value = temp;
}

static int List_defaultCompare(const void* a, const void* b) {
    return (long)a - (long)b;
}

static void List_sortBubble(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
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

static void List_sortQuick(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
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

static void mergeSortRec(List* list, void** temp_values, int left, int right, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortRec(list, temp_values, left, mid, compare_func, gm, delay_func, mainframe, column_graph);
        mergeSortRec(list, temp_values, mid + 1, right, compare_func, gm, delay_func, mainframe, column_graph);
        merge(list, temp_values, left, mid, right, compare_func, gm, delay_func, mainframe, column_graph);
    }
}

static void List_sortMerge(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
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

static void List_sortInsertion(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
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

static void List_sortBitonic(List* list, CompareFunc compare_func, SDL_mutex* gm, DelayFunc delay_func, MainFrame* mainframe, ColumnGraph* column_graph) {
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
        default:
            log_message(LOG_LEVEL_WARN, "Unknown ListSortType: %d", sortType);
            break;
    }
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

void List_shuffle(List* list) {
    if (!list || list->size < 2) return;

    size_t n = list->size;
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        List_swap(list, i, j);
    }
}

ListIterator *ListIterator_new(List *list) {
    ListIterator *iterator = calloc(1, sizeof(ListIterator));
    if (!iterator) {
        error("Failed to allocate memory for ListIterator");
        return NULL;
    }
    iterator->head = list->head;
    iterator->current = list->head->next;
    iterator->index = 0;
    iterator->size = list->size;
    return iterator;
}


void ListIterator_destroy(ListIterator *iterator) {
    if (!iterator) return;
    safe_free((void **) &iterator);
}

bool ListIterator_hasNext(ListIterator *iterator) {
    return iterator->current != iterator->head && iterator->size > 0;
}

void *ListIterator_next(ListIterator *iterator) {
    if (!ListIterator_hasNext(iterator)) {
        error("No more elements in ListIterator");
        return NULL;
    }
    if (!iterator->current || !iterator->current->value) {
        //error("Current node is NULL in ListIterator");
        iterator->current = iterator->head;
        return NULL;
    }
    void *value = iterator->current->value;
    iterator->current = iterator->current->next;
    iterator->index++;
    return value;
}

int ListIterator_index(ListIterator *iterator) {
    return iterator->index - 1;
}

char* ListSortType_toString(ListSortType sortType) {
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
        default:
            return "Unknown Sort Type";
    }
}
