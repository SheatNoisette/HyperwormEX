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

#include "game.h"
#include "sound_atlas.h"
#include "upgrades.h"

void init_newgame(Game *g) {
    // Initialize a new game
    g->round = 0;
    player_reset_stats(g->player_stats);
    reset_upgrades();

    // Reset audio
    audio_stop(g->rd->as);

#if FEATURE_MUSIC
    // Start the music
    audio_play(g->rd->as, sad_get_sound(SAD_MAIN_SONG), 0.25f, 1, 0, true);
#endif
}

GameState update_newgame(Game *g) {
    return GS_LEVEL;
}
