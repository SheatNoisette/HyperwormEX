/*

Copyright (c) 2024, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT

Based on Sonant, published under the Creative Commons Public License
(c) 2008-2009 Jake Taylor [ Ferris / Youth Uprising ] 


-- Synopsis

// Define `PL_SYNTH_IMPLEMENTATION` in *one* C/C++ file before including this
// library to create the implementation.

#define PL_SYNTH_IMPLEMENTATION
#include "pl_synth.h"

// Initialize the lookup table for the oscillators
void *synth_tab = malloc(PL_SYNTH_TAB_SIZE);
pl_synth_init(synth_tab);

// A sound is described by an instrument (synth), the row_len in samples and
// a note.
pl_synth_sound_t sound = {
	.synth = {7,0,0,0,192,0,7,0,0,0,192,0,0,200,2000,20000,192}, 
	.row_len = 5168, 
	.note = 135
};

// Determine the number of the samples for a sound effect and allocate the
// sample buffer for both (stereo) channels
int num_samples = pl_synth_sound_len(&sound);
uint16_t *sample_buffer = malloc(num_samples * 2 * sizeof(uint16_t));

// Generate the samples
pl_synth_sound(&sound, sample_buffer);

See below for a documentation of all functions exposed by this library.
*/

#ifndef PL_SYNTH_H
#define PL_SYNTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PL_SYNTH_TAB_LEN 4096
#define PL_SYNTH_TAB_SIZE (sizeof(float) * PL_SYNTH_TAB_LEN * 4)

typedef struct {
	uint8_t osc0_oct;
	uint8_t osc0_det;
	uint8_t osc0_detune;
	uint8_t osc0_xenv;
	uint8_t osc0_vol;
	uint8_t osc0_waveform;

	uint8_t osc1_oct;
	uint8_t osc1_det;
	uint8_t osc1_detune;
	uint8_t osc1_xenv;
	uint8_t osc1_vol;
	uint8_t osc1_waveform;

	uint8_t noise_fader;

	uint32_t env_attack;
	uint32_t env_sustain;
	uint32_t env_release;
	uint32_t env_master;

	uint8_t fx_filter;
	uint32_t fx_freq;
	uint8_t fx_resonance;
	uint8_t fx_delay_time;
	uint8_t fx_delay_amt;
	uint8_t fx_pan_freq;
	uint8_t fx_pan_amt;

	uint8_t lfo_osc_freq;
	uint8_t lfo_fx_freq;
	uint8_t lfo_freq;
	uint8_t lfo_amt;
	uint8_t lfo_waveform;
} pl_synth_t;

typedef struct {
	pl_synth_t synth;
	uint32_t row_len;
	uint8_t note;
} pl_synth_sound_t;

typedef struct {
	uint8_t notes[32];
} pl_synth_pattern_t;

typedef struct {
	pl_synth_t synth;
	uint32_t sequence_len;
	uint8_t *sequence;
	pl_synth_pattern_t *patterns;
} pl_synth_track_t;

typedef struct {
	uint32_t row_len;
	uint8_t num_tracks;
	pl_synth_track_t *tracks;
} pl_synth_song_t;

// Initialize the lookup table for all instruments. This needs to be done only
// once. The table will be written to the memory pointed to by tab_buffer, which
// must be PL_SYNTH_TAB_LEN elements long or PL_SYNTH_TAB_SIZE bytes in size.
void pl_synth_init();

// Determine the number of samples needed for one channel of a particular sound
// effect.
int pl_synth_sound_len(pl_synth_sound_t *sound);

// Generate a stereo sound into the buffer pointed to by samples. The buffer 
// must be at least pl_synth_sound_len() * 2 elements long.
int pl_synth_sound(pl_synth_sound_t *sound, int16_t *samples);

// Determine the number of samples needed for one channel of a particular song.
int pl_synth_song_len(pl_synth_song_t *song);

// Generate a stereo song into the buffer pointed to by samples, with temporary
// storage provided to by temp_samples. The buffers samples and temp_samples 
// must each be at least pl_synth_song_len() * 2 elements long.
int pl_synth_song(pl_synth_song_t *song, int16_t *samples, int16_t *temp_samples);

#ifdef __cplusplus
}
#endif
#endif /* PL_SYNTH_H */
