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

#include "engine/sprite.h"
#include "sprites/sprites.h"
#include "sprite_atlas.h"

// Add sprites here and update SpriteAtlasDefinition {
static SpriteAtlasEntry sprite_entries[SPR_NUMBER] = {
    // def, data, palette, width, height[, center X, center Y]
    {SPR_GUN, spr_gun_data, spr_gun_palette, spr_gun_size, spr_gun_size, spr_gun_size*2, spr_gun_size*2},
    {SPR_WORM, spr_worm_data, spr_worm_palette, spr_worm_width, spr_worm_height},
    {SPR_WORM_2, spr_worm_data, spr_worm_palette_2, spr_worm_width, spr_worm_height},
    {SPR_WORMHOLE, spr_wh_data, spr_wh_palette, spr_wh_size, spr_wh_size},
};
// }

// Complete pre-computed atlas
static Sprite *sprites[SPR_NUMBER];

void spr_build();
Sprite *spr_get_sprite(SpriteAtlasDefinition def);

// Build the sprite atlas
void spr_build() {
    for (size_t i = 0; i < (size_t)SPR_NUMBER; i++) {
        SpriteAtlasEntry e = sprite_entries[i];
        Sprite *spr = sprite_new(e.width, e.height, e.cx, e.cy);
        sprite_unpack_2bit(spr, e.data, e.palette, e.width, e.height);
        sprites[e.def] = spr;
    }
}

// Get a sprite
Sprite *spr_get(SpriteAtlasDefinition def) {
    return sprites[def];
}
