#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vec2.h"
#include <SDL2/SDL.h>
// #include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <vector>

struct Graphics {
    static int window_width;
    static int window_height;
    static SDL_Window *window;
    static SDL_Renderer *renderer;

    static int width();
    static int height();
    static bool open_window();
    static void close_window();
    static void clear_screen(Uint32 color);
    static void render_frame();
    static void draw_line(int x0, int y0, int x1, int y1, Uint32 color);
    static void draw_circle(int x, int y, int radius, float angle,
                            Uint32 color);
    static void draw_fill_circle(int x, int y, int radius, Uint32 color);
    static void draw_rect(int x, int y, int width, int height, Uint32 color);
    static void draw_fill_rect(int x, int y, int width, int height,
                               Uint32 color);
    static void draw_polygon(int x, int y, const std::vector<Vec2> &vertices,
                             Uint32 color);
    static void draw_fill_polygon(int x, int y,
                                  const std::vector<Vec2> &vertices,
                                  Uint32 color);
    static void draw_texture(int x, int y, int width, int height,
                             float rotation, SDL_Texture *texture);
};

#endif
