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

#include <stddef.h>
#include <stdio.h>

#include "engine/actor.h"
#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"
#include "engine/sprite.h"
#include "game.h"
#include "level.h"
#include "palettes.h"
#include "player.h"
#include "wormhole.h"

#define intro_txt_count 4
static const char *intro_txt[intro_txt_count] = {
    " SHOOT WORMS ",
    "BLOW STUFF UP",
    "FIND WORMHOLE",
    "  PAY DEBTS  ",
};
static Sprite *intro_sprites[intro_txt_count] = {0};
// Time per text item
static const float intro_time = 1.75f;
static float intro_timer;

static void init_intro() {
    intro_timer = 0;

    if (intro_sprites[0] != NULL) {
        return;
    }

    for (int i = 0; i < intro_txt_count; i++) {
        intro_sprites[i] = sprite_new_text(intro_txt[i], 0xffffff);
    }
}

static void init_lights(Render *rd) {
    static const vec3 white = (vec3){1, 1, 1};

    // Player's light
    PLAYER_LIGHT(rd).color = white;
    PLAYER_LIGHT(rd).intensity = PLAYER_LIGHT_INTENSITY;

    // Explosion light
    BLAST_LIGHT(rd).color = white;
    BLAST_LIGHT(rd).intensity = 0; // Starts disabled

    rd->num_lights = 2;
}

void init_level(Game *g) {
    ENGINE_LOG("LOADING: LEVEL\n");

    init_intro();
    g->round++;

#if ENABLE_LUT
    // Change palettes every level
    change_palette();
#endif

    g->rd->num_actors = 0;

    init_lights(g->rd);

    GenData gen = generate_rooms(g->rd);
    level_spawn_actors(g, &gen);
    free_rooms(gen.room_tree);
}

GameState update_level(Game *g) {
    Render *rd = g->rd;

    // Update actors
    actor_update_all(g->rd->actors, g->rd);
    // Update global lights
    BLAST_LIGHT(rd).intensity -= BLAST_LIGHT_FALLOFF * GET_DELTATIME(rd);

    // Render the world
    render_world(rd);
    // Render actors
    actor_draw_all(rd->actors, rd);
    // Draw the gun + hud
    player_draw_gun(g->player, rd);
    player_draw_hud(g->player_stats, rd);
#if FEATURE_RADAR
    player_draw_radar(g->player, g->wormhole, rd);
#endif

    // Draw intro
    if (g->round == 1) {
        intro_timer += GET_DELTATIME(rd);
        size_t index = intro_timer / intro_time;

        if (index < intro_txt_count) {
            sprite_draw_rotation(
                rd, intro_sprites[index],
                SOFTWARE_WIDTH/2, SOFTWARE_HEIGHT/2,
                0, 3
            );
        }
    }

    // Lose condition
    if (!g->player->is_active) {
        return GS_LOSE;
    }

    // Change levels
    if (ACTOR_DATA(g->wormhole, WormholeData)->is_triggered) {
        return GS_WORMHOLE;
    }

    return GS_LEVEL;
}

void exit_level(Game *g) {
    actor_destroy_all(g->rd->actors, g->rd);
    world_free(g->rd->world);
#if SAFE_C
    g->rd->world = NULL;
    g->player = NULL;
    g->wormhole = NULL;
#endif
}
