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

#ifndef __AABB_H__
#define __AABB_H__

#include <stdbool.h>

#include "engine/vec3.h"

typedef struct {
    float min_x;
    float min_y;
    float min_z;
    float max_x;
    float max_y;
    float max_z;
} AABB;

// Check if a 3D point is inside a box
bool aabb_point_in_aabb(vec3 point, AABB box);
// Check if 2 bounding boxes overlap
bool aabb_overlap(AABB a, AABB b);
// Offset a box by a vector
AABB aabb_offset(AABB box, vec3 offset);
// Get the x,y and z extents of the box
vec3 aabb_extents(AABB box);
// Create a symmetrical aabb from width and height, useful for actors
AABB aabb_from_size(float width, float height);

#endif
