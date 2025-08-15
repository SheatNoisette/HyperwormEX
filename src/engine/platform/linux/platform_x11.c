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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "engine/platform.h"
#include "engine/const.h"
#include "engine/logging.h"
#include "engine/render.h"

// -----------------------------------------------------------------------------
// Defs


// -----------------------------------------------------------------------------
// Types

typedef struct XElevenPlatform {
    Window root;
    XVisualInfo *vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    XEvent xev;
    Display *dpy;
    Window win;
    GLXContext glc;
    GLuint textureID;
    Cursor invisible_cursor;
    struct timespec last_spec;
    uint8_t keys[65535];
} XElevenPlatform;


// -----------------------------------------------------------------------------
// Render

Cursor create_invisible_cursor(Display *display, Window root) {
    Pixmap bitmap_no_data;
    XColor black;
    black.red = 0;
    black.green = 0;
    black.blue = 0;
    static char no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};

    bitmap_no_data = XCreateBitmapFromData(display, root, no_data, 8, 8);
    return XCreatePixmapCursor(display, bitmap_no_data, bitmap_no_data,
                               &black, &black, 0, 0);
}

void platform_init(Render *rd) {
    rd->platform_render_data = malloc(sizeof(XElevenPlatform));
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;

    // Sample time for DT, first time
    clock_gettime(CLOCK_MONOTONIC, &prd->last_spec);

    // Init window
    prd->dpy = XOpenDisplay(NULL);
#if SAFE_C
    if (rd->dpy == NULL) {
        fprintf(stderr, "Error: Cannot connect to X server\n");
    }
#endif
    prd->root = DefaultRootWindow(prd->dpy);

    // Create invisible cursor for mouse lock
    prd->invisible_cursor = create_invisible_cursor(prd->dpy, prd->root);

    prd->vi = glXChooseVisual(prd->dpy, 0, (GLint[]){ GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None });
#if SAFE_C
    if (prd->vi == NULL) {
        fprintf(stderr, "Error: No appropriate visual found\n");
        XCloseDisplay(prd->dpy);
    }
#endif

    prd->cmap = XCreateColormap(prd->dpy, prd->root, prd->vi->visual, AllocNone);
    prd->swa.colormap = prd->cmap;
    prd->swa.event_mask =  ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask;
    // Ensure window manager handles the window
    prd->swa.override_redirect = False;

    // Create window
    prd->win = XCreateWindow(prd->dpy, prd->root, 0, 0, rd->window_width, rd->window_height, 0, prd->vi->depth, InputOutput,
                        prd->vi->visual, CWColormap | CWEventMask, &prd->swa);
    XMapWindow(prd->dpy, prd->win);
    XStoreName(prd->dpy, prd->win, WINDOW_NAME);

    prd->glc = glXCreateContext(prd->dpy, prd->vi, NULL, GL_TRUE);
#if SAFE_C
    if (prd->glc == NULL) {
        fprintf(stderr, "Error: Cannot create GL context\n");
        XDestroyWindow(prd->dpy, prd->win);
        XFreeColormap(prd->dpy, cmap);
        XFree(vi);
        XCloseDisplay(prd->dpy);
    }
#endif

    // Make context current
    glXMakeCurrent(prd->dpy, prd->win, prd->glc);

    // Enable 2D texturing for fb
    glEnable(GL_TEXTURE_2D);

    // Generate texture name for screen texture
    glGenTextures(1, &prd->textureID);
    glBindTexture(GL_TEXTURE_2D, prd->textureID);

#if NINTENDO_64
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

    // Bind the texture once at init, we aren't using any others anyway
    glBindTexture(GL_TEXTURE_2D, prd->textureID);
}

