/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */
#include "geometry.h"

#include "logger.h"
#include "utils.h"

Box* Box_new(float width, float height, int border_size, Position* position, Color* background, Color* border_color, bool center) {
    Box* self = calloc(1, sizeof(Box));
    if (!self) {
        error("Box_new: Failed to allocate memory for Box");
        return NULL;
    }
    self->size.width = width;
    self->size.height = height;
    self->border_size = border_size;
    self->position = position;
    self->background = background;
    self->border_color = border_color;
    self->center = center;
    return self;
}

void Box_destroy(Box* self) {
    if (!self) return;
    Position_destroy(self->position);
    Color_destroy(self->background);
    Color_destroy(self->border_color);
    safe_free((void**)&self);
}

void Box_render(Box* self, SDL_Renderer* renderer) {
    if (!self || !renderer) return;

    SDL_FRect rect = self->center ? SDL_CreateRect(self->position->x, self->position->y, self->size.width, self->size.height, self->center) :
                (SDL_FRect){ self->position->x, self->position->y, self->size.width, self->size.height };

    if (self->border_size > 0 && self->border_color) {
        Color* border = self->border_color;
        SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);
        SDL_FRect borderRect = {  rect.x - self->border_size, rect.y - self->border_size, rect.w + (self->border_size * 2), rect.h + (self->border_size * 2)};
        SDL_RenderFillRect(renderer, &borderRect);
    }

    if (self->background) {
        Color* background = self->background;
        SDL_SetRenderDrawColor(renderer, background->r, background->g, background->b, background->a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

Circle* Circle_new(int radius, int border_size, Position* center, Color* background, Color* border_color) {
    Circle* self = calloc(1, sizeof(Circle));
    if (!self) {
        error("Circle_new: Failed to allocate memory for Circle");
        return NULL;
    }
    self->radius = radius;
    self->border_size = border_size;
    self->center = center;
    self->background = background;
    self->border_color = border_color;
    return self;
}

void Circle_destroy(Circle* self) {
    if (!self) return;
    Position_destroy(self->center);
    Color_destroy(self->background);
    Color_destroy(self->border_color);
    safe_free((void**)&self);
}

static void Circle_draw(SDL_Renderer* renderer, float centerX, float centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void Circle_render(Circle* self, SDL_Renderer *renderer) {
    if (!self || !renderer) return;

    if (self->border_size > 0 && self->border_color) {
        Color* border = self->border_color;
        SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);
        Circle_draw(renderer, self->center->x, self->center->y, self->radius + self->border_size);
    }
    if (self->background) {
        Color* background = self->background;
        SDL_SetRenderDrawColor(renderer, background->r, background->g, background->b, background->a);
        Circle_draw(renderer, self->center->x, self->center->y, self->radius);
    }
}

Polygon* Polygon_new(Position** vertices, int vertex_count, int border_size, Color* background, Color* border) {
    Polygon* self = calloc(1, sizeof(Polygon));
    if (!self) {
        error("Polygon_new: Failed to allocate memory for Polygon");
        return NULL;
    }
    self->vertices = vertices;
    self->vertex_count = vertex_count;
    self->border_size = border_size;
    self->background = background;
    self->border = border;
    return self;
}

void Polygon_destroy(Polygon* self) {
    if (!self) return;
    for (int i = 0; i < self->vertex_count; i++) {
        Position_destroy(self->vertices[i]);
    }
    safe_free((void**)&self->vertices);
    Color_destroy(self->background);
    Color_destroy(self->border);
    safe_free((void**)&self);
}

typedef struct {
    float x;
} Intersection;

static int compare_intersections(const void* a, const void* b) {
    return ((Intersection*)a)->x - ((Intersection*)b)->x;
}

void Polygon_render(Polygon* self, SDL_Renderer* renderer) {
    if (!self || !renderer || self->vertex_count < 3) return;

    Color* background = self->background;
    SDL_SetRenderDrawColor(renderer, background->r, background->g, background->b, background->a);

    float min_y = self->vertices[0]->y, max_y = self->vertices[0]->y;
    float min_x = self->vertices[0]->x, max_x = self->vertices[0]->x;

    for (int i = 1; i < self->vertex_count; i++) {
        if (self->vertices[i]->y < min_y) min_y = self->vertices[i]->y;
        if (self->vertices[i]->y > max_y) max_y = self->vertices[i]->y;
        if (self->vertices[i]->x < min_x) min_x = self->vertices[i]->x;
        if (self->vertices[i]->x > max_x) max_x = self->vertices[i]->x;
    }

    Intersection* intersections = calloc(self->vertex_count, sizeof(Intersection));
    if (!intersections) {
        error("Polygon_render: Failed to allocate memory for intersections");
        return;
    }

    for (int y = min_y; y <= max_y; y++) {
        int nb_intersections = 0;

        for (int i = 0; i < self->vertex_count; i++) {
            int j = (i + 1) % self->vertex_count;

            float y1 = self->vertices[i]->y;
            float y2 = self->vertices[j]->y;
            float x1 = self->vertices[i]->x;
            float x2 = self->vertices[j]->x;

            if ((y1 <= y && y < y2) || (y2 <= y && y < y1)) {
                int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                intersections[nb_intersections].x = x;
                nb_intersections++;
            }
        }

        qsort(intersections, nb_intersections, sizeof(Intersection), compare_intersections);

        for (int i = 0; i < nb_intersections - 1; i += 2) {
            SDL_RenderLine(renderer, intersections[i].x, y, intersections[i + 1].x, y);
        }
    }

    if (self->border && self->border_size > 0) {
        Color* border = self->border;
        SDL_SetRenderDrawColor(renderer, border->r, border->g, border->b, border->a);
        for (int i = 0; i < self->vertex_count; i++) {
            int j = (i + 1) % self->vertex_count;
            float x1 = self->vertices[i]->x;
            float y1 = self->vertices[i]->y;
            float x2 = self->vertices[j]->x;
            float y2 = self->vertices[j]->y;

            float dx = x2 - x1;
            float dy = y2 - y1;
            float len = sqrtf(dx * dx + dy * dy);
            float nx = -dy / len;
            float ny = dx / len;
            for (int b = -self->border_size/2; b <= self->border_size/2; b++) {
                float ox = nx * b;
                float oy = ny * b;
                SDL_RenderLine(renderer, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
            }
        }
    }

    safe_free((void**)&intersections);
}

Polygon* Polygon_newEmpty(int border_size, Color* background, Color* border) {
    Polygon* self = calloc(1, sizeof(Polygon));
    if (!self) {
        error("Polygon_newEmpty: Failed to allocate memory for Polygon");
        return NULL;
    }
    self->vertices = calloc(1, sizeof(Position*));
    if (!self->vertices) {
        error("Polygon_newEmpty: Failed to allocate memory for vertices");
        safe_free((void**)&self);
        return NULL;
    }
    self->vertex_count = 0;
    self->border_size = border_size;
    self->background = background;
    self->border = border;
    return self;
}

void Polygon_addVertex(Polygon* self, Position* vertex) {
    self->vertices[self->vertex_count++] = vertex;
    Position** new_vertices = realloc(self->vertices, (self->vertex_count + 1) * sizeof(Position*));
    if (!new_vertices) {
        error("Polygon_addVertex: Failed to reallocate memory for vertices");
        return;
    }
    self->vertices = new_vertices;
}
