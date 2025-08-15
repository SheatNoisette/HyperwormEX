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

#ifndef __TUNNEL_H__
#define __TUNNEL_H__

#include "engine/render.h"

// Amount of rotation change
#define TUNNEL_ROTATION 1.0f
// Star movement speed
#define TUNNEL_SPEED 30.0f
// Tunnel effect texture size (determines star size)
#define TUNNEL_TEX_SIZE 80
// Streak ("star") color
#define TUNNEL_COLOR 0xB06050
// Streak length (inverse)
#define TUNNEL_LENGTH 12.0f
// 1 chance in TUNNEL_SPARSENESS for a pixel to be colored (aka a "star")
#define TUNNEL_SPARSENESS 300
// Darkness factor
#define TUNNEL_DARKNESS 0.5f

// Initialize LUTs and textures
void tunnel_init();
// Render the tunnel onto the screen framebuffer
void tunnel_draw(Render *rd);

#endif
