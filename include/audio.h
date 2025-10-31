/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

Mix_Chunk* Audio_createNoise(int frequency, int duration_ms);
void Audio_play(Mix_Chunk* chunk, int volume, bool loop);
void Audio_playSweepingNoise(int start_freq, int end_freq, int duration_ms);