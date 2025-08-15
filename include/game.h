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

#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>

#include "engine/render.h"
#include "player.h"

typedef struct Game {
    Render *rd;
    int round;
    Actor *player;
    PlayerStats *player_stats;
    Actor *wormhole;
} Game;

typedef enum {
    GS_INIT, // Empty state for startup

    GS_MENU, // Main menu
    GS_NEWGAME, // Starting a new game
    GS_LEVEL, // In-game (level/round)
    GS_WORMHOLE, // Upgrades in between rounds
    GS_LOSE, // Player death
    GS_END, // Game end

    GS_EXIT, // Empty state for exit

    STATE_COUNT // Count value
} GameState;

// New game
void init_newgame(Game *g);
GameState update_newgame(Game *g);

// Lose state
void init_lose(Game *g);
GameState update_lose(Game *g);

// End/win state
void init_end(Game *g);
GameState update_end(Game *g);

#endif
