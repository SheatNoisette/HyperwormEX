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

#ifndef __UPGRADES_H__
#define __UPGRADES_H__

#include "game.h"
#include "player.h"

#define NUM_UPGRADES 6

typedef struct Upgrade {
    char *name;
    int cost;
    int extra_cost;
    void (*apply)(struct Upgrade *, PlayerStats *);
} Upgrade;

Upgrade *get_upgrade(size_t i);
void reset_upgrades();

#endif
