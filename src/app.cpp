#include "app.h"
#include "constants.h"
#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>

bool App::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void App::setup() {
    running = Graphics::open_window();

    particle = new Particle(50, 100, 1.0);
    particle->radius = 4;
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
    // Check if we are too fast; if so, wait for some milliseconds,
    // until we reach the MS_PER_FRAME
    static int time_previous_frame;
    // SDL_GetTicks(): current total number of milliseconds that our SDL app has
    // been running
    int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - time_previous_frame);
    // we want to make sure MS_PER_FRAME has passed
    if (time_to_wait > 0) {
        SDL_Delay(time_to_wait);
    }

    // delta time: difference between the current frame and the last frame (in
    // seconds)
    // we want to move _per second_, not _per frame_
    // **frame rate independent movement**
    //  (current time in ms of this frame - time in ms of previous frame)
    float delta_time = (SDL_GetTicks() - time_previous_frame) / 1000.0f;
    // protect delta_time from consuming large values that could potentially
    // mess up the execution
    // e.g. when debugging
    if (delta_time > 0.016) {
        delta_time = 0.016;
    }

    time_previous_frame = SDL_GetTicks();

    // how many pixels to move _per second_

    // this is only constant velocity
    // particle->velocity = Vec2(100.0 * delta_time, 30.0 * delta_time);

    // particle->acceleartion = Vec2(0.0, 9.8 * PIXELS_PER_METER);
    particle->acceleartion.x = 2.0 * PIXELS_PER_METER;
    particle->acceleartion.y = 9.8 * PIXELS_PER_METER;

    // Integrate the acceleration and velocity to estimate the new position
    particle->integrate(delta_time);

    if (particle->position.x - particle->radius <= 0) {
        // left border reached
        particle->position.x = particle->radius;
        particle->velocity.x *= -0.9;
    } else if (particle->position.x + particle->radius >= Graphics::width()) {
        // reached right border
        particle->position.x = Graphics::width() - particle->radius;
        particle->velocity.x *= -0.9;
    }
    if (particle->position.y - particle->radius <= 0) {
        // top border reached
        particle->position.y = particle->radius;
        particle->velocity.y *= -0.9;
    } else if (particle->position.y + particle->radius >= Graphics::height()) {
        // reached bottom border
        particle->position.y = Graphics::height() - particle->radius;
        particle->velocity.y *= -0.9;
    }
}

/**
 * Render function (called several times per second to draw objects)
 */
void App::render() {
    // `FF` - full opacity, no transparency
    Graphics::clear_screen(0xFF056263);
    Graphics::draw_fill_circle(particle->position.x, particle->position.y, 4,
                               0xFFFFFFFF); // all white circle
    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void App::destroy() {
    delete particle;
    Graphics::close_window();
}
