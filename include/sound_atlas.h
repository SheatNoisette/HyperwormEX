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

#ifndef __SOUND_ATLAS_H__
#define __SOUND_ATLAS_H__

#include "engine/const.h"
#include "engine/audio.h"
#include "flags.h"
#include "pl_synth.h"

typedef enum SoundAtlasDefinition {
    SAD_PLAYER_SHOOT = 0,
    SAD_EXPLOSION,
    SAD_GRENADE_THROW,
    SAD_PLAYER_JUMP,
    SAD_ENEMY_HIT,
    SAD_PLAYER_HIT,
#if FEATURE_MUSIC
    SAD_MAIN_SONG,
#endif
#if FEATURE_STEP_SOUNDS
    SAD_PLAYER_WALK,
#endif
#if FEATURE_UI_SOUNDS
    SAD_UI_BEEP,
#endif
    /* Internal number, DO NOT TOUCH */
    SAD_NUMBER,
} SoundAtlasDefinition;

// Sound entry
typedef struct SoundAtlasEntry {
    SoundAtlasDefinition def;
    pl_synth_song_t *song;
} SoundAtlasEntry;

void sad_build(AudioSystem *as);
AudioSample *sad_get_sound(SoundAtlasDefinition def);

#endif
