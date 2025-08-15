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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <float.h>

#include "vec3.h"

typedef struct {
    vec3 pos;
    float yaw;   // Rotation around Y axis
    float pitch; // Rotation around X axis (clamped)
    float pitch_offset; // Extra offset for effects
    vec3 forward;
    vec3 right;
    vec3 up;
} EngineCamera;

void camera_update_vectors(EngineCamera *cam);

#endif
