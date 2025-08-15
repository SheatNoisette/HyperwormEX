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

#include <string.h>

#include "engine/aabb.h"
#include "engine/utils.h"
#include "engine/world.h"
#include "flags.h"
#include "level.h"
#include "textures.h"

// Make a rectangular region a certain tile (inclusive bounds)
void set_region(
    World *world,
    int min_x, int max_x,
    int min_y, int max_y,
    int min_z, int max_z,
    uint8_t type
) {
    for (int x = min_x; x <= max_x; x++) {
        for (int z = min_z; z <= max_z; z++) {
            for (int y = min_y; y <= max_y; y++) {
                world_set(world, x, y, z, type);
            }
        }
    }
}

// Clear out an area of the world based on an AABB
void carve_aabb(World *world, AABB bb) {
    set_region(
        world,
        bb.min_x, bb.max_x,
        bb.min_y, bb.max_y,
        bb.min_z, bb.max_z,
        0 // air
    );
}

// Carve a tunnel until an air block is reached
void carve_tunnel(World *world, int start_x, int start_z, int dx, int dz) {
    int x = start_x;
    int z = start_z;

    // Extra tunnel width
    int x_w = 1 - (dx*dx);
    int z_w = 1 - (dz*dz);

    // Do while because we might start inside a room,
    // which would end the loop instantly :(
    do {
        set_region(world, x-x_w, x+x_w, HALL_Y, FLOOR_Y, z-z_w, z+z_w, 0);
        x += dx;
        z += dz;
    } while (world_get(world, x, FLOOR_Y, z) > TEX_BARRIER);
}

void carve_tunnel_x(World *world, int x1, int x2, int z) {
    set_region(world, x1, x2, HALL_Y, FLOOR_Y, z-1, z+1, 0);
}

void carve_tunnel_z(World *world, int x, int z1, int z2) {
    set_region(world, x-1, x+1, HALL_Y, FLOOR_Y, z1, z2, 0);
}

// Connect children of a node with a tunnel
// Also updates dimensions for the next level up
void connect_children(World *world, BspRoom *node) {
    BspRoom *a = node->child_a;
    BspRoom *b = node->child_b;

    // Update dimensions for parent
    node->bb.min_x = min(a->bb.min_x, b->bb.min_x);
    node->bb.min_z = min(a->bb.min_z, b->bb.min_z);
    node->bb.max_x = max(a->bb.max_x, b->bb.max_x);
    node->bb.max_z = max(a->bb.max_z, b->bb.max_z);

    // Horizontal split : children are above and below (+-Z)
    if (node->is_horizontal) {
        // If not aligned, make an S tunnel
        if (a->bb.max_x < b->bb.min_x || b->bb.max_x < a->bb.min_x) {
            int x1 = randi(a->bb.min_x+1, a->bb.max_x);
            int x2 = randi(b->bb.min_x+1, b->bb.max_x);
            int z = randi(a->bb.max_z, b->bb.min_z+1);
            carve_tunnel(world, x1, z, 0, -1);
            carve_tunnel(world, x2, z, 0, 1);
            carve_tunnel_x(world, x1, x2, z);
        } else {
            // Otherwise, make a straight line
            int x_min = max(a->bb.min_x, b->bb.min_x);
            int x_max = min(a->bb.max_x, b->bb.max_x);
            int x = randi(x_min+1, x_max);
            carve_tunnel(world, x, b->bb.min_z, 0, -1);
            carve_tunnel(world, x, b->bb.min_z+1, 0, 1);
        }
    } else {
        // If not aligned, make an S tunnel
        if (a->bb.max_z < b->bb.min_z || b->bb.max_z < a->bb.min_z) {
            int z1 = randi(a->bb.min_z+1, a->bb.max_z);
            int z2 = randi(b->bb.min_z+1, b->bb.max_z);
            int x = randi(a->bb.max_x, b->bb.min_x+1);
            carve_tunnel(world, x, z1, -1, 0);
            carve_tunnel(world, x, z2, 1, 0);
            carve_tunnel_z(world, x, z1, z2);
        } else {
            // Otherwise, make a straight line
            int z_min = max(a->bb.min_z, b->bb.min_z);
            int z_max = min(a->bb.max_z, b->bb.max_z);
            int z = randi(z_min+1, z_max);
            carve_tunnel(world, b->bb.min_x, z, -1, 0);
            carve_tunnel(world, b->bb.min_x+1, z, 1, 0);
        }
    }
}

