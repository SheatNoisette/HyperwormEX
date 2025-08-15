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

#ifndef __TEXT_H__
#define __TEXT_H__

#include <stddef.h>
#include <stdint.h>

#define LETTER_WIDTH 5
#define LETTER_HEIGHT 7
#define LETTER_SPACING 1

#define TEXT_WIDTH(n) (n * (LETTER_WIDTH + LETTER_SPACING))

#define text_draw_fb(rd, text, pos_x, pos_y, color) text_draw(rd->frame_buffer, SOFTWARE_WIDTH, SOFTWARE_HEIGHT, text, (pos_x), (pos_y), (color))

#define text_draw_sprite(sprite, text, pos_x, pos_y, color) text_draw(sprite->data, sprite->width, sprite->height, text, (pos_x), (pos_y), (color))

#define text_draw_centered_fb(rd, text, pos_y, color) text_draw_centered(rd->frame_buffer, SOFTWARE_WIDTH, SOFTWARE_HEIGHT, text, (pos_y), (color))

// Draw text to a buffer
void text_draw (
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    const char *text,
    size_t position_x,
    size_t position_y,
    uint32_t color
);

// Draw horizontally centered text
void text_draw_centered (
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    const char *text,
    size_t position_y,
    uint32_t color
);

#endif
