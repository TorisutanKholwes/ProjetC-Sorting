/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */
#include "random.h"

#include "logger.h"
#include "utils.h"

PRNG* PRNG_init(Uint32 seed) {
    PRNG* p = malloc(sizeof(PRNG));
    if (!p) {
        error("Failed to allocate memory for PRNG");
        return NULL;
    }
    p->state = seed ? seed : 0xDEADBEEF;
    return p;
}

void PRNG_destroy(PRNG* p) {
    if (!p) return;
    safe_free((void**)&p);
}

Uint32 PRNG_next(PRNG* p) {
    p->state ^= p->state << 13;
    p->state ^= p->state >> 17;
    p->state ^= p->state << 5;
    return p->state;
}