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
#include <string.h>

#include "engine/const.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "engine/utils.h"
#include "game.h"

static char *lose_text = "YOU DIED.";
static Sprite *lose_sprite = NULL;

static float lose_tick;
static const float lose_time = 5.0f;
static const uint32_t blood = RGBA2U32(0x80, 0, 0, 0xFF);
static const int blood_per_second = 25000; // also known as BPS

void init_lose(Game *g) {
    lose_tick = 0;

    if (!lose_sprite) {
        lose_sprite = sprite_new_text(lose_text, 0xffffff);
    }

    // Stop music
    audio_stop(g->rd->as);
}

void draw_blood_pixels(Render *rd, int amt) {
    while (amt--) {
        int x = randi(0, SOFTWARE_WIDTH);
        int y = 0;
        int i;

        while (
            (i = y*SOFTWARE_WIDTH+x) < SOFTWARE_WIDTH*SOFTWARE_HEIGHT &&
            rd->frame_buffer[i] == blood
        ) {
            y++;
        }

        rd->frame_buffer[i] = blood;
    }
}

GameState update_lose(Game *g) {
    lose_tick += GET_DELTATIME(g->rd);

    if (lose_tick >= lose_time) {
        return GS_END;
    }

    int blood_amt = blood_per_second * GET_DELTATIME(g->rd);
    draw_blood_pixels(g->rd, blood_amt);

    // Draw text
    sprite_draw_rotation(g->rd, lose_sprite, SOFTWARE_WIDTH/2, SOFTWARE_HEIGHT/2, 0, 4);

    return GS_LOSE;
}
