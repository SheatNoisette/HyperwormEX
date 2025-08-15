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
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "engine/camera.h"
#include "engine/const.h"
#include "engine/render.h"
#include "engine/lut.h"
#include "engine/utils.h"

// This bad boy renders the scene, fasten your seatbelt!
// @FIXME: REFACTOR TO INLINED FUNCTIONS
void render_world(Render *rd)
{
    // Texture sampling coords
    float u, v;
    // Ray direction
    vec3 step_dir;
    vec3 side_dist;

    // Voxel face normal for Lighting
    vec3 face_normal;

    uint8_t r_amb = (AMBIENT_LIGHT_COLOR >> 16) & 0xFF;
    uint8_t g_amb = (AMBIENT_LIGHT_COLOR >> 8) & 0xFF;
    uint8_t b_amb = AMBIENT_LIGHT_COLOR & 0xFF;

    for (size_t py = 0; py < SOFTWARE_HEIGHT; py += RENDER_DIV)
    {
        for (size_t px = 0; px < SOFTWARE_WIDTH; px += RENDER_DIV)
        {
            // Setup ray direction
            float ndc_x = (2.0f * px / SOFTWARE_WIDTH) - 1.0f;
            float ndc_y = 1.0f - (2.0f * py / SOFTWARE_HEIGHT);
            float view_x = ndc_x * rd->aspect_ratio * rd->tan_fov_half;
            float view_y = ndc_y * rd->tan_fov_half;
            vec3 ray_dir_view = {view_x, view_y, 1.0f};
            vec3 ray_dir = v3_add(v3_add(v3_muls(rd->cam.right, ray_dir_view.x),
                                         v3_muls(rd->cam.up, ray_dir_view.y)),
                                  v3_muls(rd->cam.forward, ray_dir_view.z));
            ray_dir = v3_norm(ray_dir);

            // Ray Casting / Voxel Traversal, sometime I wanna throw up
            vec3 ray_pos = rd->cam.pos;
            vec3 map_pos = {floorf(ray_pos.x), floorf(ray_pos.y), floorf(ray_pos.z)};
            vec3 delta_dist = {
                .x = fabsf(ray_dir.x) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / ray_dir.x),
                .y = fabsf(ray_dir.y) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / ray_dir.y),
                .z = fabsf(ray_dir.z) < ENGINE_MIN_FLOAT ? ENGINE_MAX_FLOAT : fabsf(1.0f / ray_dir.z)
            };

            // Set ray direction XYZ
            if (ray_dir.x < 0)
            { step_dir.x = -1; side_dist.x = (ray_pos.x - map_pos.x) * delta_dist.x; }
            else
            { step_dir.x = 1;  side_dist.x = (map_pos.x + 1.0f - ray_pos.x) * delta_dist.x; }

            if (ray_dir.y < 0)
            { step_dir.y = -1; side_dist.y = (ray_pos.y - map_pos.y) * delta_dist.y; }
            else
            { step_dir.y = 1;  side_dist.y = (map_pos.y + 1.0f - ray_pos.y) * delta_dist.y; }

            if (ray_dir.z < 0)
            { step_dir.z = -1; side_dist.z = (ray_pos.z - map_pos.z) * delta_dist.z; }
            else
            { step_dir.z = 1;  side_dist.z = (map_pos.z + 1.0f - ray_pos.z) * delta_dist.z; }

            uint8_t r_sky = (SKY_COLOR >> 16) & 0xFF;
            uint8_t g_sky = (SKY_COLOR >> 8) & 0xFF;
            uint8_t b_sky = SKY_COLOR & 0xFF;

#if ENABLE_LUT
            uint32_t pixel_color = lut_convert_rgb(r_sky, g_sky, b_sky);
#else
            uint32_t pixel_color = SKY_COLOR;
#endif
            // Face_hit simplifiy where the ray collided
            // @TODO: MAKE THIS AS A ENUM !!!!!!!!!!!!!!!!
            // THIS IS UNREADABLE!
            uint8_t face_hit = 0;
            float distance = 0.0f;
            float hit_steps = 0.f;

            // For the first pass, we're going to assume we hit nothing in the depth buffer, this will
            // be updated in the loop
            rd->depth_buffer[py * SOFTWARE_WIDTH + px] = ENGINE_MAX_FLOAT;

            // Traverse the map and hope to collide with a voxel
            while (hit_steps < RENDER_DISTANCE && distance <= RENDER_DISTANCE)
            {
                hit_steps += 1.f;
                if (side_dist.x < side_dist.y && side_dist.x < side_dist.z)
                {
                    distance = side_dist.x;
                    side_dist.x += delta_dist.x;
                    map_pos.x += step_dir.x;
                    face_hit = (step_dir.x > 0) ? 3 : 2;
                }
                else if (side_dist.y < side_dist.z)
                {
                    distance = side_dist.y;
                    side_dist.y += delta_dist.y;
                    map_pos.y += step_dir.y;
                    face_hit = (step_dir.y > 0) ? 1 : 0;
                }
                else
                {
                    distance = side_dist.z;
                    side_dist.z += delta_dist.z;
                    map_pos.z += step_dir.z;
                    face_hit = (step_dir.z > 0) ? 5 : 4;
                }

                uint8_t voxel_type = world_get(rd->world, (size_t)map_pos.x, (size_t)map_pos.y, (size_t)map_pos.z);

                // Hit a solid voxel LEZGO
                if (voxel_type > 0)
                {
                    const uint32_t *tex_data = get_texture_data(rd, voxel_type);
#if SAFE_C
                    if (!tex_data)
                    {
                        pixel_color = 0xFF00FF;
                        break;
                    }
#endif

                    vec3 hit_pos = v3_add(ray_pos, v3_muls(ray_dir, distance));

                    // Texture Mapping, map a texel (px coordinates) to a face
                    if (face_hit == 0 || face_hit == 1)
                    { u = hit_pos.x - map_pos.x; v = hit_pos.z - map_pos.z; }
                    else if (face_hit == 2 || face_hit == 3)
                    { u = hit_pos.z - map_pos.z; v = hit_pos.y - map_pos.y; }
                    else
                    { u = hit_pos.x - map_pos.x; v = hit_pos.y - map_pos.y; }

                    // Flip UV relative to ray face position
                    if (face_hit == 3)
                        u = 1.0f - u;
                    if (face_hit == 1)
                        v = 1.0f - v;
                    if (face_hit == 5)
                        u = 1.0f - u;
#if UNREAL_TEX_AA
                    // Unreal Anti-Alias dithering depending on the position
                    // @TODO: Set it as a matrix
                    // https://www.flipcode.com/archives/Texturing_As_In_Unreal.shtml
                    if ((px & 1) == 1 && (py & 1) == 1) {
                        //u+=.00;
                        v+=0.05f;
                    } else if ((px & 1) == 1 && (py & 1) == 0) {
                        u+=0.04f;
                        v+=0.07f;
                    } else if ((px & 1) == 0 && (py & 1) == 1) {
                        u+=0.07f;
                        v-=0.03f;
                    } else { // ((px & 1) == 0 && (py & 1) == 0)
                        u+=0.04f;
                        //v+=.00;
                    }
#endif
                    // Map texture x, y to the REAL texture
                    size_t tex_x = (size_t)(u * TEX_SIZE) % TEX_SIZE;
                    size_t tex_y = (size_t)(v * TEX_SIZE) % TEX_SIZE;
                    uint32_t texel_color = tex_data[tex_y * TEX_SIZE + tex_x];

                    // Normal vector of the face that was hit
                    if (face_hit == 0)
                        face_normal = (vec3){0.0f, 1.0f, 0.0f};  // Y+
                    else if (face_hit == 1)
                        face_normal = (vec3){0.0f, -1.0f, 0.0f}; // Y-
                    else if (face_hit == 2)
                        face_normal = (vec3){1.0f, 0.0f, 0.0f};  // X+
                    else if (face_hit == 3)
                        face_normal = (vec3){-1.0f, 0.0f, 0.0f}; // X-
                    else if (face_hit == 4)
                        face_normal = (vec3){0.0f, 0.0f, 1.0f};  // Z+
                    else
                        face_normal = (vec3){0.0f, 0.0f, -1.0f}; // Z-

                    // Start with ambient light
                    // @FIXME: THIS COULD BE A LOT SIMPLIFIED
                    float final_r = (texel_color & 0xFF) / 255.0f * (r_amb / 255.0f) * AMBIENT_INTENSITY;
                    float final_g = ((texel_color >> 8) & 0xFF) / 255.0f * (g_amb / 255.0f) * AMBIENT_INTENSITY;
                    float final_b = ((texel_color >> 16) & 0xFF) / 255.0f * (b_amb / 255.0f) * AMBIENT_INTENSITY;

                    // Add contribution from each light source
                    for (size_t i = 0; i < rd->num_lights; i++)
                    {
                        // Skip lights that are too dark
                        if (rd->lights[i].intensity < ENGINE_MIN_FLOAT) {
                            continue;
                        }

                        vec3 light_dir = v3_sub(rd->lights[i].pos, hit_pos);
                        float dist_to_light_sq = v3_dot(light_dir, light_dir);
                        float dist_to_light = sqrtf(dist_to_light_sq);
                        // Normalize after getting distance
                        light_dir = v3_norm(light_dir);

                        // Shadow Check
                        if (!is_in_shadow(rd->world, hit_pos, rd->lights[i].pos, dist_to_light))
                        {
                            // Lambertian diffuse reflection (dot product between normal and light direction)
                            // to make it look pretty, this is overkill and is barely noticeable :(
                            // https://en.wikipedia.org/wiki/Lambertian_reflectance
                            float diffuse_factor = fmaxf(0.0f, v3_dot(face_normal, light_dir));

                            // Simple distance attenuation (inverse square falloff)
                            // Could be swapped for linear, but this looks better for me
                            float attenuation = 1.0f / (1.0f + 0.01f * dist_to_light_sq);

                            float light_contrib_r = rd->lights[i].color.x * rd->lights[i].intensity * diffuse_factor * attenuation;
                            float light_contrib_g = rd->lights[i].color.y * rd->lights[i].intensity * diffuse_factor * attenuation;
                            float light_contrib_b = rd->lights[i].color.z * rd->lights[i].intensity * diffuse_factor * attenuation;

                            // Add light contribution modulated by texture color
                            final_r += (texel_color & 0xFF) / 255.0f * light_contrib_r;
                            final_g += ((texel_color >> 8) & 0xFF) / 255.0f * light_contrib_g;
                            final_b += ((texel_color >> 16) & 0xFF) / 255.0f * light_contrib_b;
                        }
                    }

                    // Clamp color values to 0 -> 1 range before converting back
                    final_r = fmaxf(0.0f, fminf(final_r, 1.0f)) * 255.0f;
                    final_g = fmaxf(0.0f, fminf(final_g, 1.0f)) * 255.0f;
                    final_b = fmaxf(0.0f, fminf(final_b, 1.0f)) * 255.0f;

#if ENABLE_FOG
                    // Apply Fog (applied after lighting)
                    float fog_factor = 1.0f - fminf(distance / (RENDER_DISTANCE / FOG_INTENSITY), 1.0f);
                    fog_factor = fmaxf(fog_factor, 0.0f);
                    uint8_t r_final = (uint8_t)(final_r * fog_factor + r_sky * (1.0f - fog_factor));
                    uint8_t g_final = (uint8_t)(final_g * fog_factor + g_sky * (1.0f - fog_factor));
                    uint8_t b_final = (uint8_t)(final_b * fog_factor + b_sky * (1.0f - fog_factor));
#else
                    uint8_t r_final = (uint8_t)(final_r);
                    uint8_t g_final = (uint8_t)(final_g);
                    uint8_t b_final = (uint8_t)(final_b);
#endif

#if ENABLE_LUT
                    pixel_color = lut_convert_rgb(r_final, g_final, b_final);

#else
                    pixel_color = RGBA2U32(r_final, g_final, b_final, 0xff);
#endif

                    // Update depth buffer
                    for (size_t y = 0; y < RENDER_DIV; y++)
                    {
                        for (size_t x = 0; x < RENDER_DIV; x++)
                        {
#if SAFE_C
                            if ((px + x < SOFTWARE_WIDTH) && (py + y < SOFTWARE_HEIGHT))
                            {
#endif
                                rd->depth_buffer[(py + y) * SOFTWARE_WIDTH + (px + x)] = distance;
#if SAFE_C
                            }
#endif
                        }
                    }

                    // Stop ray casting for this pixel or remove it if you want a good trip
                    break;
                }
            }

#if SAFE_C
            // Draw the pixel block
            if (px >= 0 && px < SOFTWARE_WIDTH && py >= 0 && py < SOFTWARE_HEIGHT)
            {
#endif
                for (size_t y = 0; y < RENDER_DIV; y++)
                {
                    for (size_t x = 0; x < RENDER_DIV; x++)
                    {
#if SAFE_C
                        // Bounds check for block fill
                        if ((px + x < SOFTWARE_WIDTH) && (py + y < SOFTWARE_HEIGHT))
                        {
#endif
                            rd->frame_buffer[(py + y) * SOFTWARE_WIDTH + (px + x)] = pixel_color;
#if SAFE_C
                        }
#endif
                    }
                }
#if SAFE_C
            }
#endif
        }
    }
}

