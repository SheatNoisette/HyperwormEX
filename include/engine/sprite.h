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

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <stddef.h>
#include <stdint.h>

#include "engine/render.h"

#define SPRITE_SIZE(spr) (sizeof(uint32_t) * spr->width * spr->height)

// Create a new sprite
Sprite *sprite_new(size_t width, size_t height, size_t center_x, size_t center_y);
// Create a new sprite pre-filled with text
Sprite *sprite_new_text(const char *text, uint32_t color);

void sprite_pixel(Sprite *spr, size_t pos_x, size_t pos_y, uint32_t color);
void sprite_draw_rotation(Render *rd, Sprite *spr, size_t pos_x, size_t pos_y, float angle, float zoom);

// Generate a 2 bit per pixel sprite using a palette
void sprite_unpack_2bit(
    Sprite *spr,
    uint8_t *sprite,
    uint32_t palette[4],
    size_t w,
    size_t h
);

#endif
