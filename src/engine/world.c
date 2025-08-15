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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include "engine/world.h"
#include "textures.h"

// Create a new world and allocate voxel data
World *new_world(size_t width, size_t height, size_t depth) {
    World *world = malloc(sizeof(World));
    world->width = width;
    world->height = height;
    world->depth = depth;
    world->nb = world->width * world->height * world->depth;
    world->data = malloc(world->nb * sizeof(uint8_t));

    return world;
}

void world_free(World *world) {
    if (world->data != NULL) {
        free(world->data);
        world->data = NULL;
    }

    free(world);
}

// Get voxel type at world coordinates (returns 0 if out of bounds)
uint8_t world_get(const World *world, size_t x, size_t y, size_t z)
{
    if (x < 0 || x >= world->width ||
        y < 0 || y >= world->height ||
        z < 0 || z >= world->depth)
    {
        return 0; // Air
    }
    size_t index = y * world->width * world->depth + z * world->width + x;
    return world->data[index];
}

// Set voxel type (careful with bounds)
void world_set(World *world, size_t x, size_t y, size_t z, uint8_t type)
{
    if (x < 0 || x >= world->width ||
        y < 0 || y >= world->height ||
        z < 0 || z >= world->depth)
    {
        return;
    }
    size_t index = y * world->width * world->depth + z * world->width + x;
    world->data[index] = type;
}

void world_break(World *world, size_t x, size_t y, size_t z) {
    if (x < 0 || x >= world->width ||
        y < 0 || y >= world->height ||
        z < 0 || z >= world->depth)
    {
        return;
    }
    size_t index = y * world->width * world->depth + z * world->width + x;

    // Barriers are unbreakable :^)
    if (world->data[index] != TEX_BARRIER) {
        world->data[index] = 0; // Air
    }
}
