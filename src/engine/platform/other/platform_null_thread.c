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

#include <stdlib.h>

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
