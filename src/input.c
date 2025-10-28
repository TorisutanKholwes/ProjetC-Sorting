/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "input.h"

#include "logger.h"
#include "list.h"
#include "map.h"
#include "utils.h"

Input *Input_create() {
    Input *input = calloc(1, sizeof(Input));
    if (!input) {
        error("Failed to allocate memory for Input");
        return NULL;
    }
    input->keysDown = List_create();
    if (!input->keysDown) {
        error("Failed to create keysDown list");
        safe_free((void **) &input);
        return NULL;
    }
    input->eventHandlers = Map_create(false);
    if (!input->eventHandlers) {
        error("Failed to create eventHandlers map");
        List_destroy(input->keysDown);
        safe_free((void **) &input);
        return NULL;
    }
    input->keyEventHandlers = Map_create(false);
    if (!input->keyEventHandlers) {
        error("Failed to create keyEventHandlers map");
        List_destroy(input->keysDown);
        Map_destroy(input->eventHandlers);
        safe_free((void **) &input);
        return NULL;
    }
    float x, y;
    SDL_GetMouseState(&x, &y);
    input->mousePos = Position_new(x, y);
    input->lastPressed = SDL_SCANCODE_UNKNOWN;
    return input;
}

void Input_destroy(Input *input) {
    if (!input) return;

    List_destroy(input->keysDown);

    if (input->eventHandlers) {
        MapIterator *it = MapIterator_new(input->eventHandlers);
        while (MapIterator_hasNext(it)) {
            MapIterator_next(it);
            List *handlers = (List *) MapIterator_value(it);
            if (handlers) {
                // Destroy all EventHandlers in the list
                ListIterator *lit = ListIterator_new(handlers);
                while (ListIterator_hasNext(lit)) {
                    EventHandler *handler = ListIterator_next(lit);
                    safe_free((void **) &handler);
                }
                ListIterator_destroy(lit);
                List_destroy(handlers);
            }
        }
        MapIterator_destroy(it);
        Map_destroy(input->eventHandlers);
    }

    if (input->keyEventHandlers) {
        MapIterator *it = MapIterator_new(input->keyEventHandlers);
        while (MapIterator_hasNext(it)) {
            MapIterator_next(it);
            List *handlers = (List *) MapIterator_value(it);
            if (handlers) {
                ListIterator *lit = ListIterator_new(handlers);
                while (ListIterator_hasNext(lit)) {
                    EventHandler *handler = ListIterator_next(lit);
                    safe_free((void **) &handler);
                }
                ListIterator_destroy(lit);
                List_destroy(handlers);
            }
        }
        MapIterator_destroy(it);
        Map_destroy(input->keyEventHandlers);
    }

    Position_destroy(input->mousePos);
    safe_free((void **) &input);
}

