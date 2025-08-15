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

#ifndef __RENDER_H__
#define __RENDER_H__

#include <stdint.h>
#include <stddef.h>
#if defined(__APPLE__)
#include <sys/types.h>
#endif

#include "audio.h"
#include "vec3.h"
#include "light.h"
#include "const.h"
#include "world.h"
#include "camera.h"
#include "actor.h"

// Forward declare actor to avoid recursive includes
typedef struct Actor Actor;

// Get the delta time
#define GET_DELTATIME(render) ((render->dt))

// Mouse Control macros
#define MOUSE_LEFT_CLICK (1 << 0)
#define MOUSE_RIGHT_CLICK (1 << 1)
#define MOUSE_MASK 0xFF
#define MOUSE_GET_CLICK(rd, mc) ((rd->mouse_click & mc ) != 0)
#define MOUSE_GET_ANY(rd) (rd->mouse_click != 0)

// Declared here to avoid recursive includes
typedef struct Sprite {
    size_t width;
    size_t height;
    size_t center_x;
    size_t center_y;

    uint32_t *data;
} Sprite;

typedef struct Billboard {
    vec3 pos;
    float width;
    float height;
    Sprite *spr;
} Billboard;

typedef struct Render {
    // Current world
    World *world;

    // Audio system
    AudioSystem *as;

    // Internal platform data
    void *platform_render_data;

    // Window Management {
    // Window size
    size_t window_width;
    size_t window_height;
    // Depth buffer used for billboards/fast checking
    float *depth_buffer;
    // ARGB Game framebuffer
    uint32_t *frame_buffer;
    // The game camera
    EngineCamera cam;
    // }

    // Controls {
    // Keys

    // Is the mouse locked to screen
    bool mouse_locked;

    // Mouse button
    uint8_t mouse_click;

    // Relative mouse delta
    ssize_t mouse_delta_y;
    ssize_t mouse_delta_x;

    // Mouse position relative to screen
    size_t mouse_x;
    size_t mouse_y;

    // Delta time
    float dt;

    // Number of lights in the scene
    size_t num_lights;
    Light lights[MAX_LIGHTS];

    // Actors in the scene
    size_t num_actors;
    Actor **actors;

    // Maps voxel index to ptr texture
    uint32_t **textures;

    // Precomputed for rendering
    float tan_fov_half;
    float aspect_ratio;
} Render;

Render *new_render(size_t width, size_t height);
void render_free(Render *rd);
void render_world(Render *rd);
void render_billboard(Billboard *bb, Render *rd);
uint32_t *get_texture_data(Render *rd, uint8_t voxel);

#endif