// Render a billboard to the scene
void render_billboard(Billboard *bb, Render *rd) {
    // Transform billboard center to the view
    vec3 cam_to_bb = v3_sub(bb->pos, rd->cam.pos);
    // Distance along camera forward
    float view_z = v3_dot(cam_to_bb, rd->cam.forward);

    // Avoid to render if we're far
    if (view_z >= RENDER_SPRITE_DISTANCE)
        return;

    float view_x = v3_dot(cam_to_bb, rd->cam.right);
    float view_y = v3_dot(cam_to_bb, rd->cam.up);

    // Project to Screen Space & Calculate Screen Size
    // Perspective projection factor (inverse distance)
    float inv_view_z = 1.0f / view_z;

    // Calculate screen center coordinates (NDC -> Screen)
    // This was approximated on Paint, no idea why it works
    float focal_length_y = SOFTWARE_HEIGHT / (2.0f * rd->tan_fov_half);
    float focal_length_x = SOFTWARE_WIDTH / (2.0f * rd->tan_fov_half * rd->aspect_ratio);
    float screen_center_x = (view_x * inv_view_z * focal_length_x) + (SOFTWARE_WIDTH * 0.5f);
    float screen_center_y = (-view_y * inv_view_z * focal_length_y) + (SOFTWARE_HEIGHT * 0.5f);

    // Calculate screen dimensions relative to view
    float screen_height = fabsf(bb->height) * inv_view_z * focal_length_y;
    float screen_width = fabsf(bb->width) * inv_view_z * focal_length_x;

    // If we are out of bound, this may cause a under zero number and mess up
    // scaling computation AND do a segfault
    if (screen_height < 0.f || screen_width <= 0.f)
        return;

    // Screen Bounds
    // @TODO: DIAGNOSE WHY WE ARE OUT OF BOUND SOMETIMES, CLAMP TO BE SAFE!
    size_t min_sx = (size_t)fmin(fmaxf(0.0f, floorf(screen_center_x - screen_width * 0.5f)), SOFTWARE_WIDTH);
    size_t max_sx = (size_t)fmax(fminf((float)SOFTWARE_WIDTH, ceilf(screen_center_x + screen_width * 0.5f)), (float)SOFTWARE_WIDTH);
    size_t min_sy = (size_t)fmaxf(0.0f, floorf(screen_center_y - screen_height * 0.5f));
    size_t max_sy = (size_t)fmax(fminf((float)SOFTWARE_HEIGHT, ceilf(screen_center_y + screen_height * 0.5f)), (float)SOFTWARE_HEIGHT);

    // Quick hack when we're looking at weird angles and really near (looking at our feets/sky)
    // @TODO: Better fustrum culling
    if (view_z <= .5f && min_sy == 0 && max_sx == SOFTWARE_WIDTH && min_sy == 0 && max_sy == SOFTWARE_HEIGHT)
        return;

    // Iterate through pixels within bounds
    for (size_t sy = min_sy; sy < max_sy; sy++)
    {
        for (size_t sx = min_sx; sx < max_sx; sx++)
        {
            // Depth Check, to occult if there's a block
            size_t pixel_index = (size_t)sy * SOFTWARE_WIDTH + sx;

            // Normalize the view from the raycaster to the billboard, took a F**** while to find out
            float normalized_view_z = v3_length(cam_to_bb);

            if (normalized_view_z > rd->depth_buffer[pixel_index])
                continue;

            // Calculate texel (same as before)
            float u = (sx - (screen_center_x - screen_width * 0.5f)) / screen_width;
            float v = (sy - (screen_center_y - screen_height * 0.5f)) / screen_height;

            // We don't need to sample beyond the texture
            if (u > 1.f || v > 1.f || u < 0.f || v < 0.f)
                continue;

            // Flip if width < 0 or height < 0
            u = bb->width < 0 ? (1.0 - u) : u;
            v = bb->height < 0 ? (1.0 - v) : v;

            // Sample Teexture
            size_t tex_x = (size_t)(u * bb->spr->width) % bb->spr->width;
            size_t tex_y = (size_t)(v * bb->spr->height) % bb->spr->height;
            uint32_t texel_color = bb->spr->data[tex_y * bb->spr->width + tex_x];

            // Skip transparent pixel
            if (texel_color == 0)
                continue;

            // Apply Fog (using billboard distance view_z)
            float fog_factor = 1.0f - fminf(view_z / (RENDER_DISTANCE / FOG_INTENSITY), 1.0f);
            fog_factor = fmaxf(fog_factor, 0.0f);

            uint8_t r_lit = (texel_color >> 16) & 0xFF;
            uint8_t g_lit = (texel_color >> 8) & 0xFF;
            uint8_t b_lit = texel_color & 0xFF;
            uint8_t r_sky = (SKY_COLOR >> 16) & 0xFF;
            uint8_t g_sky = (SKY_COLOR >> 8) & 0xFF;
            uint8_t b_sky = SKY_COLOR & 0xFF;

            uint8_t r_final = (uint8_t)(r_lit * fog_factor + r_sky * (1.0f - fog_factor));
            uint8_t g_final = (uint8_t)(g_lit * fog_factor + g_sky * (1.0f - fog_factor));
            uint8_t b_final = (uint8_t)(b_lit * fog_factor + b_sky * (1.0f - fog_factor));

            // Draw
            for (size_t y = 0; y < RENDER_DIV; y++)
            {
                for (size_t x = 0; x < RENDER_DIV; x++)
                {
#if SAFE_C
                    if ((sx + x < SOFTWARE_WIDTH) && (sy + y < SOFTWARE_HEIGHT))
                    {
#endif
                        // Final depth check for sub-pixels (important if RENDER_DIV > 1 !!!)
                        size_t b_pos = (sy + y) * SOFTWARE_WIDTH + (sx + x);
                        if (view_z < rd->depth_buffer[b_pos])
                        {
#if ENABLE_LUT
                            rd->frame_buffer[b_pos] = lut_convert_rgb(r_final, g_final, b_final);
#else
                            rd->frame_buffer[b_pos] = RGBA2U32(r_final, g_final, b_final, 0xff);
#endif
                            rd->depth_buffer[b_pos] = normalized_view_z;
                        }
#if SAFE_C
                    }
#endif
                }
            }
        }
    }
}
