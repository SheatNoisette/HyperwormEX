#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
Program to bruteforce every configuration possible of ALSA to check if it's valid
gcc alsa_bruteforce.c -lasound -o alsa && ./alsa 2> out.csv
*/

const char *format_to_string(snd_pcm_format_t format)
{
    switch (format)
    {
    case SND_PCM_FORMAT_S8:
        return "S8";
    case SND_PCM_FORMAT_U8:
        return "U8";
    case SND_PCM_FORMAT_S16_LE:
        return "S16_LE";
    case SND_PCM_FORMAT_S16_BE:
        return "S16_BE";
    case SND_PCM_FORMAT_U16_LE:
        return "U16_LE";
    case SND_PCM_FORMAT_U16_BE:
        return "U16_BE";
    case SND_PCM_FORMAT_S24_LE:
        return "S24_LE";
    case SND_PCM_FORMAT_S24_BE:
        return "S24_BE";
    case SND_PCM_FORMAT_U24_LE:
        return "U24_LE";
    case SND_PCM_FORMAT_U24_BE:
        return "U24_BE";
    case SND_PCM_FORMAT_S32_LE:
        return "S32_LE";
    case SND_PCM_FORMAT_S32_BE:
        return "S32_BE";
    case SND_PCM_FORMAT_U32_LE:
        return "U32_LE";
    case SND_PCM_FORMAT_U32_BE:
        return "U32_BE";
    case SND_PCM_FORMAT_FLOAT_LE:
        return "FLOAT_LE";
    case SND_PCM_FORMAT_FLOAT_BE:
        return "FLOAT_BE";
    case SND_PCM_FORMAT_FLOAT64_LE:
        return "FLOAT64_LE";
    case SND_PCM_FORMAT_FLOAT64_BE:
        return "FLOAT64_BE";
    default:
        return "UNKNOWN";
    }
}

int try_alsa_config(const char *device, unsigned int rate, unsigned int channels, snd_pcm_format_t format)
{
    snd_pcm_t *handle = NULL;
    snd_pcm_hw_params_t *params;
    int rc;
    unsigned int val;
    int dir;

    rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0)
    {
        // fprintf(stderr, "Error opening device %s: %s\n", device, snd_strerror(rc));
        return rc;
    }

    snd_pcm_hw_params_alloca(&params);

    rc = snd_pcm_hw_params_any(handle, params);
    if (rc < 0)
    {
        // fprintf(stderr, "Error getting hardware parameters: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

    rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0)
    {
        // fprintf(stderr, "Error setting access type: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

    rc = snd_pcm_hw_params_set_format(handle, params, format);
    if (rc < 0)
    {
        // fprintf(stderr, "Error setting format: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

    rc = snd_pcm_hw_params_set_channels(handle, params, channels);
    if (rc < 0)
    {
        // fprintf(stderr, "Error setting channels: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

    val = rate;
    rc = snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
    if (rc < 0)
    {
        // fprintf(stderr, "Error setting rate: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

#if 0
    if (val != rate) {
        fprintf(stderr, "WARNING: rate mismatch %li vs expected %li\n", rate, val);
    }
#endif

    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0)
    {
        // fprintf(stderr, "Error setting hw parameters: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return rc;
    }

    snd_pcm_close(handle);

    return 0;
}

int main()
{
    const char *devices[] = {
        "default",
        "hw:0,0",
    };

    snd_pcm_format_t formats[] = {
        SND_PCM_FORMAT_S8,
        SND_PCM_FORMAT_U8,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_FORMAT_S16_BE,
        SND_PCM_FORMAT_U16_LE,
        SND_PCM_FORMAT_U16_BE,
        SND_PCM_FORMAT_S24_LE,
        SND_PCM_FORMAT_S24_BE,
        SND_PCM_FORMAT_U24_LE,
        SND_PCM_FORMAT_U24_BE,
        SND_PCM_FORMAT_S32_LE,
        SND_PCM_FORMAT_S32_BE,
        SND_PCM_FORMAT_U32_LE,
        SND_PCM_FORMAT_U32_BE,
        SND_PCM_FORMAT_FLOAT_LE,
        SND_PCM_FORMAT_FLOAT_BE,
        SND_PCM_FORMAT_FLOAT64_LE,
        SND_PCM_FORMAT_FLOAT64_BE,
    };

    unsigned int rates[] = {
        8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 192000};

    unsigned int channels[] = {
        1, 2, 4, 6, 8};

    int num_devices = sizeof(devices) / sizeof(devices[0]);
    int num_formats = sizeof(formats) / sizeof(formats[0]);
    int num_rates = sizeof(rates) / sizeof(rates[0]);
    int num_channels = sizeof(channels) / sizeof(channels[0]);

    fprintf(stdout, "Starting ALSA configuration bruteforce...\n");
    fprintf(stderr, "device,format,rate,channels,success\n");

    for (int i = 0; i < num_devices; ++i)
    {
        for (int j = 0; j < num_formats; ++j)
        {
            for (int k = 0; k < num_rates; ++k)
            {
                for (int l = 0; l < num_channels; ++l)
                {
                    const char *device = devices[i];
                    snd_pcm_format_t format = formats[j];
                    unsigned int rate = rates[k];
                    unsigned int channel = channels[l];

                    fprintf(stderr, "%s,%s,%u,%u,",
                            device, format_to_string(format), rate, channel);
                    fflush(stderr);

                    int rc = try_alsa_config(device, rate, channel, format);

                    if (rc == 0)
                    {
                        fprintf(stderr, "1\n");
                    }
                    else
                    {
                        fprintf(stderr, "0\n");
                    }
                }
            }
        }
    }

    fprintf(stdout, "Bruteforce done!\n");

    return 0;
}
