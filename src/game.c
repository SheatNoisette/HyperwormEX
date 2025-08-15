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

#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"
#include "engine/text.h"
#include "engine/platform.h"
#include "game.h"
#include "level.h"
#include "menu.h"
#include "player.h"
#include "sound_atlas.h"
#include "sprite_atlas.h"
#include "textures.h"
#include "tunnel.h"
#include "wormhole.h"

typedef struct {
    void (*init)(Game*);
    GameState (*update)(Game*);
    void (*exit)(Game*);
} GameStateFuncs;

static const GameStateFuncs state_table[STATE_COUNT] = {
    { 0 }, // GS_INIT is empty
    { init_menu,     update_menu,                }, // GS_MENU
    { init_newgame,  update_newgame,             }, // GS_NEWGAME
    { init_level,    update_level,    exit_level }, // GS_LEVEL
    { init_wormhole, update_wormhole,            }, // GS_WORMHOLE
    { init_lose,     update_lose,                }, // GS_LOSE
    { init_end,      update_end,                 }, // GS_END
    { 0 }, // GS_EXIT is empty
};

static GameState current_state = GS_INIT;

void change_state(Game *g, GameState new_state) {
    if (current_state == new_state) {
        return;
    }

    // Clean up old state
    if (state_table[current_state].exit) {
        state_table[current_state].exit(g);
    }

    // Switch state
    current_state = new_state;

    // Initialize new state
    if (state_table[current_state].init) {
        state_table[current_state].init(g);
    }
}

#ifdef USE_MAIN
int main(void) {
#else
void _start(void) {
    // Align the stack
    asm volatile("sub $8, %rsp \t\n");
#endif

    ENGINE_LOG("[GAME] Starting...\n");

    // Global setup
    Render *rd = new_render(SOFTWARE_WIDTH, SOFTWARE_HEIGHT);
    // Print a simple message for loading
    text_draw_centered_fb(rd, "PRECACHING...", SOFTWARE_HEIGHT/2 - 5, 0xFFFFFF);
    // Push the surface by simulating a frame
    platform_update(rd);
    platform_render_begin(rd);
    platform_render_end(rd);

    // Start audio
    rd->as = audio_start();

    // Load assets
    load_voxel_textures(rd);
    // Build sprite atlas
    spr_build();
    // Build sound atlas
    sad_build(rd->as);
    // Initialize tunnel effect
    tunnel_init();

    // Setup game object
    Game g;
    g.rd = rd;
    PlayerStats stats;
    g.player_stats = &stats;

    // Load menu
    change_state(&g, GS_MENU);

    // Event loop
    while (current_state != GS_EXIT) {
        // If the platform don't want to update anymore, yeet
        if (!platform_update(rd)) {
            break;
        }

        platform_render_begin(rd);
        // Tick the current state
        GameState new_state = current_state;
        if (state_table[current_state].update) {
            new_state = state_table[current_state].update(&g);
        }
        platform_render_end(rd);

        // If the state needs to be changed, do it
        if (new_state != current_state) {
            change_state(&g, new_state);
        }
    }

    ENGINE_LOG("Quitting.\n");

    audio_destroy(rd->as);
    render_free(rd);

#ifdef USE_MAIN
    return 0;
#else
    // Call syscall 231, exit_group, which stops everything with threads
    asm volatile("mov $231, %rax \t\n");
    asm volatile("xor %rdi, %rdi \t\n");
    asm volatile("syscall \t\n");
#endif
}
