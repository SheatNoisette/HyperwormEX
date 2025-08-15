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
#include <stdbool.h>
#include <math.h>
#include <float.h>

#include "engine/camera.h"
#include "engine/utils.h"
#include "engine/vec3.h"

void camera_update_vectors(EngineCamera *cam)
{
    // Clamp pitch
    cam->pitch = clampf(cam->pitch, -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
    float effective_pitch = cam->pitch + cam->pitch_offset;

    // Calculate forward vector
    cam->forward.x = cosf(effective_pitch) * sinf(cam->yaw);
    cam->forward.y = sinf(effective_pitch);
    cam->forward.z = cosf(effective_pitch) * cosf(cam->yaw);
    // Ensure it's normalized of sometimes is messes up ray calculation
    cam->forward = v3_norm(cam->forward);

    cam->right.x = cosf(cam->yaw);
    cam->right.y = 0;
    cam->right.z = -sinf(cam->yaw);
    cam->right = v3_norm(cam->right);

    // Calculate camera's up vector (cross product of right and forward)
    cam->up.x = cam->right.y * cam->forward.z - cam->right.z * cam->forward.y;
    cam->up.y = cam->right.z * cam->forward.x - cam->right.x * cam->forward.z;
    cam->up.z = cam->right.x * cam->forward.y - cam->right.y * cam->forward.x;
    cam->up = v3_norm(cam->up);
}
