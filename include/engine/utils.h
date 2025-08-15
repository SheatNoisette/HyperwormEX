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

#ifndef __UTILS_H__
#define __UTILS_H__

// Big endian
// #define RGBA2U32(r, g, b, a) ((a) << 24 | (r) << 16 | (g) << 8 | (b))
// Little endian
#define RGBA2U32(r, g, b, a) ((a) << 24 | (b) << 16 | (g) << 8 | (r))

// Interpolate between two values
// Freya's framerate-independent lerp
// Decay: useful range 1-25, from slow to fast
float lerpf(float a, float b, float decay, float dt);

int min(int a, int b);

int max(int a, int b);

// Clamp a float between two values
float clampf(float x, float min, float max);

// Random integer between a (incl.) and b (excl.)
int randi(int a, int b);

// Random float between a and b
float randf(float a, float b);

// Random int, 0 or 1
int coinflip();

#endif
