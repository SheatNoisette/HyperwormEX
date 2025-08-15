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

#ifndef __SPRITE_ATLAS_H__
#define __SPRITE_ATLAS_H__

#include "engine/const.h"
#include "engine/sprite.h"
#include "flags.h"
#include "pl_synth.h"

typedef enum SpriteAtlasDefinition {
    SPR_GUN = 0,
    SPR_WORM,
    SPR_WORM_2,
    SPR_WORMHOLE,
    /* Internal number, DO NOT TOUCH */
    SPR_NUMBER,
} SpriteAtlasDefinition;

// Sprite entry
typedef struct SpriteAtlasEntry {
    SpriteAtlasDefinition def;
    uint8_t *data;
    uint32_t *palette;
    size_t width;
    size_t height;
    size_t cx; // center X
    size_t cy; // center Y
} SpriteAtlasEntry;

void spr_build();
Sprite *spr_get(SpriteAtlasDefinition def);

#endif
