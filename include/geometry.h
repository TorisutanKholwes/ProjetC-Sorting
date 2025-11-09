/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

#include "Settings.h"

/**
 * @struct Box
 * @brief Represents an axis-aligned rectangle with optional border and centering.
 *
 * The Box stores its size, border parameters, position and colors. The \p center
 * flag indicates whether \p position is the box center (true) or top-left (false).
 */
struct Box {
    /** @brief Width and height of the box. */
    Size size;
    /** @brief Border thickness in pixels. */
    int border_size;
    /** @brief Position of the box (top-left or center depending on \p center). */
    Position* position;
    /** @brief Fill/background color (may be NULL). */
    Color* background;
    /** @brief Border color (may be NULL). */
    Color* border_color;
    /** @brief If true, \p position is treated as the box center. */
    bool center;
};

/**
 * @brief Create a new Box instance.
 *
 * @param width Box width in pixels.
 * @param height Box height in pixels.
 * @param border_size Border thickness in pixels.
 * @param position Pointer to Position (ownership documented in implementation).
 * @param background Fill color (may be NULL).
 * @param border_color Border color (may be NULL).
 * @param center If true, \p position is the center of the box.
 * @return Pointer to newly allocated Box or NULL on allocation failure.
 */
Box* Box_new(float width, float height, int border_size, Position* position, Color* background, Color* border_color, bool center);

/**
 * @brief Destroy a Box and free its resources.
 *
 * Safe to call with NULL.
 *
 * @param self Box instance to destroy.
 */
void Box_destroy(Box* self);

/**
 * @brief Render the box using the provided SDL_Renderer.
 *
 * Draws the border (if any) and fills the interior with the background color.
 *
 * @param self Box to render.
 * @param renderer SDL_Renderer used for drawing.
 */
void Box_render(Box* self, SDL_Renderer* renderer);

/**
 * @brief Set the border thickness and color for the box.
 *
 * Existing border color may be replaced depending on ownership semantics.
 *
 * @param self Box instance.
 * @param border_size New border thickness.
 * @param border_color New border color pointer.
 */
void Box_setBorder(Box* self, int border_size, Color* border_color);

/**
 * @brief Retrieve the top-left position of the box.
 *
 * If the box is centered, this computes the top-left from the stored center.
 *
 * @param self Box instance.
 * @param x Output pointer for x coordinate.
 * @param y Output pointer for y coordinate.
 */
void Box_getPosition(Box* self, float* x, float* y);

/**
 * @brief Set the box position.
 *
 * If the box is centered, the stored center is updated accordingly.
 *
 * @param self Box instance.
 * @param x New x coordinate (interpreted as top-left or center per \p center).
 * @param y New y coordinate.
 */
void Box_setPosition(Box* self, float x, float y);


/**
 * @struct Circle
 * @brief Represents a filled circle with optional border.
 *
 * The Circle stores radius, border size, center position and colors.
 */
struct Circle {
    /** @brief Radius of the circle in pixels. */
    int radius;
    /** @brief Border thickness in pixels. */
    int border_size;
    /** @brief Center position of the circle. */
    Position* center;
    /** @brief Fill/background color (may be NULL). */
    Color* background;
    /** @brief Border color (may be NULL). */
    Color* border_color;
};

/**
 * @brief Create a new Circle instance.
 *
 * @param radius Circle radius in pixels.
 * @param border_size Border thickness in pixels.
 * @param center Pointer to center Position.
 * @param background Fill color pointer (may be NULL).
 * @param border_color Border color pointer (may be NULL).
 * @return Newly allocated Circle or NULL on failure.
 */
Circle* Circle_new(int radius, int border_size, Position* center, Color* background, Color* border_color);

/**
 * @brief Destroy a Circle and free its resources.
 *
 * Safe to call with NULL.
 *
 * @param self Circle instance to destroy.
 */
void Circle_destroy(Circle* self);

/**
 * @brief Render the circle using the provided SDL_Renderer.
 *
 * Draws the border (if any) and fills the interior with the background color.
 *
 * @param self Circle to render.
 * @param renderer SDL_Renderer used for drawing.
 */
void Circle_render(Circle* self, SDL_Renderer* renderer);


/**
 * @struct Polygon
 * @brief Represents a filled polygon with optional border.
 *
 * The Polygon stores a dynamic array of vertex positions, fill and border colors,
 * and border thickness. Vertex winding and validity are the caller's responsibility.
 */
struct Polygon {
    /** @brief Array of pointers to vertex positions (length = vertex_count). */
    Position** vertices;
    /** @brief Number of vertices currently stored. */
    int vertex_count;
    /** @brief Fill/background color (may be NULL). */
    Color* background;
    /** @brief Border color (may be NULL). */
    Color* border;
    /** @brief Border thickness in pixels. */
    int border_size;
};

/**
 * @brief Create a new Polygon from an array of vertices.
 *
 * @param vertices Array of Position\* (ownership as per implementation).
 * @param vertex_count Number of vertices.
 * @param border_size Border thickness in pixels.
 * @param background Fill color pointer (may be NULL).
 * @param border Border color pointer (may be NULL).
 * @return Newly allocated Polygon or NULL on failure.
 */
Polygon* Polygon_new(Position** vertices, int vertex_count, int border_size, Color* background, Color* border);

/**
 * @brief Destroy a Polygon and free its resources.
 *
 * Iterates and destroys owned vertices if applicable.
 *
 * @param self Polygon instance to destroy.
 */
void Polygon_destroy(Polygon* self);

/**
 * @brief Render the polygon using the provided SDL_Renderer.
 *
 * Fills the polygon interior and optionally draws the border.
 *
 * @param self Polygon to render.
 * @param renderer SDL_Renderer used for drawing.
 */
void Polygon_render(Polygon* self, SDL_Renderer* renderer);

/**
 * @brief Create an empty Polygon ready to accept vertices.
 *
 * @param border_size Border thickness in pixels.
 * @param background Fill color pointer (may be NULL).
 * @param border Border color pointer (may be NULL).
 * @return Newly allocated empty Polygon or NULL on failure.
 */
Polygon* Polygon_newEmpty(int border_size, Color* background, Color* border);

/**
 * @brief Add a vertex to the polygon.
 *
 * Reallocates internal vertex array to accommodate the new vertex.
 *
 * @param self Polygon instance.
 * @param vertex Pointer to Position to add.
 */
void Polygon_addVertex(Polygon* self, Position* vertex);
