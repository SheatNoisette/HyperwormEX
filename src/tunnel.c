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

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/render.h"
#include "engine/utils.h"
#include "tunnel.h"

// Textures
static uint32_t texture_tunnel[TUNNEL_TEX_SIZE * TUNNEL_TEX_SIZE] = {0};
static float distance_map[SOFTWARE_WIDTH * SOFTWARE_HEIGHT * sizeof(float)] = {0};
static float angle_map[SOFTWARE_WIDTH * SOFTWARE_HEIGHT * sizeof(float)] = {0};
static float tunnel_tick;

void tunnel_init() {
    tunnel_tick = 0;

    // Build tunnel effect texture
    for (size_t ty = 0; ty < TUNNEL_TEX_SIZE; ty++)
    {
        for (size_t tx = 0; tx < TUNNEL_TEX_SIZE; tx++)
        {
            texture_tunnel[ty * TUNNEL_TEX_SIZE + tx] = (rand() % TUNNEL_SPARSENESS == 0) ? TUNNEL_COLOR : 0x000000;
        }
    }

    for (ssize_t y = 0; y < SOFTWARE_HEIGHT; y++)
    {
        for (ssize_t x = 0; x < SOFTWARE_WIDTH; x++)
        {
            size_t index = y * SOFTWARE_WIDTH + x;
            float dx = x - ((float)SOFTWARE_WIDTH / 2);
            float dy = y - ((float)SOFTWARE_HEIGHT / 2);
            // Distance from center
            distance_map[index] = sqrtf(dx * dx + dy * dy);
            // Angle relative center -PI to +PI
            angle_map[index] = atan2f(dy, dx);
        }
    }
}

void tunnel_draw(Render *rd) {
    tunnel_tick += GET_DELTATIME(rd);
    // Rotation
    float time_offset_angle = tunnel_tick * TUNNEL_ROTATION;
    // speed
    float time_offset_dist = tunnel_tick * TUNNEL_SPEED;

    for (size_t y = 0; y < SOFTWARE_HEIGHT; y++)
    {
        for (size_t x = 0; x < SOFTWARE_WIDTH; x++)
        {
            size_t screen_index = y * SOFTWARE_WIDTH + x;
            float dist = distance_map[screen_index];
            float angle = angle_map[screen_index];

            // Avoid division by zero at the exact center oops
            if (dist < 0.01f)
            {
                rd->frame_buffer[screen_index] = 0;
                continue;
            }

            // Angle to texture U coordinate (horizontal)
            // From [-PI, PI] to [0, 1]
            float u_raw = (angle / M_2_PI + 0.5f) * TUNNEL_TEX_SIZE - time_offset_angle * (TUNNEL_TEX_SIZE / M_2_PI);

            // Distance to texture V coordinate (vertical)
            float v_raw = (TUNNEL_TEX_SIZE * TUNNEL_LENGTH / dist) + time_offset_dist;

            // Avoid getting pranked by the modulo
            size_t u = ((ssize_t)(u_raw) % TUNNEL_TEX_SIZE + TUNNEL_TEX_SIZE) % TUNNEL_TEX_SIZE;
            size_t v = ((ssize_t)(v_raw) % TUNNEL_TEX_SIZE + TUNNEL_TEX_SIZE) % TUNNEL_TEX_SIZE;

            uint32_t tex_color = texture_tunnel[v * TUNNEL_TEX_SIZE + u];

            float darkness_factor = dist / (SOFTWARE_WIDTH * TUNNEL_DARKNESS);
            // Clamp
            darkness_factor = clampf(darkness_factor, 0.01f, 1.0f);

            uint8_t r = (tex_color >> 16) & 0xFF;
            uint8_t g = (tex_color >> 8) & 0xFF;
            uint8_t b = tex_color & 0xFF;

            r *= darkness_factor;
            g *= darkness_factor;
            b *= darkness_factor;

            rd->frame_buffer[screen_index] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
    }
}
