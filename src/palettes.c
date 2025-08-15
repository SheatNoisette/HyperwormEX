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

#include "engine/logging.h"
#include "engine/lut.h"
#include <stddef.h>
#include <stdint.h>

#define palette_count 6
#define palette_size 8

static const uint32_t palettes[palette_count][palette_size] = {
    {
        // https://lospec.com/palette-list/slso8
        0x0d2b45, 0x203c56, 0x544e68, 0x8d697a,
        0xd08159, 0xffaa5e, 0xffd4a3, 0xffecd6,
    },
    {
        // https://lospec.com/palette-list/minty-8 modified
        0x190b26, 0x2d1860, 0x3d6ac9, 0x56d3ac,
        0xf3ead6, 0xf1d869, 0xe87a48, 0xc2313f,
    },
    {
        // https://lospec.com/palette-list/darkroom modified
        0xb00005, 0x921315, 0x7c1023, 0x641224,
        0x471024, 0x2b0b1d, 0x000000, 0xffeeee,
    },
    {
        // https://lospec.com/palette-list/rust-gold-8
        0xf6cd26, 0xac6b26, 0x563226, 0x331c17,
        0xbb7f57, 0x725956, 0x393939, 0x202020,
    },
    {
        // https://lospec.com/palette-list/paper-8 modified
        0x1f244b, 0x553043, 0xa8604d, 0xd1a67e,
        0xf6e79c, 0xb6cf8e, 0x60ae7b, 0x3c6b64,
    },
    {
        // custom creepy one
        0x002000, 0x207050, 0x908090, 0xffffff,
    },
};

static size_t next_index = 0;

void change_palette() {
    ENGINE_LOG("[GAME] Building palette LUT...\n");
    lut_build(palettes[next_index], palette_size);
    ENGINE_LOG("[GAME] Done!\n");

    next_index = (next_index + 1) % palette_count;
}
