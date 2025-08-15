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

#include <math.h>
#include <stdbool.h>

#include "engine/aabb.h"
#include "engine/world.h"

bool check_world_collision(const World *world, AABB box) {
    // Convert AABB float coordinates to voxel integer coordinates
    // Use floorf for min coordinates and ceilf for max coordinates to cover
    // all potentially overlapping voxels
    size_t min_vx = floorf(box.min_x);
    // Use floor on max as well, voxel index is inclusive
    size_t max_vx = floorf(box.max_x);
    size_t min_vz = floorf(box.min_z);
    size_t max_vz = floorf(box.max_z);
    // INVERTED Y: min_y is higher up (smaller world Y index), max_y is lower
    // down (larger world Y index)
    size_t min_vy = floorf(box.min_y);
    size_t max_vy = floorf(box.max_y);

    for (size_t y = min_vy; y <= max_vy; y++) {
        // Clamp Y to world bounds (remembering Y (aka Z) is inverted AGAIN)
        // Skip checks outside world height
        if (y < 0 || y >= world->height) continue;

        for (size_t z = min_vz; z <= max_vz; z++) {
            for (size_t x = min_vx; x <= max_vx; x++) {
                if (world_get(world, (size_t)x, (size_t)y, (size_t)z) != 0) {
                    // Collision detected
                    return true;
                }
            }
        }
    }
    return false;
}
