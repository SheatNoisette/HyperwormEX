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

#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "engine/render.h"

// Number of textures
#define TEXTURES_NUMBER 6
// Atlas
#define TEX_BARRIER 1
#define TEX_ROCKS 2
#define TEX_BRICK 3
#define TEX_STEEL 4
#define TEX_TILES 5
#define TEX_CRATE 6
// End atlas

#define TEXTURE_NOISE_AMOUNT 20

void load_voxel_textures(Render *rd);

#endif