bool platform_update(Render *rd) {
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;

    // Calculate delta time
    struct timespec current_spec;
    clock_gettime(CLOCK_MONOTONIC, &current_spec);

    // Calculate elapsed time in seconds
    rd->dt = (current_spec.tv_sec - prd->last_spec.tv_sec) +
             (current_spec.tv_nsec - prd->last_spec.tv_nsec) / 1e9;

    prd->last_spec = current_spec;

    // Cap delta time to prevent spiral of death and break everything (if slow)
    // like lights etc.
    if (rd->dt > 0.25) rd->dt = 0.25;

    // Update key event loop
    XFlush(prd->dpy);
    while (XPending(prd->dpy)) {
        XNextEvent(prd->dpy, &prd->xev);

        switch (prd->xev.type) {
            case ConfigureNotify: {
                // Window resized
                XConfigureEvent xce = prd->xev.xconfigure;
                if (xce.width != rd->window_width || xce.height != rd->window_height) {
                    rd->window_width = xce.width;
                    rd->window_height = xce.height;
                    glViewport(0, 0, rd->window_width, rd->window_height);
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    platform_render_end(rd);
                    ENGINE_LOG("Window resized to: %li x %li\n", rd->window_width, rd->window_height);
                }
            }
            break;
            case KeyPress:
            {
                KeySym keysym = XLookupKeysym(&prd->xev.xkey, 0);

                // Register key as pressed
                if (keysym < sizeof(prd->keys)) {
                    prd->keys[keysym] = 1;
                }

                if (keysym == XK_Escape) {
                    if (rd->mouse_locked) {
                        XUngrabPointer(prd->dpy, CurrentTime);
                        XUndefineCursor(prd->dpy, prd->win);
                        rd->mouse_locked = false;
                        ENGINE_LOG("Mouse unlocked.\n");

                        rd->mouse_delta_x = 0;
                        rd->mouse_delta_y = 0;
                    }
                }
            }
            break;
            case KeyRelease:
            {
                KeySym keysym = XLookupKeysym(&prd->xev.xkey, 0);
                // printf("Key released: %s (keysym=%ld)\n", XKeysymToString(keysym), keysym);
                // Register key as released
                if (keysym < sizeof(prd->keys)) {
                    prd->keys[keysym] = 0;
                }
            }
            break;
            case ButtonPress: {
                // Update mouse click
                if (prd->xev.xbutton.button == Button1)
                    rd->mouse_click |= MOUSE_LEFT_CLICK;

                if (prd->xev.xbutton.button == Button3)
                    rd->mouse_click |= MOUSE_RIGHT_CLICK;

                // Lock on left click && swap to invisible cursor
                if (!rd->mouse_locked && prd->xev.xbutton.button == Button1) {
                    int grab_status = XGrabPointer(prd->dpy, prd->win, True,
                                PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                                GrabModeAsync, GrabModeAsync,
                                prd->win, prd->invisible_cursor, CurrentTime);

                    if (grab_status == GrabSuccess) {
                        rd->mouse_locked = true;
                        rd->mouse_click = 0;
                        ENGINE_LOG("Mouse locked!\n");
                    } else {
#if LOGGING
                        fprintf(stderr, "Failed to grab pointer, status: %d\n", grab_status);
#endif
                    }
                }
            }
            break;
            case ButtonRelease: {
                // Update mouse click
                if (prd->xev.xbutton.button == Button1)
                    rd->mouse_click &= ~MOUSE_LEFT_CLICK;

                if (prd->xev.xbutton.button == Button3)
                    rd->mouse_click &= ~MOUSE_RIGHT_CLICK;
            }
            break;
            case MotionNotify: {
                if (rd->mouse_locked) {
                    size_t center_x = rd->window_width / 2;
                    size_t center_y = rd->window_height / 2;

                    // Check if the event position is significantly different from the center when
                    // moving the camera like a maniac
                    if (prd->xev.xmotion.x != center_x || prd->xev.xmotion.y != center_y) {
                        // Set it to center an warp the pointer back
                        rd->mouse_delta_x = prd->xev.xmotion.x - center_x;
                        rd->mouse_delta_y = prd->xev.xmotion.y - center_y;
                        XWarpPointer(prd->dpy, None, prd->win, 0, 0, 0, 0, center_x, center_y);
                    } else {
                        // Event is the one generated by XWarpPointer, ignore delta to avoid
                        // weird accel
                        rd->mouse_delta_x = 0;
                        rd->mouse_delta_y = 0;
                    }
                }
            }
            break;
        }

    }

    return true;
}

void platform_render_begin(Render *rd) {
    // Force clear just in case
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void platform_render_end(Render *rd) {
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;

    // Set up orthographic projection matching window pixels
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, rd->window_width, rd->window_height, 0, -1, 1);

    // Get aspect ratio
    float buffer_aspect = (float)SOFTWARE_WIDTH / (float)SOFTWARE_HEIGHT;
    float window_aspect = (float)rd->window_width / (float)rd->window_height;

    float quad_width, quad_height;
    float offset_x = 0.0f, offset_y = 0.0f;

    if (window_aspect > buffer_aspect) {
        // Window is wider than the buffer aspect ratio -> Pillarbox
        quad_height = (float)rd->window_height;
        quad_width = quad_height * buffer_aspect;
        offset_x = (rd->window_width - quad_width) / 2.0f;
    } else {
        // Window is narrower or same aspect ratio -> Letterbox or exact fit
        quad_width = (float)rd->window_width;
        quad_height = quad_width / buffer_aspect;
        offset_y = (rd->window_height - quad_height) / 2.0f;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SOFTWARE_WIDTH, SOFTWARE_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rd->frame_buffer);

    glBegin(GL_QUADS);
    {
        // Top left
        glTexCoord2f(0.0f, 0.0f); glVertex2f(offset_x, offset_y);
        // Top right
        glTexCoord2f(1.0f, 0.0f); glVertex2f(offset_x + quad_width, offset_y);
        // Bottom right
        glTexCoord2f(1.0f, 1.0f); glVertex2f(offset_x + quad_width, offset_y + quad_height);
        // Bottom left
        glTexCoord2f(0.0f, 1.0f); glVertex2f(offset_x, offset_y + quad_height);
    }
    glEnd();

    glXSwapBuffers(prd->dpy, prd->win);
}

void platform_free(Render *rd) {
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;

    glXMakeCurrent(prd->dpy, None, NULL);
    glXDestroyContext(prd->dpy, prd->glc);
    if (prd->textureID) {
        glDeleteTextures(1, &prd->textureID);
    }
    if (prd->invisible_cursor != None) {
        XFreeCursor(prd->dpy, prd->invisible_cursor);
    }
#if SAFE_C
    XDestroyWindow(prd->dpy, prd->win);
    XFreeColormap(prd->dpy, prd->cmap);
    XFree(prd->vi);
    XCloseDisplay(prd->dpy);
#endif
}

bool platform_key(Render *rd, int key)
{
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;
    // This is a small hack, the keys are mapped to X11 keystrokes
    return prd->keys[key &  0xffff];
}

bool platform_key_pressed(Render *rd, int key) {
    XElevenPlatform *prd = (XElevenPlatform *)rd->platform_render_data;
    bool p = prd->keys[key &  0xffff];
    prd->keys[key &  0xffff] = 0;
    return p;
}
