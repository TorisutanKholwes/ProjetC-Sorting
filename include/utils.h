/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Position
 * @brief Represents a 2D position with floating point coordinates.
 */
struct Position {
    float x, y;
};

/**
 * @struct Color
 * @brief Represents an RGBA color with integer components in range [0..255].
 */
struct Color {
    int r, g, b, a;
};

/**
 * @def POSITION_NULL
 * @brief Creates a Position used as a sentinel invalid/null value.
 *
 * This macro returns a newly allocated Position with coordinates set to -1.0f.
 */
#define POSITION_NULL Position_new(-1.0f, -1.0f)

/**
 * @brief Allocate and initialize a Position.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pointer to a newly allocated Position (caller must call Position_destroy).
 */
Position* Position_new(float x, float y);

/**
 * @brief Free a Position allocated with Position_new.
 * @param pos Pointer to the Position to free. Safe to pass NULL.
 */
void Position_destroy(Position* pos);

/**
 * @brief Check whether a Position is the null sentinel.
 * @param pos Pointer to the Position to test.
 * @return true if pos is NULL or matches the null sentinel values.
 */
bool Position_isNull(const Position* pos);

/**
 * @brief Compare two Position instances for equality.
 * @param a First position.
 * @param b Second position.
 * @return true if both positions have equal coordinates.
 */
bool Position_equals(const Position* a, const Position* b);

/**
 * @brief Create a Color from RGB components (alpha defaults to 255).
 * @param r Red component [0..255].
 * @param g Green component [0..255].
 * @param b Blue component [0..255].
 * @return Newly allocated Color (caller must call Color_destroy).
 */
Color* Color_rgb(int r, int g, int b);

/**
 * @brief Create a Color from RGBA components.
 * @param r Red component [0..255].
 * @param g Green component [0..255].
 * @param b Blue component [0..255].
 * @param a Alpha component [0..255].
 * @return Newly allocated Color (caller must call Color_destroy).
 */
Color* Color_rgba(int r, int g, int b, int a);

/**
 * @brief Create a Color from HSV components.
 * @param h Hue in degrees [0..360).
 * @param s Saturation [0..1].
 * @param v Value/Brightness [0..1].
 * @return Newly allocated Color in RGB space (alpha = 255).
 */
Color* Color_hsv(float h, float s, float v);

/**
 * @brief Allocate a deep copy of an existing Color.
 * @param color Color to copy.
 * @return Newly allocated Color (caller must call Color_destroy).
 */
Color* Color_copy(Color* color);

/**
 * @brief Convert a project Color to SDL_Color.
 * @param color Pointer to the Color to convert.
 * @return SDL_Color with equivalent components.
 */
SDL_Color Color_toSDLColor(Color* color);

/**
 * @brief Linearly interpolate between two colors.
 * @param start Start color.
 * @param end End color.
 * @param t Interpolation factor in [0..1].
 * @return Newly allocated interpolated Color (caller must call Color_destroy).
 */
Color* interpolateColor(Color* start, Color* end, float t);

/**
 * @brief Compare two Color instances for equality.
 * @param a First color.
 * @param b Second color.
 * @return true if all RGBA components are equal.
 */
bool Color_equals(const Color* a, const Color* b);

/**
 * @brief Parse a hex color string and return a Color.
 * @param hex Hex string like "#RRGGBB" or "#RRGGBBAA".
 * @return Newly allocated Color on success, NULL on parse error.
 */
Color* Color_fromHex(const char* hex);

/**
 * @brief Free a Color previously allocated.
 * @param color Pointer to the Color to free. Safe to pass NULL.
 */
void Color_destroy(Color* color);

/**
 * @brief Convert a Color to a hex string.
 * @param color Pointer to the Color to convert.
 * @return Pointer to a null-terminated string (caller should free) in the form "#RRGGBBAA" or NULL on error.
 */
const char* Color_toHex(const Color* color);

/**
 * @brief Safely free a pointer and set it to NULL.
 * @param ptr Pointer to the pointer that will be freed and nulled.
 */
void safe_free(void** ptr);

/**
 * @brief Safely free an array of pointers and set each element to NULL.
 * @param arr Pointer to the array of pointers.
 * @param len Number of elements in the array.
 */
void safe_freeArray(void** arr, int len);

/**
 * @brief Global initialization routine for the utility module.
 * @return 0 on success, non-zero on error.
 * @note Typically used to initialize SDL or other subsystems in Settings.h.
 */
int init();

/**
 * @def SDL_RECT_NULL
 * @brief Sentinel SDL_FRect with all fields set to -1.0f representing an invalid rect.
 */
#define SDL_RECT_NULL (SDL_FRect) { -1.0f, -1.0f, -1.0f, -1.0f }

/**
 * @brief Create an SDL_FRect optionally centered around (x,y).
 * @param x X coordinate (center or top-left depending on from_center).
 * @param y Y coordinate.
 * @param w Width.
 * @param h Height.
 * @param from_center If true, (x,y) is treated as the rect center; otherwise top-left.
 * @return SDL_FRect instance.
 */
SDL_FRect SDL_CreateRect(float x, float y, float w, float h, bool from_center);

/**
 * @brief Render a stroked rectangle with a specified thickness.
 * @param renderer SDL renderer to draw to.
 * @param rect Rectangle to stroke (floating).
 * @param thickness Stroke thickness in pixels.
 */
void SDL_RenderStroke(SDL_Renderer* renderer, const SDL_FRect* rect, float thickness);

