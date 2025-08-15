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

#include "engine/logging.h"

/*
    This is a NULL driver, nothing is done. Good base for a port.
*/

void platform_init(Render *rd)
{
    ENGINE_LOG("[BACKEND] Null backend selected, well, you're on your own!\n");
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
