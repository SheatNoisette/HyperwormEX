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

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "engine/render.h"
#include "engine/audio.h"
#include "engine/logging.h"
#include "engine/keys.h"

#include "raylib.h"

typedef struct RaylibBackendData
{
    // Rendering
    RenderTexture2D fbRenderTexture;
    Texture2D fbTexture;
    Color *fbPixels;

    // Keys Engine -> Raylib
    int keys[65535];
    bool mouselocked;
} RaylibBackendData;

void platform_init(Render *rd)
{
    ENGINE_LOG("[BACKEND] Starting Raylib Backend...\n");

    rd->platform_render_data = malloc(sizeof(RaylibBackendData));
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME " - (Raylib)");
    SetTargetFPS(9999);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    rd->window_width = WINDOW_WIDTH;
    rd->window_height = WINDOW_HEIGHT;
    rbd->mouselocked = false;
    rd->dt = GetFrameTime();

    // Create a custom render texture (framebuffer)
    rbd->fbRenderTexture = LoadRenderTexture(SOFTWARE_WIDTH, SOFTWARE_HEIGHT);
    rbd->fbTexture = rbd->fbRenderTexture.texture;
    rbd->fbPixels = (Color *)malloc(SOFTWARE_WIDTH * SOFTWARE_HEIGHT * sizeof(Color));
    SetTextureFilter(rbd->fbTexture, TEXTURE_FILTER_POINT);

    // @TODO: Handle ALL KEYS IN AN ARRAY!
    memset(rbd->keys, 0, sizeof(rbd->keys) * sizeof(uint8_t));
    rbd->keys[EKEY_SPACE] = KEY_SPACE;
    rbd->keys[EKEY_BACKSPACE] = KEY_BACKSPACE;
    rbd->keys[EKEY_ENTER] = KEY_ENTER;
    rbd->keys[EKEY_LEFT] = KEY_LEFT;
    rbd->keys[EKEY_UP] = KEY_UP;
    rbd->keys[EKEY_DOWN] = KEY_DOWN;
    rbd->keys[EKEY_A] = KEY_A;
    rbd->keys[EKEY_D] = KEY_D;
    rbd->keys[EKEY_L] = KEY_L;
    rbd->keys[EKEY_O] = KEY_O;
    rbd->keys[EKEY_Q] = KEY_Q;
    rbd->keys[EKEY_S] = KEY_S;
    rbd->keys[EKEY_W] = KEY_W;
    rbd->keys[EKEY_Z] = KEY_Z;
}

bool platform_update(Render *rd)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;

    bool sc = !WindowShouldClose();
    rd->dt = GetFrameTime();

    if (rd->dt > 0.25) {
        rd->dt = 0.25;
    }

    // Check mouse buttons for shooting
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        rd->mouse_click |= MOUSE_LEFT_CLICK;
    else
        rd->mouse_click &= ~MOUSE_LEFT_CLICK;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        rd->mouse_click |= MOUSE_RIGHT_CLICK;
    else
        rd->mouse_click &= ~MOUSE_RIGHT_CLICK;

    // Toggle mouse lock for FPS view
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !rbd->mouselocked) {
        rbd->mouselocked = true;
        HideCursor();
        DisableCursor();
    }

    if (IsKeyPressed(KEY_ESCAPE) && rbd->mouselocked) {
        rbd->mouselocked = false;
        ShowCursor();
        EnableCursor();
        rd->mouse_delta_x = 0;
        rd->mouse_delta_y = 0;
    }

    if (rbd->mouselocked) {
        Vector2 mousePosition = GetMousePosition();
        size_t screenCenterX = GetScreenWidth() / 2;
        size_t screenCenterY = GetScreenHeight() / 2;

        rd->mouse_delta_x = mousePosition.x - screenCenterX;
        rd->mouse_delta_y = mousePosition.y - screenCenterY;

        // Wrap mouse to center
        SetMousePosition((int)screenCenterX, (int)screenCenterY);
    } else {
        rd->mouse_delta_x = 0.0f;
        rd->mouse_delta_y = 0.0f;
    }

    return sc;
}

void platform_render_begin(Render *rd)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;

    // Copy the texture
    for (size_t x = 0; x < SOFTWARE_WIDTH; x++)
    {
        for (size_t y = 0; y < SOFTWARE_HEIGHT; y++)
        {
            size_t pos = x + y * SOFTWARE_WIDTH;
            rbd->fbPixels[pos].r = rd->frame_buffer[pos] & 0xff;
            rbd->fbPixels[pos].g = (rd->frame_buffer[pos] >> 8) & 0xff;
            rbd->fbPixels[pos].b = (rd->frame_buffer[pos] >> 16) & 0xff;
            rbd->fbPixels[pos].a = 255;
        }
    }

    rd->window_width = GetScreenWidth();
    rd->window_height = GetScreenHeight();
}

void platform_render_end(Render *rd)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;
    UpdateTexture(rbd->fbTexture, rbd->fbPixels);
    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle sourceRec = {0, 0, (float)rbd->fbTexture.width, (float)rbd->fbTexture.height};
    Rectangle destRec = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    Vector2 origin = {0, 0};
    DrawTexturePro(rbd->fbTexture, sourceRec, destRec, origin, 0.0f, WHITE);
    EndDrawing();
}

void platform_free(Render *rd)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;
    UnloadRenderTexture(rbd->fbRenderTexture);
    free(rbd->fbPixels);
    free(rd->platform_render_data);
    CloseWindow();
}

bool platform_key(Render *rd, int key)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;
    return IsKeyDown(rbd->keys[key & 0xffff]);
}

bool platform_key_pressed(Render *rd, int key)
{
    RaylibBackendData *rbd = (RaylibBackendData *)rd->platform_render_data;
    return IsKeyReleased(rbd->keys[key & 0xffff]);
}