// Carve a room within a node
// is_leftmost : the leftmost node in the tree is the player room,
// so don't add crates in there
void carve_room(World *world, BspRoom *node, int node_width, int node_depth, int add_crates) {
    // Room dimensions are between ROOM_MIN and node dimensions
    int room_width = randi(ROOM_MIN, node_width + 1);
    int room_depth = randi(ROOM_MIN, node_depth + 1);
    int dx = randi(0, node_width - room_width + 1);
    int dz = randi(0, node_depth - room_depth + 1);
    int height = randi(ROOM_H_MIN, ROOM_H_MAX + 1);

    // Update bounding box to room dimensions
    node->bb.min_x += dx;
    node->bb.min_z += dz;
    node->bb.max_x = node->bb.min_x + room_width - 1;
    node->bb.max_z = node->bb.min_z + room_depth - 1;
    node->bb.min_y = node->bb.max_y - height + 1;

    // Carve out the room
    carve_aabb(world, node->bb);

#if FEATURE_CRATES
    int crates = randi(0, (room_width+room_depth)/ROOM_MIN+2) * add_crates;
    for (int i = 0; i < crates; i++) {
        int x = randi(node->bb.min_x, node->bb.max_x);
        int z = randi(node->bb.min_z, node->bb.max_z);
        int y = randi(0, 3);
        set_region(world, x, x+coinflip(), FLOOR_Y-y, FLOOR_Y, z, z+coinflip(), TEX_CRATE);
    }
#endif
}

// Return a random split position between min and max
// Keep enough space for a room on both sides
int random_split(int min, int max) {
    int split_min = min + ROOM_MIN;
    int split_max = max - ROOM_MIN;
    return randi(split_min, split_max + 1);
}

// Recursive Binary Space Partitioning (BSP)
void split_bsp(World *world, GenData *gen, BspRoom *node, int left, int right) {
    int width = node->bb.max_x - node->bb.min_x + 1;
    int depth = node->bb.max_z - node->bb.min_z + 1;

    // Width and depth are small enough, stop here
    // This node is a leaf, generate a room
    if (width <= ROOM_MAX && depth <= ROOM_MAX) {
        carve_room(world, node, width, depth, !(left | right));
        if (left)
            gen->player_room = node;
        else if (right)
            gen->wormhole_room = node;
        return;
    }

    // Choose split direction
    //  - If width is already small enough, split along Z
    //  - If depth is already small enough, split along X
    //  - If width and depth are still large, pick randomly
    bool is_x = (width > ROOM_MAX) && (depth <= ROOM_MAX || coinflip());

    // Calculate child node dimensions
    AABB bb_a = node->bb;
    AABB bb_b = node->bb;
    int split;
    if (is_x) {
        split = random_split(node->bb.min_x, node->bb.max_x);
        bb_a.max_x = split - 1;
        bb_b.min_x = split + 1;
    } else {
        split = random_split(node->bb.min_z, node->bb.max_z);
        bb_a.max_z = split - 1;
        bb_b.min_z = split + 1;
    }

    // Save split axis (for tunnels)
    // Horizontal : split is along Z axis
    node->is_horizontal = !is_x;
    // Create child nodes
    node->child_a = new_room(bb_a);
    node->child_b = new_room(bb_b);
    // Recursively split children
    split_bsp(world, gen, node->child_a, left, 0);
    split_bsp(world, gen, node->child_b, 0, right);

    // Connect children with tunnel
    connect_children(world, node);
}

// Generate an indoor rooms level
GenData generate_rooms(Render *rd) {
    // Create a new world with desired size
    World *world = new_world(MAP_SIZE, MAP_HEIGHT, MAP_SIZE);
    rd->world = world;

    // Fill with barriers
    memset(world->data, TEX_BARRIER, world->nb * sizeof(uint8_t));

    for (int x = 1; x < world->width-1; x++) {
        for (int z = 1; z < world->depth-1; z++) {
            // Stone layer
            int y = world->height-2;
            for (; y > FLOOR_Y+1; y--) {
                world_set(world, x, y, z, TEX_ROCKS);
            }
            // Floor layer
            world_set(world, x, y--, z, TEX_TILES);
            // Walls layer
            for (; y > HALL_Y; y--) {
                int tex = y % 2 ? TEX_ROCKS : TEX_STEEL;
                world_set(world, x, y, z, tex);
            }
            // Ceiling layer
            for (; y > 0; y--) {
                int chance = randi(0, 3);
                world_set(world, x, y, z, chance ? TEX_BRICK : TEX_TILES);
            }
        }
    }

    GenData gen;
    // Create the root node spanning the entire world (minus padding)
    gen.room_tree = new_room((AABB){
        // Leave room for outer walls
        .min_x = 1 + MAP_PADDING,
        .max_x = world->width - 2 - MAP_PADDING,
        .min_y = 1,
        .max_y = FLOOR_Y,
        .min_z = 1 + MAP_PADDING,
        .max_z = world->depth - 2 - MAP_PADDING,
    });

    // Recursively split into smaller rooms
    split_bsp(world, &gen, gen.room_tree, 1, 1);

    return gen;
}
