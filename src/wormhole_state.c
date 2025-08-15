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

#include <stdio.h>

#include "engine/const.h"
#include "engine/render.h"
#include "engine/text.h"
#include "engine/keys.h"
#include "engine/platform.h"
#include "game.h"
#include "player.h"
#include "sound_atlas.h"
#include "tunnel.h"
#include "upgrades.h"

static size_t selected_upgrade = 0;
static Upgrade *u;
static char upgrade_txt[32];
static char pager_txt[32];
static bool can_afford = false;

// Dir : -1 for left, +1 for right
void change_upgrade(Game *g, int dir) {
    selected_upgrade = (selected_upgrade + NUM_UPGRADES + dir) % NUM_UPGRADES;
    u = get_upgrade(selected_upgrade);

    // Generate text
    sprintf(upgrade_txt, "%d CREDITS - PRESS SPACE", u->cost + u->extra_cost);
    sprintf(pager_txt, "< LEFT  %zu/%d  RIGHT >", selected_upgrade+1, NUM_UPGRADES);

    can_afford = g->player_stats->money >= u->cost + u->extra_cost;
}

void init_wormhole(Game *g) {
    // Heal the player
    g->player_stats->hp = g->player_stats->max_hp;
    // Refill some grenades
    g->player_stats->grenades += 3;

    // Load upgrade
    change_upgrade(g, 0);

    // Prevent accidental purchase
    // g->rd->keys[XK_space] = 0;
    // THIS IS UGLY AF
    while (platform_key_pressed(g->rd, EKEY_SPACE) == true);
}

void buy_upgrade(Game *g, Upgrade *u) {
    g->player_stats->money -= u->cost + u->extra_cost;
    u->apply(u, g->player_stats);

    change_upgrade(g, 0);
}

GameState update_wormhole(Game *g) {
    Render *rd = g->rd;

    bool key_up = platform_key_pressed(rd, EKEY_Z) || platform_key_pressed(rd, EKEY_W) || platform_key_pressed(rd, EKEY_UP);
    bool key_down = platform_key_pressed(rd, EKEY_S) || platform_key_pressed(rd, EKEY_DOWN);
    bool key_left = platform_key_pressed(rd, EKEY_Q) || platform_key_pressed(rd, EKEY_A) || platform_key_pressed(rd, EKEY_LEFT);
    bool key_right = platform_key_pressed(rd, EKEY_D) || platform_key_pressed(rd, EKEY_RIGHT);
    bool key_select = platform_key_pressed(rd, EKEY_SPACE) || platform_key_pressed(rd, EKEY_ENTER);
    bool key_skip = platform_key(rd, EKEY_BACKSPACE);

    // Change selection
    int dir = key_right - key_left;
    if (dir) {
#if FEATURE_UI_SOUNDS
        audio_play(rd->as, sad_get_sound(SAD_UI_BEEP), 1, 1, 0.f, false);
#endif
        change_upgrade(g, dir);
    }

    // Buy upgrade
    if (key_select && can_afford) {
#if FEATURE_UI_SOUNDS
        audio_play(rd->as, sad_get_sound(SAD_UI_BEEP), 1, 1.5f, 0.f, false);
#endif
        buy_upgrade(g, u);
        return GS_WORMHOLE;
    }

    // Skip
    if (key_skip) {
#if FEATURE_UI_SOUNDS
        audio_play(rd->as, sad_get_sound(SAD_UI_BEEP), 1, 1.5f, 0.f, false);
#endif
        // Go to the next level
        return GS_LEVEL;
    }

    // Win condition
    if (g->player_stats->won) {
        return GS_END;
    }

    tunnel_draw(rd);

    player_draw_hud(g->player_stats, rd);

    // Draw upgrade
    const int x = 80;
    int y = SOFTWARE_HEIGHT / 3;
    text_draw_fb(rd, u->name, x, y, 0xFFFFFF);
    y += 10;
    text_draw_fb(rd, upgrade_txt, x, y, can_afford ? HUD_MONEY_COLOR : HUD_HP_COLOR);

    // Draw pager
    y += 25;
    text_draw_centered_fb(rd, pager_txt, y, 0xFFFFFF);

    text_draw_fb(rd, "BACKSPACE - SKIP", 112, 155, HUD_MONEY_COLOR);

    return GS_WORMHOLE;
}

void exit_wormhole(Game *g) {

}
