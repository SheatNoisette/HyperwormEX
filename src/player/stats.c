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

#include "player.h"

void player_load_stats(Actor *self, PlayerStats *stats) {
    PlayerData *d = self->data;
    d->stats = stats;
    d->speed = stats->move_speed;
}

void player_reset_stats(PlayerStats *stats) {
    stats->max_hp = 4;
    stats->move_speed = 7.5f;
    stats->shoot_speed = 0.15f;
    stats->grenade_speed = 0.5f;
    stats->shoot_recoil = 0.15f;
    stats->shoot_recoil_recovery = 2.0f;
    stats->block_destroy_chance = 2;

    stats->hp = stats->max_hp;
    stats->money = 0;
    stats->grenades = 10;

    stats->won = false;

#if FEATURE_STATS
    stats->money_collected = 0;
    stats->enemies_killed = 0;
    stats->damage_taken = 0;
    stats->grenades_fired = 0;
#endif
}
