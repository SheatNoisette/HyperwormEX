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

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdbool.h>

#include "engine/const.h"

#include "sts_mixer.h"
#include "pl_synth.h"

// Sample buffer
#define SAMPLE_BUFFER 2048
// Sampling rate
#define SAMPLE_RATE 44100
// Number of channels
#define NUMBER_CHANNELS 2

typedef struct AudioSystem {
    void *platform_data;
    void *audio_mutex;
    void *audio_thread_id;

    float buf[SAMPLE_BUFFER];
    size_t pos;
    bool audio_end;
    sts_mixer_t mixer;
} AudioSystem;

typedef struct AudioSample {
    sts_mixer_sample_t sample;
} AudioSample;

AudioSystem *audio_start();
void audio_destroy(AudioSystem *as);
void audio_play(AudioSystem *as, AudioSample *aspl, float volume, float pitch, float pan, bool loop);
AudioSample *audio_build_sample(AudioSystem *as, pl_synth_song_t *sg);
void audio_stop(AudioSystem *as);

#endif
