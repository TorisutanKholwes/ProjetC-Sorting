/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

struct PRNG {
    Uint32 state;
};

PRNG* PRNG_init(Uint32 seed);
void PRNG_destroy(PRNG* p);
Uint32 PRNG_next(PRNG* p);