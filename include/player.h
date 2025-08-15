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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "engine/aabb.h"
#include "engine/actor.h"
#include "engine/render.h"
#include "engine/vec3.h"
#include "flags.h"

#define ACT_T_PLAYER 0

typedef struct PlayerStats {
    int max_hp; // health points
    float move_speed; // maximum walking speed
    float shoot_speed; // time between shots
    float grenade_speed; // time between grenades
    float shoot_recoil; // amount of recoil
    float shoot_recoil_recovery; // recoil recovery speed
    int block_destroy_chance; // 1/x chance to break block on shoot

    int hp;
    int money;
    int grenades;

    // Win condition (checked in upgrade screen)
    bool won;

#if FEATURE_STATS
    // Stats for ending screen
    int money_collected;
    int enemies_killed;
    int damage_taken;
    int grenades_fired;
#endif
} PlayerStats;

typedef struct PlayerData {
    // --- Stats
    PlayerStats *stats;

    // --- Variables
    float speed;

    // --- Timers
    // Invincibility
    float inv_timer;
    // Shooting
    float shoot_timer;
    // View bobbing acculumator
    float view_accumulator;
    // Screen shake
    float shake_timer;
    // Player mouse sensitivity
    float mouse_sensitivity;

#if FEATURE_STEP_SOUNDS
    // --- SFX
    bool sfx_stepped_left;
#endif
} PlayerData;

// Function exports
ACTOR_DECLARE(player)

// Load player stats into player actor
void player_load_stats(Actor *self, PlayerStats *stats);
// Initialize player stats with default values for level 1
void player_reset_stats(PlayerStats *stats);
// Attempt to damage the player
// Return whether it was successful
bool player_damage(Actor *self, int dmg, Render *rd);
// Draw gun and crosshair
void player_draw_gun(Actor *self, Render *rd);
// Draw hud
void player_draw_hud(PlayerStats *stats, Render *rd);

#if FEATURE_RADAR
// Draw radar towards actor
void player_draw_radar(Actor *self, Actor *target, Render *rd);
#endif

// Weapons
// Shoot the gun
void player_shoot(Actor *self, Render *rd);
// Launch a grenade
void player_launch_grenade(Actor *self, Render *rd);

// Player height we are a CHAD of 1m80
#define PLAYER_HEIGHT 1.8f
// Player width
#define PLAYER_WIDTH 0.6f
// Camera height relative to player base
#define PLAYER_EYE_HEIGHT 1.6f
// Initial upward velocity
#define JUMP_VELOCITY 8.0f
// Movement smoothing on ground
#define MOVE_SMOOTH 28.0f
// Movement smoothing in air (higher means more air control)
#define MOVE_SMOOTH_AIR 2.0f
// Enable bunny hop
#define ENABLE_BUNNY_HOP 1
// Bunny hop maximum acceptable speed
#define BUNNYHOP_MAXSPEED 25.f
// Boost of bunny hop
#define BUNNYHOP_INCREMENT 1.2f
// Remove speed of bunnyhopping
#define BUNNYHOP_DECREMENT 0.3f
// Mouse sensitivity
#define LOOK_SENSITIVITY 0.0015f
// Time after hit during which the player is invincible
#define PLAYER_INV_TIME 1.0f

// Lights
// Smoothing of light movement following the player (higher is snappier)
#define PLAYER_LIGHT_SMOOTH 10.0f
// Distance of light in front of player (slightly less than width/2 to prevent
// clipping into walls)
#define PLAYER_LIGHT_DISTANCE 0.29f
// Base light intensity
#define PLAYER_LIGHT_INTENSITY 0.8f
// Muzzle flash light intensity
#define SHOOT_LIGHT_INTENSITY 1.1f
// Muzzle flash light falloff speed
#define SHOOT_LIGHT_FALLOFF 16.0f

// HUD
// Main hud color
#define HUD_COLOR 0x60B0FF
// HP color
#define HUD_HP_COLOR 0x0000FF
// Money color
#define HUD_MONEY_COLOR 0xFFDDBB
// Hud background color
#define HUD_BG_COLOR 0x202020
// Padding from the edge of the screen
#define HUD_PADDING 8
// Speed of gun swaying when walking
#define GUN_SWAY_SPEED 16.0f

// Screen shake
#define SCREENSHAKE_DURATION 0.3f
#define SCREENSHAKE_SPEED 150
#define SCREENSHAKE_SCALE 2

#endif
