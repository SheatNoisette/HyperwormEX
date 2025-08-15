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

#include "upgrades.h"
#include "player.h"

static void upgrade_speed(Upgrade *self, PlayerStats *stats) {
    if (stats->move_speed < 12.0f) {
        stats->move_speed += 1.2f;
    }

    // Scale upgrade cost
    self->extra_cost += 20;
}

static void upgrade_hp(Upgrade *self, PlayerStats *stats) {
    stats->max_hp += 2;
    stats->hp = stats->max_hp;

    // Scale upgrade cost
    self->extra_cost += 20;
}

static void upgrade_recoil(Upgrade *self, PlayerStats *stats) {
    stats->shoot_recoil *= 0.8f;
    stats->shoot_recoil_recovery *= 1.3f;

    // Scale upgrade cost
    self->extra_cost += 20;
}

static void upgrade_shooting_speed(Upgrade *self, PlayerStats *stats) {
    stats->shoot_speed *= 0.85f;
    // Change block destroy chance? nahhhh it's too fun to BREAK EVERYTHING

    // Scale upgrade cost
    self->extra_cost += 20;
}

static void upgrade_grenades(Upgrade *self, PlayerStats *stats) {
    stats->grenades += 6;
}

static void upgrade_end(Upgrade *self, PlayerStats *stats) {
    stats->won = true;
}

static Upgrade upgrades[NUM_UPGRADES] = {
    (Upgrade){
        .name = "INCREASE MOVEMENT SPEED",
        .apply = upgrade_speed,
        .cost = 300
    },
    (Upgrade){
        .name = "INCREASE HEALTH",
        .apply = upgrade_hp,
        .cost = 300
    },
    (Upgrade){
        .name = "INCREASE ACCURACY",
        .apply = upgrade_recoil,
        .cost = 200
    },
    (Upgrade){
        .name = "INCREASE FIRING SPEED",
        .apply = upgrade_shooting_speed,
        .cost = 300
    },
    (Upgrade){
        .name = "BUY 6 GRENADES",
        .apply = upgrade_grenades,
        .cost = 100
    },
    (Upgrade){
        .name = "FINISH GAME - PAY YOUR DEBTS",
        .apply = upgrade_end,
        .cost = 3000
    },
};

Upgrade *get_upgrade(size_t i) {
    return &upgrades[i];
}

void reset_upgrades() {
    for (size_t i = 0; i < NUM_UPGRADES; i++) {
        upgrades[i].extra_cost = 0;
    }
}
