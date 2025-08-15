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

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "engine/aabb.h"
#include "engine/actor.h"
#include "engine/render.h"
#include "engine/utils.h"
#include "engine/vec3.h"
#include "engine/vec3.h"
#include "engine/audio.h"
#include "engine/platform.h"
#include "engine/keys.h"
#include "flags.h"
#include "level.h"
#include "player.h"
#include "sound_atlas.h"

static void player_init(Actor *self, Render *rd) {
    // self->type = ACT_T_PLAYER;

    // Collision
    self->collision = aabb_from_size(PLAYER_WIDTH, PLAYER_HEIGHT);

    // Custom data
    PlayerData *d = self->data;
    // Start with invincibility (in case something tries to spawn kill us)
    d->inv_timer = PLAYER_INV_TIME;
    d->shoot_timer = 0;
    d->shake_timer = 0;
    d->view_accumulator = 0;
    d->mouse_sensitivity = LOOK_SENSITIVITY;
#if FEATURE_STEP_SOUNDS
    d->sfx_stepped_left = true;
#endif

    // Initialize camera : start looking towards +Z
    rd->cam.yaw = 0;
    rd->cam.pitch = 0;
    rd->cam.pitch_offset = 0;
    camera_update_vectors(&rd->cam);

    // Set light position (it's at the player's feet,
    // but the slight rise at spawn looks cool so let's keep it)
    PLAYER_LIGHT(rd).pos = self->pos;
}

