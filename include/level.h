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

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <stddef.h>
#include <stdint.h>

#include "engine/aabb.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "engine/vec3.h"
#include "engine/world.h"
#include "game.h"
#include "player.h"

// These values are for indoor room generation
#define MAP_SIZE 80
#define MAP_HEIGHT 32
// Room sizes (left + top walls included)
// Max should be larger than min (at least double) for algorithm to work
#define ROOM_MIN 8
#define ROOM_MAX 24
// Floor layer
#define FLOOR_Y 20
// Hall ceiling layer (inclusive)
#define HALL_Y 17 // 4 blocks high (17,18,19,20)
// Room height
#define ROOM_H_MIN 5
#define ROOM_H_MAX 10
// Padding on the sides of the map
#define MAP_PADDING 2
// Maximum obstacles to spawn in room
#define MAX_OBSTACLES 2
// Max number of basic enemies per room
#define MAX_SPAWN_BASIC 4

// Global lights
#define PLAYER_LIGHT(rd) (rd->lights[0])
#define BLAST_LIGHT(rd) (rd->lights[1])
// Blast light falloff speed
#define BLAST_LIGHT_FALLOFF 20.0f

// A node in a binary tree
// Leaf = room
typedef struct {
    // Left child (NULL if leaf)
    void *child_a;
    // Right child (NULL if leaf)
    void *child_b;
    // Is split along X axis (otherwise Z)
    bool is_horizontal;
    // Bounding box
    AABB bb;
} BspRoom;

// Data used during room generation
typedef struct GenData {
    BspRoom *room_tree;
    BspRoom *player_room;
    BspRoom *wormhole_room;
} GenData;

GenData generate_rooms(Render *rd);
void level_spawn_actors(Game *g, GenData *gen);

// BSP
BspRoom *new_room(AABB bb);
void free_rooms(BspRoom *tree);
vec3 room_random_pos(BspRoom *room);

// Gamestate functions
void init_level(Game *g);
GameState update_level(Game *g);
void exit_level(Game *g);

#endif
