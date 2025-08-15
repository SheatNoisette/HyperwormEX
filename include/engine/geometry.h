/*
 * Copyright (c) 2025 SheatNoisette & HeraldOD
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <stddef.h>
#include <stdint.h>

// Draw a circle to the given buffer
void draw_circle(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int pos_x,
    int pos_y,
    int radius,
    uint32_t color
);

#define draw_circle_sprite(s, x, y, r, c) \
draw_circle(s->data, s->width, s->height, x, y, r, c)

// Draw a triangle to the given buffer
void draw_triangle(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    uint32_t color
);

#define draw_triangle_sprite(s, x1, y1, x2, y2, x3, y3, c) \
draw_triangle(s->data, s->width, s->height, x1, y1, x2, y2, x3, y3, c)

// Draw a line to the given buffer
void draw_line(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int x1, int y1,
    int x2, int y2,
    int width,
    uint32_t color
);

#define draw_line_sprite(s, x1, y1, x2, y2, w, c) \
draw_line(s->data, s->width, s->height, x1, y1, x2, y2, w, c)

#endif
