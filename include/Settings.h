/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

// Define application settings
#define APP_NAME "Sorting Project C"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Sorting Project C Visualizer"
#define FULLSCREEN 0
#define FRAME_RATE 60
#define MAX_GRAPHS 8

#define PRODUCTION 0 // Set to 1 for production build, 0 for development

#ifdef _MSC_VER
#  define INLINE inline
#else
#  define INLINE static inline
#endif


// Include standard libraries
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#define _USE_MATH_DEFINES
#else
#include <unistd.h>
#include <pthread.h>
#endif

// Include SDL libraries
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "types.h"
