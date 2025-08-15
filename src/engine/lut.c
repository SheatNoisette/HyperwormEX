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

#include <stddef.h>
#include <stdint.h>

#include "engine/utils.h"
#include "engine/lut.h"

// R -> G -> B
// Main LUT applied everywhere
static uint32_t system_lut[LUT_COLORS_R][LUT_COLORS_G][LUT_COLORS_B] = { 0 };

uint32_t lut_convert_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return system_lut[r][g][b];
}

void lut_reset() {
    for (size_t r = 0; r < LUT_COLORS_R; r++) {
        for (size_t g = 0; g < LUT_COLORS_G; g++) {
            for (size_t b = 0; b < LUT_COLORS_B; b++) {
                system_lut[r][g][b] = RGBA2U32(r, g, b, 0xff);
            }
        }
    }
}

void lut_build(const uint32_t *colors, size_t nb_colors) {
    for (size_t r = 0; r < LUT_COLORS_R; r++) {
        for (size_t g = 0; g < LUT_COLORS_G; g++) {
            for (size_t b = 0; b < LUT_COLORS_B; b++) {
                uint8_t current_r = 0;
                uint8_t current_g = 0;
                uint8_t current_b = 0;

                size_t match_color_pos = 0;
                // useless sqrt
                size_t match_distance_min = SIZE_MAX;
                for (size_t color = 0; color < nb_colors; color++) {
                    // Just in case of a sneaky alpha channel
                    current_r = (colors[color] >> 16) & 0xff;
                    current_g = (colors[color] >> 8) & 0xff;
                    current_b = colors[color] & 0xff;

                    size_t dist = (
                        ((r - current_r) * (r - current_r)) +
                        ((g - current_g) * (g - current_g)) +
                        ((b - current_g) * (b - current_b))
                    );

                    if (dist < match_distance_min) {
                        match_color_pos = color;
                        match_distance_min = dist;
                    }
                }

                current_r = (colors[match_color_pos] >> 16) & 0xff;
                current_g = (colors[match_color_pos] >> 8) & 0xff;
                current_b = colors[match_color_pos] & 0xff;
                system_lut[r][g][b] = RGBA2U32(current_r, current_g, current_b, 0xff);
            }
        }
    }
}
