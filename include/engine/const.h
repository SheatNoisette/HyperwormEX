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

#ifndef __CONST_H__
#define __CONST_H__

// ENGINE FLAGS
// Safe mode, check allocs etc.
#define SAFE_C 0
// Enable fog
#define ENABLE_FOG 1
// Simulate anti aliasing on texture, based on Tim Sweeney
#define UNREAL_TEX_AA 1
// Enable or disable bilinear filter on the canvas
#define NINTENDO_64 0
// Enable color palette LUT support
#define ENABLE_LUT 1

// Software renderer size
#define SOFTWARE_WIDTH 320
#define SOFTWARE_HEIGHT 180
// Real GL Window
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
// Name of the window
#define WINDOW_NAME "HYPERWORM EX - 1.1"
// Max distance to trace rays
#define RENDER_DISTANCE 64.0f
// Max sprite distance
#define RENDER_SPRITE_DISTANCE 30.f
// Reduce resolution if the CPU is crap
#define RENDER_DIV 1
// Minimum and maximum float clamp
#define ENGINE_MIN_FLOAT 1e-6f
#define ENGINE_MAX_FLOAT 1e9f

// Rendering definitions
// The FOV in deg
#define FOV_DEG 90
#define FOV (FOV_DEG * 0.01745329)
// Size of a texture
#define TEX_SIZE 32
// Max lights supported
#define MAX_LIGHTS 2
// Ambient Light
#define AMBIENT_LIGHT_COLOR 0xFFFFFF
// Color of the sky
#define SKY_COLOR 0x202020
// Mix ambient -> lights
#define AMBIENT_INTENSITY 0.25f
// Number of tiles and textures
#define MAX_TEXTURES 16
// Fog density
#define FOG_INTENSITY 1

// Physics stuff
// Acceleration due to gravity (positive due to inverted Y)
#define GRAVITY 22.0f

// Actors
// Number of actors at maximum
#define MAX_ACTORS 128

#endif
