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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "engine/vec3.h"
#include "engine/aabb.h"

// Checks if a 3D point is inside an Axis-Aligned Bounding Box (AABB).
bool aabb_point_in_aabb(vec3 point, AABB box) {
    return (
        point.x >= box.min_x && point.x <= box.max_x &&
        point.y >= box.min_y && point.y <= box.max_y &&
        point.z >= box.min_z && point.z <= box.max_z
    );
}

bool aabb_overlap(AABB a, AABB b) {
    return (
        a.min_x <= b.max_x && a.max_x >= b.min_x &&
        a.min_y <= b.max_y && a.max_y >= b.min_y &&
        a.min_z <= b.max_z && a.max_z >= b.min_z
    );
}

// Offset a box by a vector
AABB aabb_offset(AABB box, vec3 offset) {
    return (AABB){
        .min_x = box.min_x + offset.x,
        .min_y = box.min_y + offset.y,
        .min_z = box.min_z + offset.z,
        .max_x = box.max_x + offset.x,
        .max_y = box.max_y + offset.y,
        .max_z = box.max_z + offset.z,
    };
}

// Get the x, y and z extents of the box
vec3 aabb_extents(AABB box) {
    return (vec3){
        .x = box.max_x - box.min_x,
        .y = box.max_y - box.min_y,
        .z = box.max_z - box.min_z,
    };
}

AABB aabb_from_size(float width, float height) {
    return (AABB){
        .min_x = -width / 2.0f,
        .max_x = width / 2.0f,
        .min_z = -width / 2.0f,
        .max_z = width / 2.0f,
        // Base is at Y = 0. Top is Y - height
        // Top Y coordinate
        .min_y = -height,
        // Bottom Y coordinate - origin is center of feet (fetish)
        .max_y = 0,
    };
}
