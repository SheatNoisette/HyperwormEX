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

#include <stdlib.h>

#include "engine/utils.h"
#include "level.h"

BspRoom *new_room(AABB bb) {
    BspRoom *r = malloc(sizeof(BspRoom));
    r->bb = bb;
    r->child_a = NULL;
    r->child_b = NULL;
    return r;
}

void free_rooms(BspRoom *tree) {
    if (tree == NULL) {
        return;
    }

    free_rooms(tree->child_a);
    free_rooms(tree->child_b);
    free(tree);
}

vec3 room_random_pos(BspRoom *room) {
    AABB bb = room->bb;
    return (vec3){
        .x = bb.min_x + randi(1, bb.max_x - bb.min_x) + 0.5f,
        .y = bb.max_y,
        .z = bb.min_z + randi(1, bb.max_z - bb.min_z) + 0.5f,
    };
}
