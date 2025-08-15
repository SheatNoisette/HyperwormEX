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
#include <string.h>

#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "engine/text.h"
#include "engine/platform.h"
#include "engine/keys.h"
#include "flags.h"
#include "game.h"
#include "menu.h"
#include "player.h"
#include "sound_atlas.h"
#include "tunnel.h"

static const char *title_text = "HYPERWORM EX";
static Sprite *title_sprite = NULL;

// Title text transformations (dirty globals ewwww gross)
static float angle;
static float scale;
static float timer;

void init_menu(Game *g) {
    ENGINE_LOG("LOADING: MENU\n");

    // Reset stuff
    timer = 0;

    if (!title_sprite) {
        title_sprite = sprite_new_text(title_text, HUD_COLOR);
    }

    audio_stop(g->rd->as);
    audio_play(g->rd->as, sad_get_sound(SAD_MAIN_SONG), 0.25f, 0.5f, 0, true);
}

GameState update_menu(Game *g) {
    // Caveman seed generation: call rand() every frame on the menu
    rand();

    // Update stuff
    timer += g->rd->dt;
    angle = sinf(timer * 3) * M_PI / 24.0f;
    scale = (sinf(timer * 6) + 1) * 0.75f + 2;

    if (platform_key(g->rd, EKEY_SPACE)) {
#if FEATURE_UI_SOUNDS
        audio_play(g->rd->as, sad_get_sound(SAD_UI_BEEP), 1, 1.5f, 0.f, false);
#endif
        return GS_NEWGAME;
    }

    if (platform_key(g->rd, EKEY_Q)) {
        return GS_EXIT;
    }

    // Tunnel effect background
    tunnel_draw(g->rd);

    // Draw title
    sprite_draw_rotation(g->rd, title_sprite, SOFTWARE_WIDTH/2, SOFTWARE_HEIGHT/3 - 10, angle, scale);

    // Draw text
    text_draw_centered_fb(g->rd, "PRESS SPACE TO START", SOFTWARE_HEIGHT - 67, 0xFFFFFF);
    text_draw_centered_fb(g->rd, "PRESS Q TO QUIT", SOFTWARE_HEIGHT - 47, 0xFFFFFF);
    text_draw_centered_fb(g->rd, "2025 HERALD AND SHEATNOISETTE", SOFTWARE_HEIGHT - 20, HUD_MONEY_COLOR);

    return GS_MENU;
}

void exit_menu(Game *game) {
    // Nothing
}
