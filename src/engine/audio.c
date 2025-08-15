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
#include <stddef.h>

#include "engine/const.h"
#include "engine/audio.h"
#include "engine/logging.h"
#include "engine/platform.h"

#define PL_SYNTH_IMPLEMENTATION
#define PL_SYNTH_SAMPLERATE SAMPLE_RATE
#include "pl_synth.h"
#define STS_MIXER_IMPLEMENTATION
#include "sts_mixer.h"

void audio_thread(void *arg) {
    AudioSystem *as = (AudioSystem *)arg;

    ENGINE_LOG("[AUDIO] Audio thread started\n");
    while (!as->audio_end) {
        platform_mutex_lock(as->audio_mutex);
        sts_mixer_mix_audio(&as->mixer, as->buf, SAMPLE_BUFFER);
        platform_mutex_unlock(as->audio_mutex);
        platform_audio_write(as, as->buf, SAMPLE_BUFFER);
        platform_audio_sync(as);
    }
}

AudioSystem *audio_start() {
    AudioSystem *as = malloc(sizeof(AudioSystem));

    // Start ALSA
    if (platform_audio_init(as) < 0) {
        ENGINE_LOG("[AUDIO] Platform audio initialization failed\n");
        free(as);
        return NULL;
    }

    if (platform_audio_writable(as) < 0) {
        ENGINE_LOG("[AUDIO] Audio not available\n");
        return NULL;
    }

    // Build PL Synth data
    pl_synth_init();

    // Init mixer
    sts_mixer_init(&as->mixer, SAMPLE_RATE);

    as->audio_mutex = platform_mutex_create();
    if (as->audio_mutex == NULL) {
        ENGINE_LOG("[AUDIO] Audio mutex failed\n");
        platform_audio_free(as);
        free(as);
        return NULL;
    }

    // Start audio thread
    as->audio_thread_id = platform_thread_create(audio_thread, as);
    if (as->audio_thread_id == NULL) {
        ENGINE_LOG("[AUDIO] Audio thread failed\n");
        platform_audio_free(as);
        free(as);
        return NULL;
    }

    ENGINE_LOG("[AUDIO] Audio Started!\n");

    return as;
}

void audio_play(AudioSystem *as, AudioSample *aspl, float volume, float pitch, float pan, bool loop) {
    if (aspl == NULL || as == NULL)
        return;
    platform_mutex_lock(as->audio_mutex);
    sts_mixer_play_sample(&as->mixer, &aspl->sample, volume, pitch, pan, loop);
    platform_mutex_unlock(as->audio_mutex);
}

AudioSample *audio_build_sample(AudioSystem *as, pl_synth_song_t *sg) {
    (void)as;

    AudioSample *aspl = malloc(sizeof(AudioSample));

#if SAFE_C
    if (aspl == NULL)
        return NULL;
#endif

    ENGINE_LOG("[AUDIO] Starting sample loading (%p)...\n", (void *)sg);

    size_t samples = pl_synth_song_len(sg);
	size_t buffer_size = samples * 2 * sizeof(int16_t);
	int16_t *output_samples = malloc(buffer_size);
	int16_t *temp_samples = malloc(buffer_size);

    if (output_samples == NULL || temp_samples == NULL) {
        ENGINE_LOG("[AUDIO] Failed to allocate buffers!\n");
        return NULL;
    }

    ENGINE_LOG("[AUDIO] Sample length: %li (%p)...\n", samples, (void *)sg);

    if (pl_synth_song(sg, output_samples, temp_samples) == 0) {
        ENGINE_LOG("[AUDIO] Could not generate audio!\n");
        return NULL;
    }

    ENGINE_LOG("[AUDIO] PL_Synth generated audio! (%p)!\n", (void *)sg);

    aspl->sample.length = samples;
    aspl->sample.data = (void *)output_samples;
    // @FIXME: We are writting as 16 bits sample, this should be converted to
    // 32 bits instead of 16 bits. As the are out of time, x2 is good enough
    // even if we lose stereo sound
    aspl->sample.frequency = SAMPLE_RATE * 2;

    ENGINE_LOG("[AUDIO] Sample %p loaded!\n", (void *)sg);

    free(temp_samples);

    return aspl;
}

void audio_stop(AudioSystem *as) {
    if (as == NULL)
        return;

    for (size_t voices = 0; voices < STS_MIXER_VOICES; voices++) {
        sts_mixer_stop_voice(&as->mixer, voices);
    }
}

void audio_destroy(AudioSystem *as) {
    if (!as)
        return;
    as->audio_end = true;
    platform_thread_join(as->audio_thread_id);
    platform_audio_free(as);
}
