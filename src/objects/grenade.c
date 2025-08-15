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

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/render.h"
#include "engine/vec3.h"
#include "engine/utils.h"
#include "engine/world.h"
#include "level.h"
#include "objects/grenade.h"
#include "sound_atlas.h"
#include "sprite_atlas.h"

static void grenade_init(Actor *self, Render *rd) {
    self->type = ACT_T_GRENADE;

    self->is_solid = true;

    // Visuals
    self->billboard = malloc(sizeof(struct Billboard));
    self->billboard->height = GRENADE_SIZE;
    self->billboard->width = GRENADE_SIZE;
    self->billboard->spr = spr_get(SPR_WORMHOLE);

    // Collision
    self->collision = aabb_from_size(GRENADE_SIZE, GRENADE_SIZE);
    self->hitbox = self->collision;
}

static void carve_sphere(World *world, vec3 pos, float radius) {
    // Bounding box
    vec3 v_radius = (vec3){radius, radius, radius};
    vec3 min = v3_sub(pos, v_radius);
    vec3 max = v3_add(pos, v_radius);

    for (float z = min.z; z <= max.z; z++) {
        for (float y = min.y; y < max.y; y++) {
            for (float x = min.x; x < max.x; x++) {
                vec3 v = (vec3){x,y,z};
                float dist = v3_length(v3_sub(v, pos));
                if (dist <= radius) {
                    world_break(world, x, y, z);
                }
            }
        }
    }
}

static void grenade_explode(Actor *self, Render *rd) {
    carve_sphere(rd->world, self->pos, GRENADE_BLAST_RADIUS);
    audio_play(rd->as, sad_get_sound(SAD_EXPLOSION), 1.2f, randf(0.8f, 1.f), 0.f, false);

    // Hit + knockback surrounding actors
    for (size_t i = 0; i < rd->num_actors; i++) {
        Actor *act = rd->actors[i];

        // Skip inactive actors, null pointers, or this grenade
        if (!act || !act->is_active || act == self) {
            continue;
        }

        // Vector to actor
        vec3 v = v3_sub(act->pos, self->pos);
        // Square distance to actor
        float dist_2 = v3_len_2(v);

        // If the actor is close enough, hit it
        if (dist_2 < GRENADE_DAMAGE_RADIUS_2) {
            if (act->hit) act->hit(act, self, rd);

            // Apply knockback
            vec3 knockback = v3_muls(v3_norm(v), GRENADE_KNOCKBACK_FORCE);
            if (knockback.y > -GRENADE_KNOCKBACK_Y) knockback.y = -GRENADE_KNOCKBACK_Y;
            act->velocity = v3_add(act->velocity, knockback);
        }
    }

    BLAST_LIGHT(rd).intensity = GRENADE_BLAST_BRIGHTNESS;

    // Destroy this grenade
    self->destroy(self, rd);
}

static void grenade_update(Actor *self, Render *rd) {
    BLAST_LIGHT(rd).pos = self->pos;

    // Apply gravity (positive because Y is inverted)
    self->velocity.y += GRAVITY * GET_DELTATIME(rd);

    bool collided = actor_move(rd, self);

    if (collided) {
        grenade_explode(self, rd);
    }
}

static void grenade_draw(Actor *self, Render *rd) {
    self->billboard->pos = self->pos;
    self->billboard->pos.y -= GRENADE_SIZE / 2;
    render_billboard(self->billboard, rd);
}

ACTOR_NEW(
    grenade,
    GrenadeData,
    grenade_init,
    grenade_update,
    NULL,
    grenade_draw,
    actor_destroy
);
