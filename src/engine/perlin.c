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
#include <stdlib.h>
#include <math.h>
#if defined(__APPLE__)
#include <sys/types.h>
#endif

// Permutation table
static uint8_t perm[512] = { 0 };

// Initialize perm[] with values 0–255
void perlin_init_permutation() {
    // Fill with identity permutation
    for (size_t i = 0; i < 256; i++) {
        perm[i] = (uint8_t)i;
    }
    for (size_t i = 255; i > 0; i--) {
        size_t j = rand() % (i + 1);
        uint8_t tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
    // Duplicate to avoid overflow when indexing
    for (size_t i = 0; i < 256; i++) {
        perm[256 + i] = perm[i];
    }
}

// Compact perlin noise
float perlin_2d(float x,float y) {
    size_t xi = (size_t)floorf(x)&255,
        yi = (size_t)floorf(y)&255;
    float xf = x - floorf(x),
          yf = y - floorf(y),
          u  = xf*xf*xf*(xf*(xf*6 - 15) + 10),
          v  = yf*yf*yf*(yf*(yf*6 - 15) + 10);
    ssize_t aa = perm[perm[xi  ] + yi],
        ab = perm[perm[xi  ] + yi+1],
        ba = perm[perm[xi+1] + yi],
        bb = perm[perm[xi+1] + yi+1];

#define G(h,x,y) ( ((h)&1?-( (h)<2?x:y ):( (h)<2?x:y )) \
                 + ((h)&2?-2*( (h)<2?y:x ):2*( (h)<2?y:x )) )
    float a = G(aa,xf,   yf),
          b = G(ba,xf-1, yf),
          c = G(ab,xf,   yf-1),
          d = G(bb,xf-1, yf-1);
#undef G

    float x1 = a + u*(b - a),
          x2 = c + u*(d - c);
    return x1 + v*(x2 - x1);
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    float result;
    result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return result;
}
