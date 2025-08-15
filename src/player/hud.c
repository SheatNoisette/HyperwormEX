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
#include <stdio.h>
#include <string.h>

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "engine/text.h"
#include "engine/utils.h"
#include "engine/vec3.h"
#include "flags.h"
#include "level.h"
#include "player.h"
#include "sound_atlas.h"
#include "sprite_atlas.h"

static const char *hp_label = "HP";
static const char *hp_bar = "|";
static const char *radar_dot = ".";

static const int center_x = SOFTWARE_WIDTH/2;
static const int center_y = SOFTWARE_HEIGHT/2;
static const int bottom_y = SOFTWARE_HEIGHT-HUD_PADDING-LETTER_HEIGHT;
static const int letter_center_x = LETTER_WIDTH/2;

static const int gun_x = SOFTWARE_WIDTH + 72;
static const int gun_y = SOFTWARE_HEIGHT + 144;

static int blink_frame = 0;

void player_draw_gun(Actor *self, Render *rd) {
    PlayerData *d = self->data;

    float sway = GUN_SWAY_SPEED * d->view_accumulator;
    float x_sway = 10.0f * sinf(sway);
    float y_sway = 10.0f * sinf(2 * sway) / 2.0f;
    // Draw gun
    sprite_draw_rotation(
        rd,
        spr_get(SPR_GUN),
        gun_x + x_sway,
        gun_y + y_sway + PLAYER_LIGHT(rd).intensity,
        (PLAYER_LIGHT(rd).intensity - PLAYER_LIGHT_INTENSITY) * 0.1f,
        4
    );

#if FEATURE_STEP_SOUNDS
    // Sound played when the player walks
    if (x_sway <= -9.f && !d->sfx_stepped_left) {
        d->sfx_stepped_left = true;
        audio_play(rd->as, sad_get_sound(SAD_PLAYER_WALK), 0.5f, randf(0.8f, 1), 0, false);
    } else if (x_sway >= 9.f && d->sfx_stepped_left) {
        d->sfx_stepped_left = false;
        audio_play(rd->as, sad_get_sound(SAD_PLAYER_WALK), 0.5f, randf(0.8f, 1), 0, false);
    }
#endif

    // Draw cursor : a white dot lmao (for now...?)
    size_t b_pos = center_y * SOFTWARE_WIDTH + center_x;
    rd->frame_buffer[b_pos] = 0xffffffff;
}

static void text_draw_fb_shadow(
    Render *rd,
    const char *text,
    size_t pos_x,
    size_t pos_y,
    uint32_t color
) {
    text_draw_fb(rd, text, pos_x+1, pos_y+1, HUD_BG_COLOR);
    text_draw_fb(rd, text, pos_x, pos_y, color);
}

void player_draw_hud(PlayerStats *stats, Render *rd) {
    // HP bar
    text_draw_fb_shadow(rd, hp_label, HUD_PADDING, bottom_y, HUD_COLOR);
    for (int i = 0; i < stats->max_hp; i++) {
        int x = HUD_PADDING + TEXT_WIDTH(2)+i*3;
        uint32_t color = i >= stats->hp ? HUD_BG_COLOR : HUD_HP_COLOR;
        text_draw_fb_shadow(rd, hp_bar, x, bottom_y, color);
    }

    // Grenade count
    if (stats->grenades > 0) {
        char grenade_txt[20];
        sprintf(grenade_txt, "%d GRENADES", stats->grenades);
        int x = SOFTWARE_WIDTH - HUD_PADDING - TEXT_WIDTH(strlen(grenade_txt));
        text_draw_fb_shadow(rd, grenade_txt, x, bottom_y, HUD_COLOR);
    }

    // Money count
    char money_txt[20];
    sprintf(money_txt, "%d CREDITS", stats->money);
    int x = center_x - TEXT_WIDTH(strlen(money_txt))/2;
    text_draw_fb_shadow(rd, money_txt, x, HUD_PADDING-4, HUD_MONEY_COLOR);
}

#if FEATURE_RADAR
void player_draw_radar(Actor *self, Actor *target, Render *rd) {
    static const int radar_radius = 32;
    static const int center = center_x - letter_center_x;
    static const int y = HUD_PADDING+8;
    static const float max_angle = M_PI_2;

    // Draw side bars
    text_draw_fb_shadow(rd, hp_bar, center-radar_radius, y, HUD_COLOR);
    text_draw_fb_shadow(rd, hp_bar, center+radar_radius, y, HUD_COLOR);

    vec3 v = v3_sub(target->pos, rd->cam.pos);
    float angle = v3_angle(v, rd->cam.forward);

    // Skip wide angles
    if (angle < -max_angle || angle > max_angle) {
        return;
    }

    int x = center_x + (angle/max_angle)*radar_radius;

    uint32_t color = blink_frame++ % 16 < 8 ? HUD_MONEY_COLOR : 0xffffffff;
    text_draw_fb(rd, radar_dot, x, y-2, color);
}
#endif
