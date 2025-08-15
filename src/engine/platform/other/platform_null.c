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

#include "engine/render.h"
#include "engine/audio.h"
#include "engine/logging.h"

/*
    This is a NULL driver, nothing is done. Good base for a port.
*/

void platform_init(Render *rd)
{
    LOG("[BACKEND] Null backend selected, well, you're on your own!\n");
    (void)rd;
}

bool platform_update(Render *rd)
{
    (void)rd;
    return true;
}

void platform_render_begin(Render *rd)
{
    (void)rd;
}

void platform_render_end(Render *rd)
{
    (void)rd;
}

void platform_free(Render *rd)
{
    (void)rd;
}

int platform_audio_init(AudioSystem *as)
{
    LOG("[BACKEND] Audio disabled, no driver!\n");
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

void *platform_mutex_create()
{
    return NULL;
}

void platform_mutex_lock(void *mutex)
{
    (void)mutex;
}

void platform_mutex_unlock(void *mutex)
{
    (void)mutex;
}

void *platform_thread_create(void (*fnc_thread)(void *), void *data)
{
    return NULL;
}

void platform_thread_join(void *thread)
{
    (void)thread;
}

bool platform_key(Render *rd, int key)
{
    (void)rd;
    (void)key;
    return false;
}

bool platform_key_pressed(Render *rd, int key)
{
    (void)rd;
    (void)key;
    return false;
}
