#include "app.h"
#include "graphics.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

bool App::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void App::setup() {
    running = Graphics::open_window();

    // TODO: setup objects in the scene
}

/**
 * Input processing
 */
void App::input() {
    SDL_Event event;

    // poll events
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            break;
        }
    }
}

/**
 * Update function (called several times per second to update objects)
 */
void App::update() {
    // TODO: update all objects in the scene
}

/**
 * Render function (called several times per second to draw objects)
 */
void App::render() {
    // `FF` - full opacity, no transparency
    Graphics::clear_screen(0xFF056263);
    Graphics::draw_fill_circle(200, 200, 40, 0xFFFFFFFF); // all white circle
    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void App::destroy() {
    // TODO: destroy all objects in the scene

    Graphics::close_window();
}
