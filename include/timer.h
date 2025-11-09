/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Timer
 * @brief Lightweight timer storing start and pause ticks and state flags.
 *
 * Fields:
 * - startTicks: Tick count when the timer was started or resumed.
 * - pausedTicks: Elapsed ticks stored when the timer was paused.
 * - paused: True when the timer is currently paused.
 * - started: True when the timer has been started and not stopped.
 */
struct Timer {
    Uint32 startTicks;
    Uint32 pausedTicks;
    bool paused;
    bool started;
};

/**
 * @brief Allocate and initialize a new Timer.
 *
 * The returned Timer is zero-initialized and must be freed with Timer_destroy().
 *
 * @return Pointer to a newly allocated Timer or NULL on allocation failure.
 */
Timer* Timer_new();

/**
 * @brief Destroy a Timer and free its memory.
 *
 * Safe to call with NULL.
 *
 * @param self Timer instance to destroy.
 */
void Timer_destroy(Timer* self);

/**
 * @brief Start or restart the timer.
 *
 * Sets the started flag and records the current tick count. Clears any paused state.
 *
 * @param self Timer instance to start.
 */
void Timer_start(Timer* self);

/**
 * @brief Stop the timer and reset internal state.
 *
 * After calling this the timer reports zero elapsed ticks until started again.
 *
 * @param self Timer instance to stop.
 */
void Timer_stop(Timer* self);

/**
 * @brief Reset the timer to begin measuring from now.
 *
 * Equivalent to starting the timer and clearing any paused ticks.
 *
 * @param self Timer instance to reset.
 */
void Timer_reset(Timer* self);

/**
 * @brief Pause the timer.
 *
 * When paused the current elapsed ticks are stored and subsequent calls to
 * Timer_getTicks() will return the paused value until the timer is resumed.
 *
 * @param self Timer instance to pause.
 */
void Timer_pause(Timer* self);

/**
 * @brief Resume a previously paused timer.
 *
 * Adjusts internal startTicks so elapsed time continues from the paused value.
 *
 * @param self Timer instance to resume.
 */
void Timer_resume(Timer* self);

/**
 * @brief Get the elapsed ticks measured by the timer.
 *
 * Returns the paused tick count if paused, the difference between current ticks
 * and startTicks if running, or 0 if the timer is not started.
 *
 * @param self Timer instance to query.
 * @return Elapsed ticks in SDL tick units (Uint32).
 */
Uint32 Timer_getTicks(Timer* self);
