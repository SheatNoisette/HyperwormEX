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

#ifndef __BASIC_ENEMY_H__
#define __BASIC_ENEMY_H__

#include "engine/actor.h"

#define ACT_T_ENEMY 1

// Dimensions (hitbox, collision and billboard)
#define ENEMY_HEIGHT 1.5f
#define ENEMY_WIDTH 0.8f
#define ENEMY_BIG_SCALE 2
// HP
#define ENEMY_HP 3
#define ENEMY_HP_BIG 15
// Base movement speed (scales)
#define ENEMY_SPEED 5.0f
#define ENEMY_SPEED_BIG 2.0f
// Jump force
#define ENEMY_JUMP_VELOCITY 7.0f
// Movement smoothing
#define ENEMY_SMOOTH 8.0f
#define ENEMY_SMOOTH_AIR 1.0f
// Distance (squared) to hit target
#define ENEMY_HIT_RANGE_2 0.3f
// Player knockback
#define ENEMY_KNOCKBACK_FORCE 8.0f
// Base credits awarded for killing (scales)
#define ENEMY_CREDITS 10
#define ENEMY_CREDITS_BIG 50

// Distance (squared) from player to become active
#define DETECT_RANGE_2 324 // 18 blocks

typedef struct BasicEnemyStorage {
    Actor *player;
    bool is_asleep;
    bool is_dead;
    int max_hp;
    int hp;
    float speed;
    int money_drop;
    int damage;
} BasicEnemyStorage;

// Function exports
ACTOR_DECLARE(basic_enemy)

#endif
