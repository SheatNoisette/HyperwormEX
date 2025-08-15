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
#include <stdlib.h>

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/render.h"
#include "engine/vec3.h"
#include "engine/utils.h"
#include "objects/grenade.h"
#include "player.h"
#include "sound_atlas.h"
#include "sprite_atlas.h"

#include "enemies/basic.h"

static void basic_enemy_init(Actor *self, Render *rd) {
    // self->type = ACT_T_ENEMY;

    // Enemies should not overlap
    self->is_solid = true;

    float width = ENEMY_WIDTH;
    float height = ENEMY_HEIGHT;
    int hp = ENEMY_HP;
    float speed = ENEMY_SPEED;
    Sprite *sprite = spr_get(SPR_WORM);
    int drop = ENEMY_CREDITS;

    // BEEG enemy
    if (rand() % 6 == 0) {
        width *= ENEMY_BIG_SCALE;
        height *= ENEMY_BIG_SCALE;
        hp = ENEMY_HP_BIG;
        speed = ENEMY_SPEED_BIG;
        sprite = spr_get(SPR_WORM_2);
        drop = ENEMY_CREDITS_BIG;
    }

    // Visuals
    self->billboard = malloc(sizeof(struct Billboard));
    self->billboard->height = height;
    self->billboard->width = (coinflip() == 0) ? width : -width;
    self->billboard->spr = sprite;

    // Collision
    self->collision = aabb_from_size(width, height);
    self->hitbox = self->collision;

    // Destroy blocks on the enemy  - This is the smallest way to do that
    const float pos_x = floorf(self->pos.x);
    const float pos_y = floorf(self->pos.y);
    const float pos_z = floorf(self->pos.z);
    world_break(rd->world, pos_x, pos_y, pos_z);
    world_break(rd->world, pos_x, pos_y - 1, pos_z);
    world_break(rd->world, pos_x, pos_y - 2, pos_z);

    // Custom data
    BasicEnemyStorage *d = self->data;
    d->is_asleep = true; // start off inactive
    d->is_dead = false; // obviously
    d->max_hp = hp;
    d->hp = hp;
    d->speed = speed;
    d->money_drop = drop;
    d->damage = 1;
}

static void basic_enemy_update(Actor *self, Render *rd) {
    BasicEnemyStorage *d = self->data;

    // Animate death and return
    if (d->is_dead) {
        float anim_speed = 12 * GET_DELTATIME(rd);
        if (fabsf(self->billboard->height) < anim_speed) {
            self->is_active = false;
            self->destroy(self, rd);
        } else {
            self->billboard->height -= anim_speed;
        }

        return;
    }

#if SAFE_C
    if (d->player == NULL) {
        return;
    }
#endif

    // Apply gravity (positive because Y is inverted)
    if (!self->on_ground) {
        self->velocity.y += GRAVITY * GET_DELTATIME(rd);
    }

    // Move towards target
    // Vector to target
    vec3 v = v3_sub(d->player->pos, self->pos);
    // Save distance^2 for later (epic foreshadowing)
    float dist_2 = v3_len_2(v);

    // Detect player, WAKE UP WAKE UP WAKE UP WAKE UP WAKE UP
    if (dist_2 <= DETECT_RANGE_2) {
        d->is_asleep = false;
    }

    if (!d->is_asleep) {
        // Move towards player
        vec3 move_norm = v3_norm((vec3){v.x, 0, v.z});
        vec3 move_vec = v3_muls(move_norm, d->speed);

        // Smooth movement
        float smooth = self->on_ground ? ENEMY_SMOOTH : ENEMY_SMOOTH_AIR;
        move_vec = v3_lerp(self->velocity, move_vec, smooth, GET_DELTATIME(rd));

        self->velocity.x = move_vec.x;
        self->velocity.z = move_vec.z;

        // Lower than player: jump
        if (self->on_ground && d->player->on_ground && v.y < -0.5f) {
            self->velocity.y = -ENEMY_JUMP_VELOCITY;
        }

        // Melee attack
        if (dist_2 < ENEMY_HIT_RANGE_2) {
            bool success = player_damage(d->player, d->damage, rd);

            if (success) {
                // Stop moving
                self->velocity.x = 0;
                self->velocity.z = 0;

                // Apply knockback
                d->player->velocity = v3_muls(move_norm, ENEMY_KNOCKBACK_FORCE);
                // A bit in the air too
                d->player->velocity.y = -ENEMY_KNOCKBACK_FORCE/2;
            }
        }
    }

    actor_move(rd, self);
}

static void basic_enemy_draw(Actor *self, Render *rd) {
    self->billboard->pos = self->pos;
    self->billboard->pos.y -= self->billboard->height / 2;
    render_billboard(self->billboard, rd);
}

static void basic_enemy_damage(Actor *self, Render *rd, int dmg) {
    BasicEnemyStorage *d = self->data;

    // Already dead
    // お前はもう死んでいる
    if (!self->is_active || d->is_dead) {
        return;
    }

    // Wake up when damaged
    d->is_asleep = false;

    d->hp -= dmg;
    self->billboard->width *= -1; // flip billboard :)
    audio_play(rd->as, sad_get_sound(SAD_ENEMY_HIT), 1.2f, randf(0.8f, 1.2f), 0, false);

    // Death
    if (d->hp <= 0) {
        d->is_dead = true;
        PlayerData *pd = ACTOR_DATA(d->player, PlayerData);
        pd->stats->money += d->money_drop;
#if FEATURE_STATS
        pd->stats->money_collected += d->money_drop;
        pd->stats->enemies_killed++;
#endif
    }
}

static void basic_enemy_hit(Actor *self, Actor *hitter, Render *rd) {
    int dmg = 1;
    if (hitter->type == ACT_T_GRENADE) {
        dmg = 5;
    }
    basic_enemy_damage(self, rd, dmg);
}

ACTOR_NEW(
    basic_enemy,
    BasicEnemyStorage,
    basic_enemy_init,
    basic_enemy_update,
    basic_enemy_hit,
    basic_enemy_draw,
    actor_destroy
);
