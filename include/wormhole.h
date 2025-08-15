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

#ifndef __WORMHOLE_H__
#define __WORMHOLE_H__

#include "engine/actor.h"
#include "game.h"

// Wormhole gamestate
void init_wormhole(Game *g);
GameState update_wormhole(Game *g);
void exit_wormhole(Game *g);

// Wormhole actor
typedef struct WormholeData {
    // Player for collision
    Actor *player;
    // Set to true when the player collides
    // funny variable name lol haha xd
    bool is_triggered;
} WormholeData;

ACTOR_DECLARE(wormhole)

// Wormhole height
#define WORMHOLE_SIZE 4.0f
// Distance squared from player to trigger wormhole
#define WORMHOLE_DIST_2 1.5f

#endif
