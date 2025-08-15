
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

#include <stddef.h>
#include <stdint.h>

#include "engine/utils.h"

void draw_circle(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int pos_x,
    int pos_y,
    int radius,
    uint32_t color
) {
    // Calculate circle's bounding box
    int x_min = max(0, pos_x - radius);
    int x_max = min(buffer_width, pos_x + radius + 1);
    int y_min = max(0, pos_y - radius);
    int y_max = min(buffer_height, pos_y + radius + 1);

    int radius_2 = radius * radius;

    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++) {
            // Calculate distance from center
            int x_2 = (x - pos_x)*(x - pos_x);
            int y_2 = (y - pos_y)*(y - pos_y);

            if (x_2 + y_2 < radius_2) {
                buffer[y * buffer_width + x] = color;
            }
        }
    }
}

void draw_triangle(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    uint32_t color
) {
    // Loosely based on http://sw-shader.sourceforge.net/rasterizer.html
    int x_min = max(0, min(x1, min(x2, x3)));
    int x_max = min(buffer_width, max(x1, max(x2, x3)));
    int y_min = max(0, min(y1, min(y2, y3)));
    int y_max = min(buffer_height, max(y1, max(y2, y3)));

    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++) {
            if (((x1 - x2) * (y - y1) - (y1 - y2) * (x - x1) > 0) &&
                ((x2 - x3) * (y - y2) - (y2 - y3) * (x - x2) > 0) &&
                ((x3 - x1) * (y - y3) - (y3 - y1) * (x - x3) > 0)) {
                buffer[y*buffer_width+x] = color;
            }
        }
    }
}

void draw_line(
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    int x1, int y1,
    int x2, int y2,
    int width,
    uint32_t color
) {
    // Value doesn't really matter, this should not be used real-time anyway
    static const int steps = 100;

    float x_step = (float)(x2 - x1)/steps;
    float y_step = (float)(y2 - y1)/steps;

    float x = x1;
    float y = y1;
    while (x <= x2 || y <= y2) {
        draw_circle(buffer, buffer_width, buffer_height, x, y, width, color);

        x += x_step;
        y += y_step;
    }
}
