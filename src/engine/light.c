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

#include <stdbool.h>
#include <math.h>

#include "engine/vec3.h"
#include "engine/world.h"
#include "engine/const.h"

// DDA-based shadow rendering
// @FIXME: THIS IS REALLY SLOW
bool is_in_shadow(const World *world, vec3 start_pos, vec3 light_pos, float dist_to_light) {
    float current_dist = 0.0f;
    vec3 step_dir;
    vec3 side_dist;

    vec3 dir_to_light = v3_norm(v3_sub(light_pos, start_pos));

    // Start the shadow ray slightly offset from the surface to avoid
    // self-intersection
    vec3 ray_pos = v3_add(start_pos, v3_muls(dir_to_light, 0.01f));
    vec3 map_pos = {
        floorf(ray_pos.x),
        floorf(ray_pos.y),
        floorf(ray_pos.z)
    };

    // Clamp to avoid small errors and holes
    vec3 delta_dist = {
        fabsf(dir_to_light.x) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / dir_to_light.x),
        fabsf(dir_to_light.y) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / dir_to_light.y),
        fabsf(dir_to_light.z) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / dir_to_light.z)
    };

    // Determine howto step relative to ray direction
    if (dir_to_light.x < 0) { step_dir.x = -1; side_dist.x = (ray_pos.x - map_pos.x) * delta_dist.x; }
    else { step_dir.x = 1; side_dist.x = (map_pos.x + 1.0f - ray_pos.x) * delta_dist.x; }
    if (dir_to_light.y < 0) { step_dir.y = -1; side_dist.y = (ray_pos.y - map_pos.y) * delta_dist.y; }
    else { step_dir.y = 1; side_dist.y = (map_pos.y + 1.0f - ray_pos.y) * delta_dist.y; }
    if (dir_to_light.z < 0) { step_dir.z = -1; side_dist.z = (ray_pos.z - map_pos.z) * delta_dist.z; }
    else { step_dir.z = 1; side_dist.z = (map_pos.z + 1.0f - ray_pos.z) * delta_dist.z; }

    while (current_dist < dist_to_light) {

        // Determine which axis to step along
        if (side_dist.x < side_dist.y && side_dist.x < side_dist.z) {
            current_dist = side_dist.x;
            side_dist.x += delta_dist.x;
            map_pos.x += step_dir.x;
        } else if (side_dist.y < side_dist.z) {
            current_dist = side_dist.y;
            side_dist.y += delta_dist.y;
            map_pos.y += step_dir.y;
        } else {
            current_dist = side_dist.z;
            side_dist.z += delta_dist.z;
            map_pos.z += step_dir.z;
        }

        // Check if the current distance exceeds the distance to the light
        if (current_dist >= dist_to_light)
            break;

        // Check the voxel at the new map position
        if (world_get(world, (size_t)map_pos.x, (size_t)map_pos.y, (size_t)map_pos.z) > 0) {
            return true;
        }
    }
    return false;
}
