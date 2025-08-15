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
#include <stdio.h>

#include "engine/render.h"
#include "engine/text.h"
#include "engine/keys.h"
#include "engine/platform.h"
#include "flags.h"
#include "game.h"
#include "player.h"
#include "tunnel.h"

#if FEATURE_STATS
static float timer;
static const float appear_delay = 0.3f;

#define STAT_LINES 5
static char stats_txt[STAT_LINES][32];
#endif

void init_end(Game *g) {
#if FEATURE_STATS
    timer = 0;

    sprintf(stats_txt[0], "REACHED LEVEL %d", g->round);
    sprintf(stats_txt[1], "COLLECTED %d CREDITS", g->player_stats->money_collected);
    sprintf(stats_txt[2], "KILLED %d ENEMIES", g->player_stats->enemies_killed);
    sprintf(stats_txt[3], "TOOK %d DAMAGE", g->player_stats->damage_taken);
    sprintf(stats_txt[4], "FIRED %d GRENADES", g->player_stats->grenades_fired);
#endif
}

GameState update_end(Game *g) {
#if FEATURE_STATS
    timer += GET_DELTATIME(g->rd);
#endif

    Render *rd = g->rd;

    // Exit
    if (platform_key(g->rd, EKEY_BACKSPACE)) {
        return GS_MENU;
    }

    tunnel_draw(rd);

    if (g->player_stats->won) {
        text_draw_centered_fb(rd, "YOU WIN", 16, HUD_MONEY_COLOR);
    } else {
        text_draw_centered_fb(rd, "YOU LOSE...", 16, HUD_MONEY_COLOR);
    }

#if FEATURE_STATS
    int x = 32;
    for (int i = 0; i < STAT_LINES; i++) {
        if (timer > appear_delay * (i+1)) {
            text_draw_centered_fb(rd, stats_txt[i], x + i*16, HUD_COLOR);
        }
    }
#endif

    text_draw_centered_fb(rd, "BACKSPACE - EXIT", 155, HUD_MONEY_COLOR);

    return GS_END;
}
