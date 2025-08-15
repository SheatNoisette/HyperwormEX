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

#ifndef __VEC3_H__
#define __VEC3_H__

#include <stdint.h>
#include <stddef.h>
#include <math.h>

typedef struct {
    float x, y, z;
} vec3;

vec3 v3_add(vec3 a, vec3 b);
vec3 v3_sub(vec3 a, vec3 b);
vec3 v3_muls(vec3 a, float s);
float v3_dot(vec3 a, vec3 b);
float v3_length(vec3 a);
vec3 v3_norm(vec3 a);

// Length squared
#define v3_len_2(a) v3_dot(a, a)

// Distance between two vectors
#define v3_distance(a, b) v3_length(v3_sub(a, b))

// Move between two vectors with a constant step
vec3 v3_move_to(vec3 a, vec3 b, float step);
// Lerp between two vectors
// Freya's framerate-independent lerp
// Decay: useful range 1-25, from slow to fast
vec3 v3_lerp(vec3 a, vec3 b, float decay, float dt);
// Create a random vector
vec3 v3_rand(float scale);
// Get the angle between 2 vectors (XZ plane)
float v3_angle(vec3 a, vec3 b);

#endif
