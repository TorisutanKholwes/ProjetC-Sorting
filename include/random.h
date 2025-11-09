/*
* Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct PRNG
 * @brief Internal state for the PRNG.
 *
 * The PRNG state is stored as a single 32-bit unsigned integer. The actual
 * algorithm that advances this state is implemented in the corresponding
 * source file.
 */
struct PRNG {
    Uint32 state; /**< Current internal state of the generator. */
};

/**
 * @brief Initialize a new PRNG instance with the provided seed.
 *
 * The returned PRNG instance must be freed with PRNG_destroy when no longer needed.
 *
 * @param seed Initial seed value to initialize the PRNG state.
 * @return Pointer to a newly allocated PRNG instance, or NULL on allocation failure.
 */
PRNG* PRNG_init(Uint32 seed);

/**
 * @brief Destroy a PRNG instance and free associated resources.
 *
 * Safe to call with a NULL pointer.
 *
 * @param p Pointer to the PRNG instance to destroy.
 */
void PRNG_destroy(PRNG* p);

/**
 * @brief Generate the next 32-bit pseudorandom number.
 *
 * Advances the internal state and returns the next pseudorandom Uint32 value.
 *
 * @param p Pointer to an initialized PRNG instance.
 * @return Next pseudorandom Uint32 value.
 */
Uint32 PRNG_next(PRNG* p);
