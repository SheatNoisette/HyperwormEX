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
#include <stdint.h>

#include "engine/audio.h"
#include "flags.h"
#include "sound_atlas.h"
#include "sounds/sfx.h"
#include "sounds/songs.h"

// Add sounds here and update SoundAtlasDefinition {
static SoundAtlasEntry sad_song[SAD_NUMBER] = {
    {SAD_PLAYER_SHOOT, &gun_sound},
    {SAD_EXPLOSION, &sfx_explosion},
    {SAD_GRENADE_THROW, &sfx_launch_grenade},
    {SAD_PLAYER_JUMP, &sfx_player_jump},
    {SAD_ENEMY_HIT, &sfx_enemy_hit},
    {SAD_PLAYER_HIT, &sfx_player_hit},
#if FEATURE_MUSIC
    {SAD_MAIN_SONG , &game_theme_1},
#endif
#if FEATURE_STEP_SOUNDS
    {SAD_PLAYER_WALK, &sfx_player_walk},
#endif
#if FEATURE_UI_SOUNDS
    {SAD_UI_BEEP, &sfx_ui_beep},
#endif
};
// }

// Complete pre-computed atlas
static AudioSample *sad_built_sound[SAD_NUMBER];

// Build the sound atlas
void sad_build(AudioSystem *as) {
    for (size_t song = 0; song < (size_t)SAD_NUMBER; song++) {
        sad_built_sound[sad_song[song].def] = audio_build_sample(as, sad_song[song].song);
    }
}

// Get a audio sample from it
AudioSample *sad_get_sound(SoundAtlasDefinition def) {
    return sad_built_sound[def];
}
