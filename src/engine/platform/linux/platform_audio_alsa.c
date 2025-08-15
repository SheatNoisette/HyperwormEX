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

#include <alsa/asoundlib.h>

#include "engine/audio.h"
#include "engine/logging.h"

typedef struct {
    snd_pcm_t *pcm;
} LinuxAudioData;

int platform_audio_init(AudioSystem *as) {
    LinuxAudioData *pad = malloc(sizeof(LinuxAudioData));
    if (pad == NULL) {
        ENGINE_LOG("[AUDIO] Failed to allocate platform audio data\n");
        return -1;
    }
    as->platform_data = pad;

    if (snd_pcm_open(&pad->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        ENGINE_LOG("[AUDIO] Audio ALSA failed\n");
        free(pad);
        return -1;
    }

    if (snd_pcm_set_params(
        pad->pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        NUMBER_CHANNELS,
        SAMPLE_RATE,
        1, 100000
    ) < 0) {
        ENGINE_LOG("[AUDIO] Audio ctx failed\n");
        snd_pcm_close(pad->pcm);
        free(pad);
        return -1;
    }

    ENGINE_LOG("[AUDIO] Platform Audio Initialized!\n");
    return 0;
}

int platform_audio_writable(AudioSystem *as) {
    LinuxAudioData *pad = (LinuxAudioData *)as->platform_data;
    int n = snd_pcm_avail(pad->pcm);
    if (n < 0) {
        snd_pcm_recover(pad->pcm, n, 0);
    }
    return n;
}
void platform_audio_write(AudioSystem *as, float *buf, size_t n) {
    LinuxAudioData *pad = (LinuxAudioData *)as->platform_data;
    int r = snd_pcm_writei(pad->pcm, buf, n);
    if (r < 0)
        snd_pcm_recover(pad->pcm, r, 0);
}

void platform_audio_sync(AudioSystem *as) {
    // Nothing to do
}

void platform_audio_free(AudioSystem *as) {
    LinuxAudioData *pad = (LinuxAudioData *)as->platform_data;
    if (pad) {
        snd_pcm_close(pad->pcm);
        free(pad);
        as->platform_data = NULL;
    }
}
