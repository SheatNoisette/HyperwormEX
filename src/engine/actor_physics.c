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

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "engine/aabb.h"
#include "engine/actor.h"
#include "engine/collision.h"
#include "engine/render.h"
#include "engine/vec3.h"

AABB actor_world_hitbox(Actor *act) {
    return aabb_offset(act->hitbox, act->pos);
}

AABB actor_world_collision(Actor *act) {
    return aabb_offset(act->collision, act->pos);
}

// Collide with other actors
bool collide_with_actors(Render *rd, Actor *act) {
    bool collided = false;

    for (size_t i = 0; i < rd->num_actors; i++) {
        Actor *act2 = rd->actors[i];

        // Skip inactive actors, null pointers, or the current actor
        if (!act2 || !act2->is_active || !act2->is_solid || act2 == act) {
            continue;
        }

        // Vector to other actor
        // HACK: use billboard coords for center
        // (we assume all solid actors have a billboard)
        vec3 v = v3_sub(act2->billboard->pos, act->billboard->pos);
        // Square distance between actors
        float dist_2 = v3_len_2(v);

        // If the actors are too close, nudge them apart
        if (dist_2 < ACTOR_MIN_DIST_2) {
            collided = true;

            v.y = 0.0f;
            vec3 v_horiz = v;
            vec3 nudge = v3_muls(v_horiz, -NUDGE_FORCE);
            vec3 nudge2 = v3_muls(v_horiz, NUDGE_FORCE);

            act->velocity = v3_add(act->velocity, nudge);
            act2->velocity = v3_add(act2->velocity, nudge2);
        }
    }

    return collided;
}

// Move the actor based on its velocity (updates velocity accordingly)
// Return true if collided with world
bool actor_move(Render *rd, Actor *act) {
    // Collision with other actors
    bool collided = act->is_solid && collide_with_actors(rd, act);

    // Calculate potential new position
    vec3 delta_pos = v3_muls(act->velocity, GET_DELTATIME(rd));
    vec3 potential_pos = v3_add(act->pos, delta_pos);

    // Reset on_ground, it will be set if falling and colliding on Y
    act->on_ground = false;

    // Calculate Y
    AABB next_y_aabb = actor_world_collision(act);
    next_y_aabb.min_y += delta_pos.y;
    next_y_aabb.max_y += delta_pos.y;

    if (check_world_collision(rd->world, next_y_aabb)) {
        collided = true;
        act->velocity.y = 0;

        if (delta_pos.y > 0) {
            act->on_ground = true;
        }
    } else {
        act->pos.y = potential_pos.y;
    }

    // Calculate X with updated Y pos
    AABB next_x_aabb = actor_world_collision(act);
    next_x_aabb.min_x += delta_pos.x;
    next_x_aabb.max_x += delta_pos.x;

    // Stop X movement
    if (check_world_collision(rd->world, next_x_aabb)) {
        collided = true;
        potential_pos.x = act->pos.x;
        act->velocity.x = 0;
    }
    // Update X position
    act->pos.x = potential_pos.x;

    // Calculate Z with updated X and Y pos
    AABB next_z_aabb = actor_world_collision(act);
    next_z_aabb.min_z += delta_pos.z;
    next_z_aabb.max_z += delta_pos.z;

    if (check_world_collision(rd->world, next_z_aabb)) {
        collided = true;
        // Stop Z movement
        potential_pos.z = act->pos.z;
        act->velocity.z = 0;
    }
    // Update Z position
    act->pos.z = potential_pos.z;

    return collided;
}

// Performs a raycast from camera perspective
RaycastResult actor_perform_raycast(Render *rd, Actor *self, vec3 origin, vec3 direction) {
    AABB world_hitbox;
    vec3 old_position = v3_add(origin, v3_muls(direction, RAYCAST_STEP));

    // Iterate along the ray's path
    for (float dist = RAYCAST_STEP; dist < MAX_RAYCAST_DISTANCE; dist += RAYCAST_STEP) {
        // Calculate the current point along the ray in world space
        vec3 check_pos = v3_add(origin, v3_muls(direction, dist));

        // Check for world collision
        size_t ix = (size_t)floorf(check_pos.x);
        size_t iy = (size_t)floorf(check_pos.y);
        size_t iz = (size_t)floorf(check_pos.z);

        // Ray hit a solid block in the world
        if (world_get(rd->world, ix, iy, iz) > 0) {
            return (RaycastResult){
                .hit = check_pos,
                .last_pos = old_position,
            };
        }

        // Check for actor collision
        for (size_t i = 0; i < rd->num_actors; i++) {
            Actor *act = rd->actors[i];

            // Skip inactive actors, null pointers, or the shooter itself
            if (!act || !act->is_active || act == self) {
                continue;
            }

            // Get the target actor's hitbox in world space
            world_hitbox = actor_world_hitbox(act);

            // Check if the current ray point is within the actor's hitbox
            if (aabb_point_in_aabb(check_pos, world_hitbox)) {
                // Call target actor's hit function
                if (act->hit != NULL) act->hit(act, self, rd);

                return (RaycastResult){
                    .hit = check_pos,
                    .last_pos = old_position,
                };
            }
        }
        old_position = check_pos;
    }

    // @TODO: Better way to return that
    return (RaycastResult){
        .hit = (vec3){-1, -1, -1},
        .last_pos = (vec3){-1, -1, -1},
    };
}
