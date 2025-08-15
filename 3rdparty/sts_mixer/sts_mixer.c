#include <stdbool.h>
#include <stddef.h>

#include "sts_mixer.h"
#include "engine/utils.h"

enum
{
    STS_MIXER_VOICE_STOPPED,
    STS_MIXER_VOICE_PLAYING
};

static float sts_mixer__clamp_sample(const float sample)
{
    return clampf(sample, -1.0f, 1.0f);
}

static float sts_mixer__get_sample(sts_mixer_sample_t *sample, unsigned int position)
{
    return (float)((short *)sample->data)[position] / 32767.0f;
}

static void sts_mixer__reset_voice(sts_mixer_t *mixer, const int i)
{
    sts_mixer_voice_t *voice = &mixer->voices[i];
    voice->state = STS_MIXER_VOICE_STOPPED;
    voice->sample = NULL;
    voice->position = voice->gain = voice->pitch = voice->pan = 0.0f;
    voice->loop = false;
}

static int sts_mixer__find_free_voice(sts_mixer_t *mixer)
{
    static int i = 0;

    do {
        i = (i + 1) % STS_MIXER_VOICES;
    // Skip looping voices, we don't want to overwrite those (e.g. BGM etc)
    } while (mixer->voices[i].loop);

    return i;
}

void sts_mixer_init(sts_mixer_t *mixer, unsigned int frequency)
{
    int i;

    for (i = 0; i < STS_MIXER_VOICES; ++i)
        sts_mixer__reset_voice(mixer, i);

    mixer->frequency = frequency;
    mixer->gain = 1.0f;
}

void sts_mixer_shutdown(sts_mixer_t *mixer)
{
    sts_mixer_init(mixer, 0);
}

int sts_mixer_get_active_voices(sts_mixer_t *mixer)
{
    int i, active;
    for (i = 0, active = 0; i < STS_MIXER_VOICES; ++i)
    {
        if (mixer->voices[i].state != STS_MIXER_VOICE_STOPPED)
            ++active;
    }
    return active;
}

int sts_mixer_play_sample(sts_mixer_t *mixer, sts_mixer_sample_t *sample, float gain, float pitch, float pan, bool loop)
{
    int i;
    sts_mixer_voice_t *voice;

    i = sts_mixer__find_free_voice(mixer);
    if (i >= 0)
    {
        voice = &mixer->voices[i];
        voice->gain = gain;
        voice->pitch = clampf(pitch, 0.1f, 10.0f);
        voice->pan = clampf(pan * 0.5f, -0.5f, 0.5f);
        voice->position = 0.0f;
        voice->sample = sample;
        voice->loop = loop;
        voice->state = STS_MIXER_VOICE_PLAYING;
    }
    return i;
}


void sts_mixer_stop_voice(sts_mixer_t *mixer, int voice)
{
    if (voice >= 0 && voice < STS_MIXER_VOICES)
        sts_mixer__reset_voice(mixer, voice);
}

void sts_mixer_stop_sample(sts_mixer_t *mixer, sts_mixer_sample_t *sample)
{
    int i;

    for (i = 0; i < STS_MIXER_VOICES; ++i)
    {
        if (mixer->voices[i].sample == sample)
            sts_mixer__reset_voice(mixer, i);
    }
}

void sts_mixer_mix_audio(sts_mixer_t *mixer, void *output, unsigned int samples)
{
    sts_mixer_voice_t *voice;
    unsigned int i, position;
    float left, right, advance, sample;
    char *out_8 = (char *)output;
    short *out_16 = (short *)output;

    // mix all voices
    advance = 1.0f / (float)mixer->frequency;
    for (; samples > 0; --samples)
    {
        left = right = 0.0f;
        for (i = 0; i < STS_MIXER_VOICES; ++i)
        {
            voice = &mixer->voices[i];
            if (voice->state == STS_MIXER_VOICE_PLAYING)
            {
                position = (int)voice->position;
                // TODO length*2 is a temporary hack because stereo not working
                if (position < voice->sample->length * 2)
                {
                    sample = sts_mixer__clamp_sample(sts_mixer__get_sample(voice->sample, position) * voice->gain);
                    left += sts_mixer__clamp_sample(sample * (0.5f - voice->pan));
                    right += sts_mixer__clamp_sample(sample * (0.5f + voice->pan));
                    voice->position += (float)voice->sample->frequency * advance * voice->pitch;
                }
                else if (voice->loop)
                {
                    // We are looping, reset
                    voice->position = 0.f;
                }
                else
                {
                    sts_mixer__reset_voice(mixer, i);
                }
            }
        }

        // write to buffer
        left = sts_mixer__clamp_sample(left);
        right = sts_mixer__clamp_sample(right);

        *out_16++ = (short)(left * 32767.0f);
        *out_16++ = (short)(right * 32767.0f);
    }
}
