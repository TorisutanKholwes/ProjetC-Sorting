/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "list.h"
#include "logger.h"
#include "utils.h"
#include "string_builder.h"

static void List_sortBubble(List* list, CompareFunc compare_func);
static void List_sortQuick(List* list, CompareFunc compare_func);
static void List_sortMerge(List* list, CompareFunc compare_func);
static int List_defaultCompare(const void* a, const void* b);

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

static void List_sortBubble(List* list, CompareFunc compare_func) {
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
                void* temp = node->value;
                node->value = next->value;
                next->value = temp;
                swapped = true;
            }
            node = next;
        }
    } while (swapped);
}

static ListNode* partitionQS(ListNode* low, ListNode* high, CompareFunc compare_func) {
    void* pivot = high->value;
    ListNode* i = low ? low->prev : NULL;
    for (ListNode* j = low; j != high; j = j->next) {
        if (compare_func(j->value, pivot) <= 0) {
            i = (i == NULL) ? low : i->next;
            void* tmp = i->value;
            i->value = j->value;
            j->value = tmp;
        }
    }
    i = (i == NULL) ? low : i->next;
    void* tmp = i->value;
    i->value = high->value;
    high->value = tmp;
    return i;
}

static void quickSortRec(ListNode* low, ListNode* high, CompareFunc compare_func) {
    if (!low || !high) return;
    if (low != high && low != high->next) {
        ListNode* p = partitionQS(low, high, compare_func);
        quickSortRec(low, p->prev, compare_func);
        quickSortRec(p->next, high, compare_func);
    }
}

static void List_sortQuick(List* list, CompareFunc compare_func) {
    if (!list || list->size < 2) return;

    if (!compare_func) {
        compare_func = List_defaultCompare;
    }

    ListNode* first = list->head->next;
    ListNode* last = list->head->prev;

    first->prev = NULL;
    last->next = NULL;

    quickSortRec(first, last, compare_func);

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

static ListNode* splitMiddle(ListNode* head) {
    if (!head || !head->next) return NULL;
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode* mid = slow->next;
    slow->next = NULL;
    if (mid) mid->prev = NULL;
    return mid;
}

static ListNode* mergeTwoSorted(ListNode* a, ListNode* b, CompareFunc compare_func) {
    if (!a) return b;
    if (!b) return a;
    ListNode* head = NULL;
    ListNode* tail = NULL;

    while (a && b) {
        ListNode* take = NULL;
        if (compare_func(a->value, b->value) <= 0) {
            take = a;
            a = a->next;
        } else {
            take = b;
            b = b->next;
        }
        take->next = NULL;
        take->prev = tail;
        if (tail) tail->next = take;
        tail = take;
        if (!head) head = take;
    }
    ListNode* rem = a ? a : b;
    while (rem) {
        ListNode* next = rem->next;
        rem->next = NULL;
        rem->prev = tail;
        if (tail) tail->next = rem;
        tail = rem;
        if (!head) head = rem;
        rem = next;
    }
    return head;
}

static ListNode* mergeSortRec(ListNode* head, CompareFunc compare_func) {
    if (!head || !head->next) return head;
    ListNode* mid = splitMiddle(head);
    ListNode* left = mergeSortRec(head, compare_func);
    ListNode* right = mergeSortRec(mid, compare_func);
    return mergeTwoSorted(left, right, compare_func);
}

static void List_sortMerge(List* list, CompareFunc compare_func) {
    if (!list || list->size < 2) return;

    ListNode* first = list->head->next;
    ListNode* last = list->head->prev;
    first->prev = NULL;
    last->next = NULL;

    ListNode* new_first = mergeSortRec(first, compare_func);

    ListNode* new_last = new_first;
    while (new_last->next) {
        new_last = new_last->next;
    }

    list->head->next = new_first;
    list->head->prev = new_last;
    new_first->prev = list->head;
    new_last->next = list->head;
}

void List_sort(List* list, ListSortType sortType, CompareFunc compare_func) {
    switch (sortType) {
        case LIST_SORT_TYPE_BUBBLE:
            List_sortBubble(list, compare_func);
            break;
        case LIST_SORT_TYPE_QUICK:
            List_sortQuick(list, compare_func);
            break;
        case LIST_SORT_TYPE_MERGE:
            List_sortMerge(list, compare_func);
            break;
        default:
            log_message(LOG_LEVEL_WARN, "Unknown ListSortType: %d", sortType);
            break;
    }
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
