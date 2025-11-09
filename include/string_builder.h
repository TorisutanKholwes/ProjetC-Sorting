/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */

#pragma once

#include "Settings.h"

#define DEFAULT_CAPACITY 64

/**
 * @struct StringBuilder
 * @brief Mutable string accumulator with automatic resizing.
 *
 * - data: NUL-terminated buffer containing the current contents.
 * - length: current number of bytes used (excluding the terminating NUL).
 * - capacity: total allocated buffer size (including space for the NUL).
 */
struct StringBuilder {
    char* data;
    size_t length;
    size_t capacity;
};

/**
 * @brief Create a new StringBuilder.
 *
 * If init_capacity is zero the DEFAULT_CAPACITY is used.
 *
 * @param init_capacity Initial capacity in bytes.
 * @return Pointer to a newly allocated StringBuilder or NULL on allocation failure.
 */
StringBuilder* StringBuilder_create(size_t init_capacity);

/**
 * @brief Destroy a StringBuilder and free its memory.
 *
 * Safe to call with a NULL pointer.
 *
 * @param builder Builder to destroy.
 */
void StringBuilder_destroy(StringBuilder* builder);

/**
 * @brief Append a NUL-terminated C string to the builder.
 *
 * @param builder Target builder.
 * @param str NUL-terminated string to append.
 */
void StringBuilder_append(StringBuilder* builder, const char* str);

/**
 * @brief Append a single character to the builder.
 *
 * @param builder Target builder.
 * @param c Character to append.
 */
void StringBuilder_append_char(StringBuilder* builder, char c);

/**
 * @brief Append an integer (decimal) to the builder.
 *
 * @param builder Target builder.
 * @param i Integer value to append.
 */
void StringBuilder_append_int(StringBuilder* builder, int i);

/**
 * @brief Append formatted text using printf-style format.
 *
 * The function handles formatting into a temporary buffer or allocates
 * a dynamic buffer if the formatted result does not fit the stack buffer.
 *
 * @param builder Target builder.
 * @param format printf-style format string.
 * @param ... Format arguments.
 */
void StringBuilder_append_format(StringBuilder* builder, const char* format, ...);

/**
 * @brief Build a NUL-terminated copy of the current contents.
 *
 * The returned pointer must be freed by the caller with free().
 *
 * @param builder Source builder.
 * @return Newly allocated NUL-terminated string or NULL on allocation failure.
 */
char* StringBuilder_build(StringBuilder* builder);

/**
 * @brief Clear the builder contents without freeing the buffer.
 *
 * Resets length to zero and sets the first byte to the terminating NUL.
 *
 * @param builder Target builder.
 */
void StringBuilder_clear(StringBuilder* builder);

/**
 * @brief Get the current length of the builder contents.
 *
 * @param builder Target builder.
 * @return Number of bytes currently stored (excluding terminating NUL).
 */
size_t StringBuilder_length(StringBuilder* builder);

/**
 * @brief Get the current buffer capacity.
 *
 * @param builder Target builder.
 * @return Allocated buffer size in bytes (including terminating NUL).
 */
size_t StringBuilder_capacity(StringBuilder* builder);
