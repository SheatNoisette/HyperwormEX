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

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"

// Find a available slot to put an actor in
// Return -1 if no slot is found
static int actor_find_available_slot(size_t *slot, Render *rd) {
    // Check if we can claim one
    for (size_t act = 0; act < rd->num_actors; act++) {
        if (rd->actors[act] == NULL) {
            *slot = act;
            return 0;
        }
    }

    // Otherwise add to end
    if (rd->num_actors < MAX_ACTORS) {
        *slot = rd->num_actors;
        rd->num_actors++;
        // We found a slot
        return 0;
    }

    // We didn't find a slot, too many actors
    return 1;
}

// Create and register a new actor
Actor *actor_new(void *custom_data, Render *rd) {
    Actor *act = malloc(sizeof(Actor));
    size_t actor_slot = 0;
#if SAFE_C
    if (act == NULL)
        return NULL;
#endif
    act->is_active = true;

    // Find a slot
    if (actor_find_available_slot(&actor_slot, rd) != 0) {
#if SAFE_C
        if (act)
            free(act);
#endif
        ENGINE_LOG("No more actor slots!\n");
        return NULL;
    }

    // Register the actor
    rd->actors[actor_slot] = act;
    act->id = actor_slot;

    // Common Actor properties
    act->is_active = true;
    act->is_solid = false;
    act->type = -1;
    act->velocity = (vec3){0};
    act->on_ground = false;

    // Set custom data
    act->data = custom_data;

    return act;
}

// Clear the whole array
void actor_init(Actor **actors, Render *rd) {
    rd->num_actors = 0;
    memset(actors, 0 /* NULL not accepted ?! */, sizeof(Actor *) * MAX_ACTORS);
}

// Update all actors in the scene
void actor_update_all(Actor **actors, Render *rd) {
    for (size_t act = 0; act < rd->num_actors; act++) {
        if (actors[act] == NULL || !actors[act]->is_active)
            continue;

        if (actors[act]->update != NULL)
            actors[act]->update(actors[act], rd);
    }
}

// Draw all actors in the scene
void actor_draw_all(Actor **actors, Render *rd) {
    for (size_t act = 0; act < rd->num_actors; act++) {
        if (actors[act] == NULL)
            continue;

        if (actors[act]->draw != NULL)
            actors[act]->draw(actors[act], rd);
    }
}

// Destroy the actor and free it
// @NOTE: This function can be used as a destroy callback
void actor_destroy(Actor *act, Render *rd) {
    size_t id = act->id;
    // Just in case somehow the pointer is different from the id
    Actor *c_act = rd->actors[id];
    // Free the data associated
    if (c_act->data)
        free(c_act->data);
    // Destroy the actor itself
    free(c_act);
    // Mark this slot as available to be reused
    rd->actors[id] = NULL;
    // Tada!
}

// Destroy every actor in the scene
// Call their destroy callback
void actor_destroy_all(Actor **actors, Render *rd) {
    for (size_t i = 0; i < rd->num_actors; i++) {
        Actor *act = rd->actors[i];
        if (act != NULL && act->destroy != NULL) {
            act->destroy(actors[i], rd);
            // destroy should already free and set null, no need here
        }
    }

#if SAFE_C
    rd->num_actors = 0;
#endif
}
