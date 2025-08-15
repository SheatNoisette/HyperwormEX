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
#include "engine/logging.h"

int platform_audio_init(AudioSystem *as)
{
    ENGINE_LOG("[BACKEND] Audio disabled, no driver!\n");
    (void)as;
    return -1;
}

int platform_audio_writable(AudioSystem *as)
{
    (void)as;
    return 0;
}

void platform_audio_write(AudioSystem *as, float *buf, size_t n)
{
    (void)as;
    (void)buf;
    (void)n;
}

void platform_audio_sync(AudioSystem *as)
{
    (void)as;
}

void platform_audio_free(AudioSystem *as)
{
    (void)as;
}
