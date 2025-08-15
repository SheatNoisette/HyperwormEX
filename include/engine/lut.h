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

#ifndef __LUT_H__
#define __LUT_H__

#include <stddef.h>
#include <stdint.h>

#define LUT_COLORS_R 256
#define LUT_COLORS_G 256
#define LUT_COLORS_B 256

void lut_reset();
void lut_build(const uint32_t *colors, size_t nb_colors);
uint32_t lut_convert_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif
