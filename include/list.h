/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

struct ListNode {
    ListNode* prev;
    ListNode* next;
    void* value;
};

struct List {
    ListNode* head;
    size_t size;
};

struct ListIterator {
    ListNode* head;
    ListNode* current;
    int index;
    size_t size;
};

List* List_create();
void List_destroy(List* list);
void List_destroyWitValues(List* list, void (*destroyValueFunc)(void* value));
void List_clear(List* list);
void List_push(List* list, void* value);
void List_remove(List* list, void* value);
#define List_popLast(list) List_pop(list, list->size - 1)
void* List_pop(List* list, size_t index);
size_t List_size(List* list);
bool List_empty(List* list);
bool List_contains(List* list, void* value, bool isString);
void* List_get(List* list, size_t index);
void* List_getLast(List* list);
void* List_getFirst(List* list);
void List_set(List* list, size_t index, void* value);
char* List_toString(List* list, const char* format, void* (*formatValueFunc)(void* value));
void List_swap(List* list, size_t index1, size_t index2);
void List_shuffle(List* list, PRNG* pr);
void* List_max(List* list, CompareFunc compare_func);
void* List_min(List* list, CompareFunc compare_func);

ListIterator* ListIterator_new(List* list);
void ListIterator_destroy(ListIterator* iterator);
bool ListIterator_hasNext(ListIterator* iterator);
void* ListIterator_next(ListIterator* iterator);
int ListIterator_index(ListIterator* iterator);

#define List_push_int(list, value) List_push(list, (void*)(long)value)