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
#include <windows.h>
#include <mmsystem.h>

#include "engine/audio.h"
#include "engine/logging.h"
#include "engine/platform.h"

#define NUM_BUFFERS 2
#define BUFFER_SIZE SAMPLE_BUFFER *NUMBER_CHANNELS * sizeof(int16_t)

typedef struct
{
    HWAVEOUT hWaveOut;
    WAVEHDR waveHeaders[NUM_BUFFERS];
    int16_t *buffers[NUM_BUFFERS];
    int currentBuffer;
    HANDLE bufferEvent;
} WindowsAudioData;

static WAVEFORMATEX createWaveFormat()
{
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = NUMBER_CHANNELS;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;
    return wfx;
}

int platform_audio_init(AudioSystem *as)
{
    WindowsAudioData *wad = malloc(sizeof(*wad));
    if (!wad)
    {
        ENGINE_LOG("[AUDIO] Failed to allocate Windows audio data\n");
        return -1;
    }
    as->platform_data = wad;

    wad->bufferEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (wad->bufferEvent == NULL)
    {
        ENGINE_LOG("[AUDIO] Failed to create buffer event\n");
        free(wad);
        return -1;
    }

    WAVEFORMATEX wfx = createWaveFormat();
    MMRESULT result = waveOutOpen(&wad->hWaveOut, WAVE_MAPPER, &wfx,
                                  (DWORD_PTR)wad->bufferEvent, 0,
                                  CALLBACK_EVENT);
    if (result != MMSYSERR_NOERROR)
    {
        ENGINE_LOG("[AUDIO] Failed to open wave device: %d\n", result);
        CloseHandle(wad->bufferEvent);
        free(wad);
        return -1;
    }

    // Allocate and initialize audio buffers
    wad->currentBuffer = 0;
    for (size_t i = 0; i < NUM_BUFFERS; i++)
    {
        wad->buffers[i] = malloc(BUFFER_SIZE);
        if (!wad->buffers[i])
        {
            ENGINE_LOG("[AUDIO] Failed to allocate audio buffer %d\n", i);
            for (int j = 0; j < i; j++)
                free(wad->buffers[j]);
            waveOutClose(wad->hWaveOut);
            CloseHandle(wad->bufferEvent);
            free(wad);
            return -1;
        }

        ZeroMemory(&wad->waveHeaders[i], sizeof(WAVEHDR));
        wad->waveHeaders[i].lpData = (LPSTR)wad->buffers[i];
        wad->waveHeaders[i].dwBufferLength = BUFFER_SIZE;
        wad->waveHeaders[i].dwFlags = 0;

        result = waveOutPrepareHeader(wad->hWaveOut, &wad->waveHeaders[i], sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR)
        {
            ENGINE_LOG("[AUDIO] Failed to prepare header %d: %d\n", i, result);
            for (size_t j = 0; j <= i; j++)
                free(wad->buffers[j]);
            waveOutClose(wad->hWaveOut);
            CloseHandle(wad->bufferEvent);
            free(wad);
            return -1;
        }
    }

    return 0;
}

int platform_audio_writable(AudioSystem *as)
{
    WindowsAudioData *wad = (WindowsAudioData *)as->platform_data;
    return (wad && wad->hWaveOut) ? 0 : -1;
}

void platform_audio_write(AudioSystem *as, float *buf, size_t n)
{
    WindowsAudioData *wad = (WindowsAudioData *)as->platform_data;
    WAVEHDR *header = &wad->waveHeaders[wad->currentBuffer];

    if (header->dwFlags & WHDR_INQUEUE) {
        platform_audio_sync(as);
    }

    int16_t *outBuf = wad->buffers[wad->currentBuffer];
    volatile uint16_t *buf_new = (uint16_t *)buf;
    for (size_t i = 0; i < n; i++)
    {
        // @TODO: ALREADY IN 16 BITS BECAUSE I MESSED UP IN THE ALSA VERSION
        outBuf[i * 2] = (int16_t)buf_new[i * 2];
        outBuf[i * 2 + 1] = (int16_t)buf_new[i * 2 + 1];
    }

    waveOutWrite(wad->hWaveOut, header, sizeof(WAVEHDR));
    wad->currentBuffer = (wad->currentBuffer + 1) % NUM_BUFFERS;
}

void platform_audio_sync(AudioSystem *as)
{
    WindowsAudioData *wad = (WindowsAudioData *)as->platform_data;

    WAVEHDR *header = &wad->waveHeaders[wad->currentBuffer];
    if (header->dwFlags & WHDR_INQUEUE)
    {
        WaitForSingleObject(wad->bufferEvent, INFINITE);
    }
}

void platform_audio_free(AudioSystem *as)
{
    WindowsAudioData *wad = (WindowsAudioData *)as->platform_data;
    if (!wad)
        return;

    waveOutReset(wad->hWaveOut);

    for (size_t i = 0; i < NUM_BUFFERS; i++)
    {
        if (wad->waveHeaders[i].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(wad->hWaveOut, &wad->waveHeaders[i], sizeof(WAVEHDR));
        free(wad->buffers[i]);
    }

    waveOutClose(wad->hWaveOut);
    CloseHandle(wad->bufferEvent);
    free(wad);
    ENGINE_LOG("[AUDIO] Unloaded WinMM backend!\n");
}
