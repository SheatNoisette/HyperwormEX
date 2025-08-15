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

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

float lerpf(float a, float b, float decay, float dt) {
    return b+(a-b)*expf(-decay*dt);
}

int min(int a, int b) {
    return a <= b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

float clampf(float x, float min, float max) {
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
}

int randi(int a, int b) {
    if (a == b) return a;

    return a + rand() % (b-a);
}

float randf(float a, float b) {
    return a + (float)rand()/((float)RAND_MAX / (b-a));
}

int coinflip() {
    return rand() % 2;
}
