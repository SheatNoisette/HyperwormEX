#ifndef __SFX_H__
#define __SFX_H__

#include "flags.h"
#include "pl_synth.h"

// Player gun sound
pl_synth_song_t gun_sound = {
	.row_len = 5513,
	.num_tracks = 2,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {0,0,0,0,0,3,0,0,0,0,0,1,255,0,219,134,166,0,11025,255,0,0,16,78,0,1,1,117},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {152}}
			}
		},
		{
			.synth = {8,0,0,1,255,1,8,0,0,0,0,0,80,0,27,10902,142,1,265,255,1,34,0,0,1,1,12,255,1},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {135}}
			}
		}
	}
};

// Explosion grenade
pl_synth_song_t sfx_explosion = {
	.row_len = 5513,
	.num_tracks = 2,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {7,2,0,0,0,3,8,0,0,0,0,0,255,0,6351,42752,255,3,2522,176,1,48,15,255,0,1,14,255,1},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {99}}
			}
		},
		{
			.synth = {7,0,0,1,192,2,7,0,0,0,0,0,0,0,92,21860,192},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {144}}
			}
		}
	}
};

// Launch grenade
pl_synth_song_t sfx_launch_grenade = {
	.row_len = 5513,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {11,9,0,1,255,3,8,0,0,0,255,1,255,1191,188,8124,196,4,11025,63},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {99}}
			}
		}
	}
};

// Player jump
pl_synth_song_t sfx_player_jump = {
	.row_len = 5513,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {3,0,0,0,255,0,4,0,0,0,255,0,255,2715,0,6999,220,3,2720,92,0,199,0,0,0,1},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {99}}
			}
		}
	}
};

// Enemy hit
pl_synth_song_t sfx_enemy_hit = {
	.row_len = 2355,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {6,0,0,0,255,2,12,2,11,1,40,3,0,2541,0,6351,188,0,0,255,0,0,0,0,1,0,6,129},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {0,190}}
			}
		}
	}
};

// Player get hurt
pl_synth_song_t sfx_player_hit = {
	.row_len = 2646,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {6,0,0,1,255,3,8,1,40,0,36,1,0,74,0,8050,255,1,0,255,3,97,0,0,1},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {175}}
			}
		}
	}
};

#if FEATURE_STEP_SOUNDS
// Player walk
pl_synth_song_t sfx_player_walk = {
	.row_len = 5513,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {0,0,0,0,0,3,0,0,0,1,0,0,202,36,1408,36,210,3,971,255,3,43,3},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {103}}
			}
		}
	}
};
#endif

#if FEATURE_UI_SOUNDS
// UI beep
pl_synth_song_t sfx_ui_beep = {
	.row_len = 5513,
	.num_tracks = 1,
	.tracks = (pl_synth_track_t[]){
		{
			.synth = {7,0,0,0,192,1,8,7,0,0,68,1,0,36,1919,485,125},
			.sequence_len = 1,
			.sequence = (uint8_t[]){1},
			.patterns = (pl_synth_pattern_t[]){
				{.notes = {135}}
			}
		}
	}
};
#endif

#endif
