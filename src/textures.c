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

#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"
#include "engine/utils.h"
#include "flags.h"
#include "textures.h"

static void set_pixel(
    Render *rd, size_t index, size_t coord,
    uint8_t r, uint8_t g, uint8_t b
) {
    rd->textures[index][coord] = RGBA2U32(r, g, b, 0xff);
}

// Load textures for voxel indexes
void load_voxel_textures(Render *rd) {
    ENGINE_LOG("[GAME] Generating block textures...\n");
    rd->textures = malloc(sizeof(uint32_t *) * (TEXTURES_NUMBER+1));
    // 0 is air - start at 1
    for (size_t texture = 1; texture <= TEXTURES_NUMBER; texture++) {
        rd->textures[texture] = malloc(sizeof(uint32_t) * TEX_SIZE * TEX_SIZE);
    }

    for (size_t y = 0; y < TEX_SIZE; y++) {
        for (size_t x = 0; x < TEX_SIZE; x++) {
            size_t i = x + TEX_SIZE * y; // pixel index
            float n = randi(0, TEXTURE_NOISE_AMOUNT); // random noise to add
            const int line_width = 4;

            // Barrier
            set_pixel(rd, TEX_BARRIER, i, 50+n*2, 40+n, 60+n*2);

            // Rocks
            int x2 = ((x+8) % 24);
            int y2 = (y % 32);
            uint8_t c = x2*x2 - y2*y2;
            c /= 3;
            set_pixel(rd, TEX_ROCKS, i, 40+c+n, 60+c+n*3, 50+n*2);

            // Bricks
            const int brick_width = 32;
            const int brick_height = 16;
            if (
                (x + y / brick_height * brick_height) % brick_width < line_width ||
                y % brick_height < line_width
            ) {
                set_pixel(rd, TEX_BRICK, i, 70+n, 70+n*2, 80+n);
            } else {
                set_pixel(rd, TEX_BRICK, i, 170+n, 80+n, 60+n);
            }

            // Steel
            if (y <= line_width || y >= TEX_SIZE-line_width || x < 1) {
                set_pixel(rd, TEX_STEEL, i, 40+n, 40+n, 40+n);
            } else {
                set_pixel(rd, TEX_STEEL, i, 40+n, 100, 255-(y*15));
            }

            // Tiles
            const int tile_size = TEX_SIZE/2;
            c = 80+n*2;
            if (x/tile_size == y/tile_size && y < x) {
                c += 16;
            }
            set_pixel(rd, TEX_TILES, i, c, c, c);

#if FEATURE_CRATES
            // Crate
            const int crate_plank_height = 10;
            if (
                y % crate_plank_height < line_width ||
                x % brick_width < line_width
            ) {
                set_pixel(rd, TEX_CRATE, i, c, c, c);
            } else {
                set_pixel(rd, TEX_CRATE, i, 200+n, 200, 80+n);
            }
#endif
        }
    }
    ENGINE_LOG("[GAME] Done.\n");
}
