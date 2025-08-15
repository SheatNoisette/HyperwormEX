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

#ifndef __WORLD_H__
#define __WORLD_H__

#include <stdint.h>
#include <stddef.h>

typedef struct World {
    size_t width;
    size_t height;
    size_t depth;
    size_t nb;
    uint8_t *data;
} World;

World *new_world(size_t width, size_t height, size_t depth);
void world_free(World *world);

uint8_t world_get(const World *world, size_t x, size_t y, size_t z);
void world_set(World *world, size_t x, size_t y, size_t z, uint8_t type);
void world_break(World *world, size_t x, size_t y, size_t z);

#endif
