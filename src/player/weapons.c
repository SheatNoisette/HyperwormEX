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

#include "engine/vec3.h"
#include "engine/utils.h"
#include "level.h"
#include "objects/grenade.h"
#include "player.h"
#include "sound_atlas.h"

static void shoot_common(Actor *self, Render *rd) {
    PlayerData *d = (PlayerData *)self->data;

    // Recoil
    rd->cam.pitch_offset += -d->stats->shoot_recoil;

    // Light effect
    PLAYER_LIGHT(rd).intensity = SHOOT_LIGHT_INTENSITY;
}

// Shoot a gun, america fuck yeah
void player_shoot(Actor *self, Render *rd) {
    PlayerData *d = (PlayerData *)self->data;

    // Shoot a ray
    RaycastResult rt = actor_perform_raycast(rd, self, rd->cam.pos, rd->cam.forward);

    // Reset shoot timer
    d->shoot_timer = d->stats->shoot_speed;

    // Destroy block
    if (rand() % d->stats->block_destroy_chance == 0) {
        world_break(rd->world, rt.hit.x, rt.hit.y, rt.hit.z);
    }

    // Common effects
    shoot_common(self, rd);

    // Play sound
    audio_play(rd->as, sad_get_sound(SAD_PLAYER_SHOOT), 1.2f, randf(0.8f, 1.f), 0.f, false);
}

// GRENAAAADAAAAA
void player_launch_grenade(Actor *self, Render *rd) {
    PlayerData *d = (PlayerData *)self->data;

    // Spawn the grenade
    vec3 grenade_pos = v3_add(self->pos, rd->cam.forward);
    grenade_pos.y -= 1.0f;
    Actor *g = ACTOR_SPAWN(rd, grenade, grenade_pos);
    vec3 direction = v3_add(rd->cam.forward, (vec3){.x=0, .y=-0.25, .z=0});
    g->velocity = v3_muls(direction, GRENADE_LAUNCH_FORCE);

    // Reset shoot timer
    d->shoot_timer = d->stats->grenade_speed;

    // Common effects
    shoot_common(self, rd);

    // Play sound
    audio_play(rd->as, sad_get_sound(SAD_GRENADE_THROW), 1.3f, randf(0.8f, 1.f), 0.f, false);
}
