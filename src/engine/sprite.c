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
#include <string.h>
#include <stdlib.h>

#include "engine/const.h"
#include "engine/render.h"
#include "engine/text.h"
#include "engine/sprite.h"

// @TODO: Move this to utils
static inline float min4(float a, float b, float c, float d) {
    float m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    if (d < m) m = d;
    return m;
}

// @TODO: Move this to utils
static inline float max4(float a, float b, float c, float d) {
    float m = a;
    if (b > m) m = b;
    if (c > m) m = c;
    if (d > m) m = d;
    return m;
}

Sprite *sprite_new(size_t width, size_t height, size_t center_x, size_t center_y) {
    Sprite *spr = malloc(sizeof(Sprite));
    spr->data = malloc(sizeof(uint32_t) * height * width);
#if SAFE_C
    if (spr == NULL) {
        return NULL;
    }
#endif
    spr->width = width;
    spr->height = height;
    spr->center_x = center_x;
    spr->center_y = center_y;

    memset(spr->data, 0, sizeof(uint32_t) * height * width);

    return spr;
}

Sprite *sprite_new_text(const char *text, uint32_t color) {
    int width = TEXT_WIDTH(strlen(text));
    int height = LETTER_HEIGHT;
    Sprite *spr = sprite_new(width, height, width/2, height/2);
    text_draw_sprite(spr, text, 0, 0, color);
    return spr;
}

void sprite_free(Sprite *spr) {
#if SAFE_C
    if (spr == NULL)
        return;
    if (spr->data != NULL)
        free(spr->data);
    free(spr);
#else
    free(spr->data);
    free(spr);
#endif
}

void sprite_pixel(Sprite *spr, size_t pos_x, size_t pos_y, uint32_t color) {
    spr->data[pos_x + pos_y * spr->width] = color;
}

// -> angle Rotation angle in radians.
// -> Zoom factor (1.0 = original size, > 1.0 = zoom in, < 1.0 = zoom out).
void sprite_draw_rotation(
    Render *rd,
    Sprite *spr,
    size_t pos_x,
    size_t pos_y,
    float angle,
    float zoom
) {
    // Extract framebuffer
    uint32_t *fb = rd->frame_buffer;
    size_t fb_width = SOFTWARE_WIDTH;
    size_t fb_height = SOFTWARE_HEIGHT;
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    float inv_zoom = 1.0f / zoom;

    // Sprite corner
    float rel_x0 = -(float)spr->center_x;
    float rel_y0 = -(float)spr->center_y;
    float rel_x1 = (float)spr->width - (float)spr->center_x;
    float rel_y1 = (float)spr->height - (float)spr->center_y;

    // Calculate transformed corner coordinates
    // top-left
    float tx0 = zoom * (rel_x0 * cos_angle - rel_y0 * sin_angle);
    float ty0 = zoom * (rel_x0 * sin_angle + rel_y0 * cos_angle);
    // top-right
    float tx1 = zoom * (rel_x1 * cos_angle - rel_y0 * sin_angle);
    float ty1 = zoom * (rel_x1 * sin_angle + rel_y0 * cos_angle);
    // bottom-left
    float tx2 = zoom * (rel_x0 * cos_angle - rel_y1 * sin_angle);
    float ty2 = zoom * (rel_x0 * sin_angle + rel_y1 * cos_angle);
    // bottom-right
    float tx3 = zoom * (rel_x1 * cos_angle - rel_y1 * sin_angle);
    float ty3 = zoom * (rel_x1 * sin_angle + rel_y1 * cos_angle);

    // Bounding box
    ssize_t min_dx = (ssize_t)floorf(min4(tx0, tx1, tx2, tx3) + pos_x);
    ssize_t max_dx = (ssize_t)ceilf(max4(tx0, tx1, tx2, tx3) + pos_x);
    ssize_t min_dy = (ssize_t)floorf(min4(ty0, ty1, ty2, ty3) + pos_y);
    ssize_t max_dy = (ssize_t)ceilf(max4(ty0, ty1, ty2, ty3) + pos_y);

    // Clip the bounding box to the framebuffer dimensions to avoid pranks
    ssize_t start_x = (min_dx < 0) ? 0 : min_dx;
    ssize_t end_x = (max_dx > (ssize_t)fb_width) ? (ssize_t)fb_width : max_dx;
    ssize_t start_y = (min_dy < 0) ? 0 : min_dy;
    ssize_t end_y = (max_dy > (ssize_t)fb_height) ? (ssize_t)fb_height : max_dy;

    for (ssize_t dy = start_y; dy < end_y; dy++) {
        for (ssize_t dx = start_x; dx < end_x; dx++) {

            float rel_dx = (float)dx - (float)pos_x;
            float rel_dy = (float)dy - (float)pos_y;

            // Apply inverse transformation to find the corresponding point
            // in the original sprite's coordinate system, relative to its center
            float orig_rel_x = (rel_dx * cos_angle + rel_dy * sin_angle) * inv_zoom;
            float orig_rel_y = (-rel_dx * sin_angle + rel_dy * cos_angle) * inv_zoom;

            // Convert back to absolute source pixel coordinate
            float sx_f = orig_rel_x + (float)spr->center_x;
            float sy_f = orig_rel_y + (float)spr->center_y;

            ssize_t sx = (ssize_t)floorf(sx_f);
            ssize_t sy = (ssize_t)floorf(sy_f);
            if (sx >= 0 && sx < (ssize_t)spr->width && sy >= 0 && sy < (ssize_t)spr->height) {
                uint32_t src_pixel = spr->data[sy * spr->width + sx];

                // Draw only if not fully transparent
                if ((src_pixel & 0xffffff) != 0x00) {
                    size_t fb_index = dy * fb_width + dx;
                    fb[fb_index] = src_pixel;
                }
            }
        }
    }
}

void sprite_unpack_2bit(
    Sprite *spr,
    uint8_t *sprite,
    uint32_t palette[4],
    size_t w,
    size_t h
) {
    // For each uint8_t
    // -> Decompose 8bits -> 2bits + 2bits + 2bits + 2bits
    for (size_t len = 0; len < w * h; len += 4) {
        uint8_t spr_content = sprite[len / 4];
        spr->data[len] = palette[((spr_content >> 6) & 0b11)];
        spr->data[len + 1] = palette[((spr_content >> 4) & 0b11)];
        spr->data[len + 2] = palette[((spr_content >> 2) & 0b11)];
        spr->data[len + 3] = palette[(spr_content & 0b11)];
    }
}
