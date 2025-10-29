/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "container.h"

#include "element.h"
#include "geometry.h"
#include "logger.h"
#include "utils.h"
#include "list.h"

Container* Container_new(float x, float y, float width, float height, Color* color, void* parent) {
    Container* container = calloc(1, sizeof(Container));
    if (!container) {
        error("Failed to allocate memory for Container");
        return NULL;
    }
    container->box = Box_new(width, height, 0, Position_new(x, y), color, NULL, false);
    container->children = List_create();
    container->parent = parent;

    return container;
}

void Container_destroy(Container* container) {
    if (!container) return;

    Box_destroy(container->box);
    List_destroyWitValues(container->children, (DestroyFunc)Element_destroy);
    safe_free((void**)&container);
}

void Container_addChild(Container* container, Element* child) {
    if (!container || !child) return;
    List_push(container->children, child);
}

void Container_removeChild(Container* container, Element* child) {
    if (!container || !child) return;
    List_remove(container->children, child);
}

void Container_render(Container* container, SDL_Renderer* renderer) {
    if (!container || !renderer) return;

    Box_render(container->box, renderer);

    Element_renderList(container->children, renderer);
}

void Container_update(Container* container) {
    if (!container) return;

    Element_updateList(container->children);
}

void Container_focus(Container* container) {
    if (!container) return;

    Element_focusList(container->children);
}

void Container_unFocus(Container* container) {
    if (!container) return;

    Element_unfocusList(container->children);
}

void Container_setSize(Container* container, float width, float height) {
    if (!container) return;

    container->box->size = (Size) {width, height};
}