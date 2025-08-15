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

#include "engine/render.h"
#include "engine/utils.h"
#include "enemies/basic.h"
#include "level.h"
#include "player.h"
#include "wormhole.h"

void populate_room(Game *g, BspRoom *room) {
    int basic_enemy_count = randi(0, MAX_SPAWN_BASIC+1);
    for (int i = 0; i < basic_enemy_count; i++) {
        vec3 pos = room_random_pos(room);
        Actor *e = ACTOR_SPAWN(g->rd, basic_enemy, pos);

        if (e == NULL) {
            continue;
        }

        BasicEnemyStorage *d = ACTOR_DATA(e, BasicEnemyStorage);
        d->player = g->player;

        // Level scaling
        d->speed += 0.5f * g->round;
        d->money_drop += (g->round - 1);
        d->damage += g->round / 4;
    }
}

void spawn_enemies_recursive(Game *g, GenData *gen, BspRoom *room) {
    // Skip player and wormhole rooms
    if (room == gen->player_room || room == gen->wormhole_room) {
        return;
    }

    // This is a leaf, aka an actual room
    if (room->child_a == NULL) {
        populate_room(g, room);
        return;
    }

    spawn_enemies_recursive(g, gen, room->child_a);
    spawn_enemies_recursive(g, gen, room->child_b);
}

void level_spawn_actors(Game *g, GenData *gen) {
    // Spawn the player (and save it in Game)
    g->player = ACTOR_SPAWN(g->rd, player, room_random_pos(gen->player_room));
    player_load_stats(g->player, g->player_stats);

    // Spawn the wormhole (and save it in Game)
    g->wormhole = ACTOR_SPAWN(g->rd, wormhole, room_random_pos(gen->wormhole_room));
    ACTOR_DATA(g->wormhole, WormholeData)->player = g->player;

    spawn_enemies_recursive(g, gen, gen->room_tree);
}