static void player_update(Actor *self, Render *rd) {
    // Movement input
    bool key_forward = platform_key(rd, EKEY_Z) || platform_key(rd, EKEY_W) || platform_key(rd, EKEY_UP);
    bool key_backward = platform_key(rd, EKEY_S) || platform_key(rd, EKEY_DOWN);
    bool key_left = platform_key(rd, EKEY_Q) || platform_key(rd, EKEY_A) || platform_key(rd, EKEY_LEFT);
    bool key_right = platform_key(rd, EKEY_D) || platform_key(rd, EKEY_RIGHT);
    bool key_jump = platform_key(rd, EKEY_SPACE);
    bool sensibility_up = platform_key(rd, EKEY_O);
    bool sensibility_down = platform_key(rd, EKEY_L);

    PlayerData *d = (PlayerData *)self->data;

    // Update timers
    d->inv_timer -= GET_DELTATIME(rd);
    d->shoot_timer -= GET_DELTATIME(rd);
    d->shake_timer -= GET_DELTATIME(rd);
    // Update the movement of the gun
    if (v3_len_2(self->velocity) > 0.1f && self->on_ground) {
        d->view_accumulator += d->speed / 8.0f * GET_DELTATIME(rd);
    }

    // Darken muzzle flash light very fast
    if (PLAYER_LIGHT(rd).intensity > PLAYER_LIGHT_INTENSITY) {
        PLAYER_LIGHT(rd).intensity -= SHOOT_LIGHT_FALLOFF * GET_DELTATIME(rd);
    } else {
        PLAYER_LIGHT(rd).intensity = PLAYER_LIGHT_INTENSITY;
    }

    // Update camera pitch offset (return to center)
    rd->cam.pitch_offset = lerpf(
        rd->cam.pitch_offset, 0.0f,
        d->stats->shoot_recoil_recovery,
        GET_DELTATIME(rd)
    );

    // Update camera orientation vectors before calculating movement direction
    rd->cam.pitch += d->mouse_sensitivity * (float)(rd->mouse_delta_y);
    rd->cam.yaw += d->mouse_sensitivity * (float)(rd->mouse_delta_x);
    camera_update_vectors(&rd->cam);

    vec3 move_input = {0};
    if (key_forward) {
        move_input = v3_add(move_input, rd->cam.forward);
    }
    if (key_backward) {
        move_input = v3_sub(move_input, rd->cam.forward);
    }
    if (key_left) {
        move_input = v3_sub(move_input, rd->cam.right);
    }
    if (key_right) {
        move_input = v3_add(move_input, rd->cam.right);
    }

    // Increase/decrease sensibility
    static const float sens_step = 0.0001f;
    if (sensibility_down && d->mouse_sensitivity > 0.0001f) {
        d->mouse_sensitivity -= sens_step;
    } else if (sensibility_up) {
        d->mouse_sensitivity += sens_step;
    }

    bool is_jumping = self->on_ground && key_jump;

#if ENABLE_BUNNY_HOP
    // Handle bunny hopping, it should be reminded that bunny hopping was a bug of the
    // Quake engine, as we don't use the same method to compute the movement, try to
    // reproduce the bug as best as possible (Strafe-like behavior)
    if (key_forward && (key_left != key_right) && is_jumping) {
        if (d->speed < BUNNYHOP_MAXSPEED) {
            d->speed += BUNNYHOP_INCREMENT;
        } else {
            d->speed = BUNNYHOP_MAXSPEED;
        }
    }

    // If we are on the ground, gradually remove the speed boost
    if (self->on_ground)
    {
        if (d->speed > d->stats->move_speed) {
            d->speed -= BUNNYHOP_DECREMENT;
        } else {
            d->speed = d->stats->move_speed;
        }
    }
#endif

    // Normalize horizontal movement direction if there's input
    // (v3_norm already checks if the length is >0 so we're all good)
    move_input.y = 0;
    move_input = v3_norm(move_input);

    // Scale horizontal intent by move speed
    vec3 move_intent = v3_muls(move_input, d->speed);

    // Update velocity based on movement intent
    float smooth = self->on_ground ? MOVE_SMOOTH : MOVE_SMOOTH_AIR;
    vec3 move_vec = v3_lerp(self->velocity, move_intent, smooth, GET_DELTATIME(rd));

    self->velocity.x = move_vec.x;
    self->velocity.z = move_vec.z;

    // Apply gravity (positive because Y is inverted)
    if (!self->on_ground) {
        self->velocity.y += GRAVITY * GET_DELTATIME(rd);
    }

    // Jump
    if (is_jumping) {
        audio_play(rd->as, sad_get_sound(SAD_PLAYER_JUMP), 0.8f, randf(0.8f, 1.f), 0.f, false);
        self->velocity.y = -JUMP_VELOCITY;
    }

    // Move based on velocity
    actor_move(rd, self);

    // Shoot the gun
    if (MOUSE_GET_CLICK(rd, MOUSE_LEFT_CLICK) && d->shoot_timer < 0.0f) {
        player_shoot(self, rd);
    }

    // Launch a grenade
    if (
        MOUSE_GET_CLICK(rd, MOUSE_RIGHT_CLICK) &&
        d->shoot_timer < 0.0f &&
        d->stats->grenades > 0
    ) {
        player_launch_grenade(self, rd);
        d->stats->grenades--;
#if FEATURE_STATS
        d->stats->grenades_fired++;
#endif
    }

    // Camera position is based on player's base + eye height offset
    // Y is inverted bc ya fuckedup, so subtract eye height to move camera up
    rd->cam.pos = self->pos;
    rd->cam.pos.y -= PLAYER_EYE_HEIGHT;
    // Add screen shake offset
    if (d->shake_timer > 0) {
        float scale = SCREENSHAKE_SCALE * d->shake_timer;
        rd->cam.pos.y += sin(d->shake_timer * SCREENSHAKE_SPEED) * scale;
    }

    // Move light to follow player's view
    vec3 light_target_pos = v3_add(
        rd->cam.pos,
        v3_muls(rd->cam.forward, PLAYER_LIGHT_DISTANCE)
    );
    PLAYER_LIGHT(rd).pos = v3_lerp(
        PLAYER_LIGHT(rd).pos,
        light_target_pos,
        PLAYER_LIGHT_SMOOTH,
        GET_DELTATIME(rd)
    );
}

static void player_hit(Actor *self, Actor *hitter, Render *rd) {
    // Uncomment to get damaged by grenades
    // player_damage(self, 1);
}

bool player_damage(Actor *self, int dmg, Render *rd) {
    PlayerData *d = (PlayerData *)self->data;
    if (d->inv_timer > 0.0f) {
        return false;
    }

    // Set invincibility timer on hit
    d->inv_timer = PLAYER_INV_TIME;

    // Play sound
    audio_play(rd->as, sad_get_sound(SAD_PLAYER_HIT), 1, randf(0.9f, 1.f), 0.f, false);

    // Screen shake
    d->shake_timer = SCREENSHAKE_DURATION;

    d->stats->hp -= dmg;
    if (d->stats->hp <= 0) {
        // Player death
        self->is_active = false;
    }

#if FEATURE_STATS
    d->stats->damage_taken += dmg;
#endif

    return true;
}

ACTOR_NEW(
    player,
    PlayerData,
    player_init,
    player_update,
    NULL, // player_hit if needed
    NULL, // Player has no visuals (weapon is drawn with hud)
    actor_destroy
);
