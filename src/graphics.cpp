#include "graphics.h"
#include "vec2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <math.h>
#include <vector>

SDL_Window *Graphics::window = NULL;
SDL_Renderer *Graphics::renderer = NULL;
int Graphics::window_width = 0;
int Graphics::window_height = 0;

int Graphics::width() { return window_width; }

int Graphics::height() { return window_height; }

bool Graphics::open_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL!" << std::endl;
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;
    window = SDL_CreateWindow(NULL, 0, 0, window_width, window_height,
                              SDL_WINDOW_BORDERLESS);
    if (!window) {
        std::cerr << "Error creating SDL window!" << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error creating SDL renderer!" << std::endl;
        return false;
    }
    return true;
}

void Graphics::clear_screen(Uint32 color) {
    SDL_SetRenderDrawColor(renderer, color >> 16, color >> 8, color, 255);
    SDL_RenderClear(renderer);
}

void Graphics::render_frame() { SDL_RenderPresent(renderer); }

void Graphics::draw_line(int x0, int y0, int x1, int y1, Uint32 color) {
    // from SDL2_gfxPrimitives.h
    lineColor(renderer, x0, y0, x1, y1, color);
}

void Graphics::draw_circle(int x, int y, int radius, float angle,
                           Uint32 color) {
    circleColor(renderer, x, y, radius, color);
    lineColor(renderer, x, y, x + cos(angle) * radius, y + sin(angle) * radius,
              color);
}

void Graphics::draw_fill_circle(int x, int y, int radius, Uint32 color) {
    filledCircleColor(renderer, x, y, radius, color);
}

void Graphics::draw_rect(int x, int y, int width, int height, Uint32 color) {
    lineColor(renderer, x - width / 2.0, y - height / 2.0, x + width / 2.0,
              y - height / 2.0, color);
    lineColor(renderer, x + width / 2.0, y - height / 2.0, x + width / 2.0,
              y + height / 2.0, color);
    lineColor(renderer, x + width / 2.0, y + height / 2.0, x - width / 2.0,
              y + height / 2.0, color);
    lineColor(renderer, x - width / 2.0, y + height / 2.0, x - width / 2.0,
              y - height / 2.0, color);
}

void Graphics::draw_fill_rect(int x, int y, int width, int height,
                              Uint32 color) {
    boxColor(renderer, x - width / 2.0, y - height / 2.0, x + width / 2.0,
             y + height / 2.0, color);
}

void Graphics::draw_polygon(int x, int y, const std::vector<Vec2> &vertices,
                            Uint32 color) {
    for (size_t i = 0; i < vertices.size(); i++) {
        int curr_index = i;
        int next_index = (i + 1) % vertices.size();
        lineColor(renderer, vertices[curr_index].x, vertices[curr_index].y,
                  vertices[next_index].x, vertices[next_index].y, color);
    }

    filledCircleColor(renderer, x, y, 1, color);
}

void Graphics::draw_fill_polygon(int x, int y,
                                 const std::vector<Vec2> &vertices,
                                 Uint32 color) {
    std::vector<short> vx;
    std::vector<short> vy;

    for (size_t i = 0; i < vertices.size(); i++) {
        vx.push_back(static_cast<int>(vertices[i].x));
    }
    for (size_t i = 0; i < vertices.size(); i++) {
        vy.push_back(static_cast<int>(vertices[i].y));
    }

    filledPolygonColor(renderer, &vx[0], &vy[0], vertices.size(), color);
    filledCircleColor(renderer, x, y, 1, 0xFF000000);
}

void Graphics::draw_texture(int x, int y, int width, int height, float rotation,
                            SDL_Texture *texture) {
    SDL_Rect dst_rect = {x - (width / 2), y - (height / 2), width, height};
    float rotation_deg = rotation * 57.2958;
    SDL_RenderCopyEx(renderer, texture, NULL, &dst_rect, rotation_deg, NULL,
                     SDL_FLIP_NONE);
}

void Graphics::close_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
