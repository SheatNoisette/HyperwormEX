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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "render.h"
#include "const.h"

void platform_init(Render *rd);
bool platform_update(Render *rd);
void platform_render_begin(Render *rd);
void platform_render_end(Render *rd);
void platform_free(Render *rd);

bool platform_key(Render *rd, int key);
bool platform_key_pressed(Render *rd, int key);

int platform_audio_init(AudioSystem *as);
int platform_audio_writable(AudioSystem *as);
void platform_audio_write(AudioSystem *as, float *buf, size_t n);
void platform_audio_sync(AudioSystem *as);
void platform_audio_free(AudioSystem *as);

void *platform_mutex_create();
void platform_mutex_lock(void *mutex);
void platform_mutex_unlock(void *mutex);

void *platform_thread_create(void (*fnc_thread)(void *), void *data);
void platform_thread_join(void *thread);

#endif