/**
 * @brief Draw a thick line between two points.
 * @param renderer SDL renderer to draw to.
 * @param x1 Starting X coordinate.
 * @param y1 Starting Y coordinate.
 * @param x2 Ending X coordinate.
 * @param y2 Ending Y coordinate.
 * @param thickness Line thickness in pixels.
 */
void SDL_drawThickLine(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, float thickness);

/**
 * @brief Duplicate a C string using heap allocation.
 * @param str Null-terminated source string.
 * @return Newly allocated copy of str (caller must free) or NULL on allocation failure.
 */
char* Strdup(const char* str);

/**
 * @brief Test whether a string is NULL or empty.
 * @param str String to test.
 * @return true if str is NULL or points to an empty string.
 */
bool String_isNullOrEmpty(const char* str);

/**
 * @brief Compare two strings for equality handling NULLs.
 * @param a First string.
 * @param b Second string.
 * @return true if both strings are equal or both NULL.
 */
bool String_equals(const char* a, const char* b);

/**
 * @brief Parse an integer from a string with a default fallback.
 * @param str Input string.
 * @param defaultValue Value to return if parsing fails.
 * @return Parsed integer or defaultValue on failure.
 */
int String_parseInt(const char* str, int defaultValue);

/**
 * @brief Parse a float from a string with a default fallback.
 * @param str Input string.
 * @param defaultValue Value to return if parsing fails.
 * @return Parsed float or defaultValue on failure.
 */
float String_parseFloat(const char* str, float defaultValue);

/**
 * @brief Format a string using printf-style formatting into a newly allocated buffer.
 * @param fmt Format string.
 * @param ... Format arguments.
 * @return Newly allocated formatted string (caller must free) or NULL on error.
 */
char* String_format(const char* fmt, ...);

/**
 * @brief Check whether a string represents a numeric value.
 * @param str Input string.
 * @return true if the string contains only numeric characters (and optional sign/decimal).
 */
bool String_isNumeric(const char* str);

/**
 * @brief Return the lexically maximal string from an array.
 * @param arr Array of null-terminated strings.
 * @param len Number of elements in the array.
 * @return Pointer to the maximal string (owned by caller's array) or NULL if len == 0.
 */
char* String_max(const char** arr, int len);

/**
 * @brief Return the lexically minimal string from an array.
 * @param arr Array of null-terminated strings.
 * @param len Number of elements in the array.
 * @return Pointer to the minimal string (owned by caller's array) or NULL if len == 0.
 */
char* String_min(const char** arr, int len);

/**
 * @brief Compare two strings lexically in a safe manner.
 * @param a First string.
 * @param b Second string.
 * @return Negative, zero or positive value as in strcmp semantics.
 */
int String_compare(const char* a, const char* b);

/**
 * @brief Format a millisecond duration into a human-readable time string.
 * @param milliseconds Time duration in milliseconds.
 * @return Newly allocated formatted string (caller must free) like "mm:ss" or "hh:mm:ss".
 */
char* String_formatTime(Uint32 milliseconds);

/**
 * @brief Compute modulo that handles negative dividends in a mathematical sense.
 * @param a Dividend.
 * @param b Divisor (must be non-zero).
 * @return Result in range [0, b-1] if b > 0.
 */
int modulo(int a, int b);

/**
 * @brief Find the maximum value in a long integer array.
 * @param arr Array of long values.
 * @param len Number of elements in arr (must be > 0).
 * @return Maximum value found.
 */
long arrayMax(const long* arr, int len);

/**
 * @brief Find the minimum value in a long integer array.
 * @param arr Array of long values.
 * @param len Number of elements in arr (must be > 0).
 * @return Minimum value found.
 */
long arrayMin(const long* arr, int len);

/**
 * @def UNUSED
 * @brief Mark a variable as intentionally unused to suppress compiler warnings.
 */
#define UNUSED(x) (void)(x)

/**
 * @brief Convert an array of void* where each element points to an int into a newly allocated int array.
 * @param arr Array of void* each pointing to an int.
 * @param len Number of elements.
 * @return Newly allocated int array (caller must free) or NULL on failure.
 */
int* voidToIntArray(void** arr, int len);

/**
 * @brief Convert an array of void* where each element points to a long into a newly allocated long array.
 * @param arr Array of void* each pointing to a long.
 * @param len Number of elements.
 * @return Newly allocated long array (caller must free) or NULL on failure.
 */
long* voidToLongArray(void** arr, int len);

/**
 * @brief Convert a long array into an array of void* where each element points to a newly allocated long.
 * @param arr Input array of long values.
 * @param len Number of elements.
 * @return Newly allocated array of void* (caller must free both the array and each pointed element).
 */
void** longToVoidArray(const long* arr, int len);

/**
 * @brief Convert an int array into an array of void* where each element points to a newly allocated int.
 * @param arr Input array of int values.
 * @param len Number of elements.
 * @return Newly allocated array of void* (caller must free both the array and each pointed element).
 */
void** intToVoidArray(const int* arr, int len);

/**
 * @brief Test whether an integer is a power of two.
 * @param n Integer to test.
 * @return true if n is a positive power of two.
 */
bool is_power_of_two(int n);

/**
 * @brief Render a filled, rotated rectangle with a Color.
 * @param renderer SDL renderer to draw to.
 * @param rect Rectangle to fill (position and size).
 * @param angle Rotation in degrees to apply about the rect center.
 * @param color Color to fill with (may be converted internally to SDL_Color).
 */
void SDL_RenderRotateFillRect(SDL_Renderer* renderer, const SDL_FRect* rect, float angle, Color* color);