void Input_update(Input *input) {
    SDL_Event evt;
    SDL_Scancode code;
    while (SDL_PollEvent(&evt)) {
        if (input->eventHandlers && Map_containsKey(input->eventHandlers, (void *) evt.type)) {
            List *handlers = Map_get(input->eventHandlers, (void *) evt.type);
            ListIterator *it = ListIterator_new(handlers);
            while (ListIterator_hasNext(it)) {
                if (it->size != List_size(handlers)) {
                    break;
                }
                const EventHandler *handler = ListIterator_next(it);
                if (handler && handler->func) {
                    handler->func(input, &evt, handler->data);
                }
            }
            ListIterator_destroy(it);
        }
        switch (evt.type) {
            case SDL_EVENT_QUIT:
                input->quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (input->keyEventHandlers && Map_containsKey(input->keyEventHandlers, (void *) evt.key.key)) {
                    List *handlers = Map_get(input->keyEventHandlers, (void *) evt.key.key);
                    ListIterator *it = ListIterator_new(handlers);
                    while (ListIterator_hasNext(it)) {
                        if (it->size != List_size(handlers)) {
                            break;
                        }
                        const EventHandler *handler = ListIterator_next(it);
                        if (handler && handler->func) {
                            handler->func(input, &evt, handler->data);
                        }
                    }
                    ListIterator_destroy(it);
                }
                code = evt.key.key;
                input->lastPressed = code;
                List_push(input->keysDown, (void *) code);
                if (code == SDLK_LSHIFT || code == SDLK_RSHIFT) {
                    input->shift = true;
                } else if (code == SDLK_LCTRL || code == SDLK_RCTRL) {
                    input->ctrl = true;
                } else if (code == SDLK_LALT || code == SDLK_RALT) {
                    input->alt = true;
                }
                if (code == SDLK_ESCAPE) {
                    input->esc = true;
                }
                break;
            case SDL_EVENT_KEY_UP:
                code = evt.key.key;
                List_remove(input->keysDown, (void *) code);
                if (code == SDLK_LSHIFT || code == SDLK_RSHIFT) {
                    input->shift = false;
                } else if (code == SDLK_LCTRL || code == SDLK_RCTRL) {
                    input->ctrl = false;
                } else if (code == SDLK_LALT || code == SDLK_RALT) {
                    input->alt = false;
                }
                if (code == SDLK_ESCAPE) {
                    input->esc = false;
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (evt.button.button == SDL_BUTTON_LEFT) {
                    input->mouse_left = true;
                } else if (evt.button.button == SDL_BUTTON_RIGHT) {
                    input->mouse_right = true;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (input->mousePos) {
                    Position_destroy(input->mousePos);
                }
                input->mousePos = Position_new(evt.motion.x, evt.motion.y);
                break;
            default:
                break;
        }
    }
}

bool Input_keyDown(Input *input, SDL_Scancode key) {
    return List_contains(input->keysDown, (void *) key, false);
}

bool Input_mouseInRect(Input *input, SDL_FRect rect) {
    if (!input) return false;
    Position *mouse = input->mousePos;
    if (!mouse) {
        log_message(LOG_LEVEL_WARN, "Mouse don't have a position set");
        return false;
    }
    return mouse->x >= rect.x &&
           mouse->x < rect.x + rect.w &&
           mouse->y >= rect.y &&
           mouse->y < rect.y + rect.h;
}

void Input_addKeyEventHandler(Input *input, SDL_Scancode key, EventHandlerFunc func, void *data) {
    if (!input || !func) return;
    EventHandler *handler = calloc(1, sizeof(EventHandler));
    if (!handler) {
        error("Failed to allocate memory for EventHandler");
        return;
    }
    handler->func = func;
    handler->data = data;
    if (!Map_containsKey(input->keyEventHandlers, (void *) key)) {
        List *handlers = List_create();
        List_push(handlers, (void *) handler);
        Map_put(input->keyEventHandlers, (void *) key, handlers);
    } else {
        List *handlers = Map_get(input->keyEventHandlers, (void *) key);
        List_push(handlers, (void *) handler);
    }
}

static void Input_destroyEventHandler(void* value) {
    EventHandler* handler = value;
    safe_free((void**)&handler);
}

void Input_removeKeyEventHandler(Input *input, SDL_Scancode key) {
    if (!input) return;
    List_destroyWitValues(Map_get(input->keyEventHandlers, (void*)key), Input_destroyEventHandler);
    Map_remove(input->keyEventHandlers, (void *) key);
}

void Input_removeOneKeyEventHandler(Input *input, SDL_Scancode key, void *data) {
    if (!input || !data) return;

    if (!Map_containsKey(input->keyEventHandlers, (void *) key)) {
        return;
    }

    List *handlers = Map_get(input->keyEventHandlers, (void *) key);
    ListIterator *it = ListIterator_new(handlers);
    while (ListIterator_hasNext(it)) {
        EventHandler *handler = ListIterator_next(it);
        if (handler && handler->data == data) {
            List_remove(handlers, (void *) handler);
            safe_free((void**)&handler);
            break;
        }
    }
    ListIterator_destroy(it);
    if (List_size(handlers) == 0) {
        Map_remove(input->keyEventHandlers, (void *) key);
        List_destroy(handlers);
    }
}

void Input_clearKeyEventHandlers(Input *input) {
    if (!input) return;
    Map_clear(input->keyEventHandlers);
}

void Input_addEventHandler(Input *input, Uint32 eventType, EventHandlerFunc func, void *data) {
    if (!input || !func) return;
    EventHandler *handler = calloc(1, sizeof(EventHandler));
    if (!handler) {
        error("Failed to allocate memory for EventHandler");
        return;
    }
    handler->func = func;
    handler->data = data;
    if (!Map_containsKey(input->eventHandlers, (void *) eventType)) {
        List *handlers = List_create();
        List_push(handlers, (void *) handler);
        Map_put(input->eventHandlers, (void *) eventType, handlers);
    } else {
        List *handlers = Map_get(input->eventHandlers, (void *) eventType);
        List_push(handlers, (void *) handler);
    }
}

void Input_removeEventHandler(Input *input, Uint32 eventType) {
    if (!input) return;
    List_destroyWitValues(Map_get(input->eventHandlers, (void*)eventType), Input_destroyEventHandler);
    Map_remove(input->eventHandlers, (void *) eventType);
}

void Input_removeOneEventHandler(Input *input, Uint32 eventType, void *data) {
    if (!input || !data) return;

    if (!Map_containsKey(input->eventHandlers, (void *) eventType)) {
        return;
    }

    List *handlers = Map_get(input->eventHandlers, (void *) eventType);
    ListIterator *it = ListIterator_new(handlers);
    while (ListIterator_hasNext(it)) {
        EventHandler *handler = ListIterator_next(it);
        if (handler && handler->data == data) {
            List_remove(handlers, (void *) handler);
            safe_free((void**)&handler);
            break;
        }
    }
    ListIterator_destroy(it);
    if (List_size(handlers) == 0) {
        Map_remove(input->eventHandlers, (void *) eventType);
        List_destroy(handlers);
    }
}


void Input_clearEventHandlers(Input *input) {
    if (!input) return;
    Map_clear(input->eventHandlers);
}

void Input_getMousePosition(Input* input, float* x, float* y) {
    if (!input) return;
    if (!input->mousePos) return;
    if (x) *x = input->mousePos->x;
    if (y) *y = input->mousePos->y;
}