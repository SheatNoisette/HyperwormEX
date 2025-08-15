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
#include <stddef.h>
#include <math.h>

#include "engine/vec3.h"
#include "engine/utils.h"

// Force GCC to optimize aggressively these functions, they are used extensively
// in the renderer. No Mercy.
#if defined(__GNUC__) && defined(NDEBUG)
#warning "Aggressive vector processing enabled"

vec3 __attribute__((optimize(3))) v3_add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
vec3 __attribute__((optimize(3))) v3_sub(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
vec3 __attribute__((optimize(3))) v3_muls(vec3 a, float s) { return (vec3){a.x * s, a.y * s, a.z * s}; }
float __attribute__((optimize(3))) v3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
float __attribute__((optimize(3))) v3_length(vec3 a) { return sqrtf(v3_dot(a, a)); }
vec3 __attribute__((optimize(3))) v3_norm(vec3 a)
{
    float len = v3_length(a);
    if (len > 1e-6f)
    { // Avoid division by zero
        return v3_muls(a, 1.0f / len);
    }
    return (vec3){0.0f, 0.0f, 0.0f};
}
#else
vec3 v3_add(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
vec3 v3_sub(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
vec3 v3_muls(vec3 a, float s) { return (vec3){a.x * s, a.y * s, a.z * s}; }
float v3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
float v3_length(vec3 a) { return sqrtf(v3_dot(a, a)); }
vec3 v3_norm(vec3 a)
{
    float len = v3_length(a);
    if (len > 1e-6f)
    { // Avoid division by zero
        return v3_muls(a, 1.0f / len);
    }
    return (vec3){0.0f, 0.0f, 0.0f};
}
#endif

vec3 v3_move_to(vec3 a, vec3 b, float step) {
    vec3 vd = v3_sub(b, a);
    float len = v3_length(vd);

    if (len < step || len < 1e-6f) {
        return b;
    }

    return v3_add(a, v3_muls(vd, step / len));
}

vec3 v3_lerp(vec3 a, vec3 b, float decay, float dt) {
    // b+(a-b)*expf(-decay*dt)
    float exp = expf(-decay*dt);
    return (vec3){
        .x = b.x+(a.x-b.x)*exp,
        .y = b.y+(a.y-b.y)*exp,
        .z = b.z+(a.z-b.z)*exp,
    };
}

vec3 v3_rand(float scale) {
    // too lazy for actual normalized random vector, fuck it
    return (vec3){
        .x = randf(-scale, scale),
        .y = randf(-scale, scale),
        .z = randf(-scale, scale),
    };
}

float v3_angle(vec3 a, vec3 b) {
    float cross = a.x * b.z - a.z * b.x;
    float dot = a.x * b.x + a.z * b.z;
    float angle = atan2f(cross, dot);
    return angle;
}
