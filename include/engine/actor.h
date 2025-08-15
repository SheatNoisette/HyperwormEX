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

#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <stdlib.h>
#include <stdbool.h>

#include "engine/vec3.h"
#include "engine/aabb.h"
#include "engine/const.h"

// Raycast from a camera
// Maximum distance the ray travels
#define MAX_RAYCAST_DISTANCE 255.0f
// How far the ray steps each check
#define RAYCAST_STEP 0.1f

// Physics interactions
// Distance (squared) to apply collision between 2 actors
#define ACTOR_MIN_DIST_2 0.8f
// Force to nudge actors apart when they collide
#define NUDGE_FORCE 2.0f

// Forward declare Render to avoid recursive includes
typedef struct Render Render;
typedef struct Actor Actor;
typedef struct Billboard Billboard;

typedef struct Actor {
    // Should be updated or not
    bool is_active;
    // Should collide with other solid actors
    bool is_solid;
    // The ID of the actor
    size_t id;
    // Type of the actor
    int type;
    // The position of the actor
    vec3 pos;
    // The velocity of the actor
    vec3 velocity;
    // Whether the actor is on the ground
    bool on_ground;
    // The ray casting hitbox
    AABB hitbox;
    // The collision bounding box
    AABB collision;
    // Associated billboard
    Billboard *billboard;
    // Init func
    void (*init)(Actor *self, Render *rd);
    // Update func
    void (*update)(Actor *self, Render *rd);
    // Draw the actor (after scene rendering)
    void (*draw)(Actor *self, Render *rd);
    // Destroy func
    void (*destroy)(Actor *self, Render *rd);
    // Getting hit by a raycast
    void (*hit)(Actor *self, Actor *hitter, Render *rd);
    // Custom data if any
    void *data;
} Actor;

typedef struct RaycastResult {
    vec3 hit;
    vec3 last_pos;
} RaycastResult;

// Make a new Actor
#define ACTOR_NEW(name, storage_type, init_func, update_func, hit_func, draw_func, destroy_func) \
Actor *actor_##name(Render *rd, vec3 pos) { \
    Actor *act = actor_new((void *)malloc(sizeof(storage_type)), rd); \
    if (act == NULL) return NULL; \
    \
    /* Set position */ \
    act->pos = pos; \
    \
    /* Specific function pointers */ \
    act->init = init_func; \
    act->update = update_func; \
    act->hit = hit_func; \
    act->draw = draw_func; \
    act->destroy = destroy_func; \
    /* Initialize the actor */ \
    act->init(act, rd); \
    \
    return act; \
}

// Spawn a new actor
#define ACTOR_SPAWN(rd, name, pos) actor_##name(rd, pos)

// Used for the preprocessor for the header only
#define ACTOR_DECLARE(name) Actor *actor_##name(Render *rd, vec3 pos);

// Access the actor's custom data
#define ACTOR_DATA(act, storage_type) ((storage_type *)(act->data))

Actor *actor_new(void *custom_data, Render *rd);
void actor_destroy(Actor *act, Render *rd);

void actor_init(Actor **actors, Render *rd);
void actor_update_all(Actor **actors, Render *rd);
void actor_draw_all(Actor **actors, Render *rd);
void actor_destroy_all(Actor **actors, Render *rd);

AABB actor_world_hitbox(Actor *act);
AABB actor_world_collision(Actor *act);

// Move the actor based on its velocity (updates velocity accordingly)
// Return true if collided with world
bool actor_move(Render *rd, Actor *act);
// Cast a ray from a position, excluding the actor
RaycastResult actor_perform_raycast(Render *rd, Actor *shooter, vec3 origin, vec3 direction);

#endif
