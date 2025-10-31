/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "list.h"
#include "logger.h"
#include "utils.h"
#include "string_builder.h"
#include "column_graph.h"
#include "sort.h"

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