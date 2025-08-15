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

#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "engine/logging.h"
#include "engine/const.h"

void *platform_mutex_create() {
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    if (mutex == NULL) {
        ENGINE_LOG("[PLATFORM] Failed to allocate mutex\n");
        return NULL;
    }
    if (pthread_mutex_init(mutex, NULL) != 0) {
        ENGINE_LOG("[PLATFORM] Failed to initialize mutex\n");
        free(mutex);
        return NULL;
    }
    return mutex;
}

void platform_mutex_lock(void *mutex) {
    pthread_mutex_lock((pthread_mutex_t *)mutex);
}

void platform_mutex_unlock(void *mutex) {
    pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

void *platform_thread_create(void (*fnc_thread)(void *), void *data) {
    pthread_t *thread_id = malloc(sizeof(pthread_t));

    if (thread_id == NULL) {
        ENGINE_LOG("[PLATFORM] Failed to allocate thread ID\n");
        return NULL;
    }

    // mfw (void *(*)(void *))
    if (pthread_create(thread_id, NULL, (void *(*)(void *))fnc_thread, data) != 0) {
        ENGINE_LOG("[PLATFORM] Failed to create thread\n");
        free(thread_id);
        return NULL;
    }

    return thread_id;
}

void platform_thread_join(void *thread) {
    pthread_join(*(pthread_t *)thread, NULL);
    free(thread);
}
