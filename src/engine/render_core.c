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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"
#include "engine/platform.h"

Render *new_render(size_t width, size_t height)
{
    Render *rd = malloc(sizeof(Render));
#if SAFE_C
    if (rd == NULL)
        return NULL;
#else
    memset(rd, 0, sizeof(Render));
#endif
    rd->frame_buffer = malloc(sizeof(uint32_t) * width * height);
    rd->depth_buffer = malloc(sizeof(float) * width * height);
    rd->textures = malloc(sizeof(uint32_t *) * MAX_TEXTURES);
    rd->actors = malloc(sizeof(Actor) * MAX_ACTORS);

    // The world isn't initialized by default
    rd->window_width = WINDOW_WIDTH;
    rd->window_height = WINDOW_HEIGHT;
    rd->mouse_locked = false;


    rd->tan_fov_half = tanf(FOV / 2.0f);
    rd->aspect_ratio = (float)SOFTWARE_WIDTH / (float)SOFTWARE_HEIGHT;

#if SAFE_C
    rd->mouse_delta_y = 0;
    rd->mouse_delta_x = 0;
    rd->num_actors = 0;
    rd->mouse_click = 0;
#endif

    // Start the renderer
    platform_init(rd);

    // Init actor system
    actor_init(rd->actors, rd);

    return rd;
}

void render_update(Render *rd) {
    platform_update(rd);
}

void render_begin(Render *rd) {
    platform_render_begin(rd);
}

void render_end(Render *rd) {
    platform_render_end(rd);
}

void render_free(Render *rd)
{
    free(rd->depth_buffer);
    free(rd->frame_buffer);
    free(rd->actors);
    platform_free(rd);
}

uint32_t *get_texture_data(Render *rd, uint8_t voxel)
{
    return rd->textures[voxel];
}
