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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "engine/const.h"
#include "engine/font.h"

void text_draw (
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    const char *text,
    size_t position_x,
    size_t position_y,
    uint32_t color
) {
    // LOWERTEXTONLY Bra u mirin ?
    size_t x = position_x;
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        size_t index;

        // Ordering:
        // " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-/|<>"
        if (c == ' ') {
            index = 0;
        } else if (c >= '0' && c <= '9') {
            index = (c - '0') + 27;
        } else switch (c) {
            case '.':
                index = 37;
                break;
            case '-':
                index = 38;
                break;
            case '/':
                index = 39;
                break;
            case '|':
                index = 40;
                break;
            case '<':
                index = 41;
                break;
            case '>':
                index = 42;
                break;
            default:
                // Letters A-Z
                // This is technically unsafe, an unknown character might SEGV
                // But we won't give it any unknown characters now will we? :)
                index = c & 0x1F;
                break;
        }

        for (size_t row = 0; row < LETTER_HEIGHT; row++) {
            uint8_t bits = system_font[index][row];
            for (size_t col = 0; col < LETTER_WIDTH; col++) {
                if (bits & (1 << (LETTER_WIDTH - 1 - col))) {
                    size_t px = x + col;
                    size_t py = position_y + row;
                    if (px < buffer_width && py < buffer_height) {
                        buffer[py * buffer_width + px] = 0xff000000 | color;
                    }
                }
            }
        }

        // advance cursor (spacing between letters)
        x += LETTER_WIDTH + LETTER_SPACING;
    }
}

void text_draw_centered (
    uint32_t *buffer,
    size_t buffer_width,
    size_t buffer_height,
    const char *text,
    size_t position_y,
    uint32_t color
) {
    size_t len = TEXT_WIDTH(strlen(text));
    size_t x = SOFTWARE_WIDTH/2 - len/2;
    text_draw(buffer, buffer_width, buffer_height, text, x, position_y, color);
}
