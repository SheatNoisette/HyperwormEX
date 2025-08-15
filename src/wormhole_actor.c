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
#include "engine/render.h"
#include "engine/vec3.h"
#include "sprite_atlas.h"
#include "wormhole.h"

static void wormhole_init(Actor *self, Render *rd) {
    self->billboard = malloc(sizeof(struct Billboard));
    self->billboard->height = WORMHOLE_SIZE;
    self->billboard->width = WORMHOLE_SIZE;
    self->billboard->spr = spr_get(SPR_WORMHOLE);

    // Set position (it won't change)
    self->billboard->pos = self->pos;
    self->billboard->pos.y -= WORMHOLE_SIZE / 2;

    // Custom data
    WormholeData *d = self->data;
    d->is_triggered = false;
}

static void wormhole_update(Actor *self, Render *rd) {
    WormholeData *d = (WormholeData *)self->data;

    // Check player collision
    // Vector to player
    vec3 v = v3_sub(d->player->pos, self->pos);
    // Square distance between actors
    float dist_2 = v3_len_2(v);
    if (dist_2 <= WORMHOLE_DIST_2) {
        d->is_triggered = true;
    }
}

static void wormhole_draw(Actor *self, Render *rd) {
    // Who the hell is Bill and why is he bored
    render_billboard(self->billboard, rd);
}

ACTOR_NEW(
    wormhole,
    WormholeData,
    wormhole_init,
    wormhole_update,
    NULL, // Can't be hit
    wormhole_draw,
    actor_destroy
);
