#include "app.h"
#include "constants.h"
#include "force.h"
#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_timer.h>

bool App::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void App::setup() {
    running = Graphics::open_window();

    Particle *small_planet = new Particle(200, 200, 1.0);
    small_planet->radius = 6;
    particles.push_back(small_planet);

    Particle *big_planet = new Particle(500, 500, 20.0);
    big_planet->radius = 20;
    particles.push_back(big_planet);

    liquid.x = 0;
    liquid.y = Graphics::height() / 2;
    liquid.w = Graphics::width();
    liquid.h = Graphics::height() / 2;
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
            if (event.key.keysym.sym == SDLK_UP) {
                push_force.y = -10 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                push_force.x = 10 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                push_force.y = 10 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                push_force.x = -10 * PIXELS_PER_METER;
            }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_UP ||
                event.key.keysym.sym == SDLK_DOWN) {
                push_force.y = 0;
            }
            if (event.key.keysym.sym == SDLK_RIGHT ||
                event.key.keysym.sym == SDLK_LEFT) {
                push_force.x = 0;
            }
            break;
        case SDL_MOUSEMOTION:
            mouse_cursor.x = event.motion.x;
            mouse_cursor.y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            /*
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Particle *particle = new Particle(x, y, 1.0);
                particle->radius = 5;
                particles.push_back(particle);
            }
            */
            if (!left_mouse_button_down &&
                event.button.button == SDL_BUTTON_LEFT) {
                left_mouse_button_down = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouse_cursor.x = x;
                mouse_cursor.y = y;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (left_mouse_button_down &&
                event.button.button == SDL_BUTTON_LEFT) {
                left_mouse_button_down = false;
                Vec2 impluse_direction =
                    (particles[0]->position - mouse_cursor).unit_vector();
                float impulse_mag =
                    (particles[0]->position - mouse_cursor).mag() * 5.0;
                particles[0]->velocity = impluse_direction * impulse_mag;
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

    Vec2 g_force =
        Force::generate_g_force(*particles[0], *particles[1], 1000.0, 5, 100);
    // apply g force
    particles[0]->apply_force(g_force);
    particles[1]->apply_force(-g_force);

    for (auto particle : particles) {
        // Vec2 wind = Vec2(1.0 * PIXELS_PER_METER, 0.0);
        // particle->apply_force(wind);

        // Vec2 weight = Vec2(0.0, particle->mass * 9.8 * PIXELS_PER_METER);
        // particle->apply_force(weight);

        particle->apply_force(push_force);

        Vec2 friction = Force::generate_firction_force(*particle, 20);
        particle->apply_force(friction);

        // apply drag forces when in liquid
        // if (particle->position.y >= liquid.y) {
        // Vec2 drag = Force::generate_drag_force(*particle, 0.03);
        // particle->apply_force(drag);
        // }
    }

    for (auto particle : particles) {
        // Integrate the acceleration and velocity to estimate the new position
        particle->integrate(delta_time);
    }

    for (auto particle : particles) {
        if (particle->position.x - particle->radius <= 0) {
            // left border reached
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9;
        } else if (particle->position.x + particle->radius >=
                   Graphics::width()) {
            // reached right border
            particle->position.x = Graphics::width() - particle->radius;
            particle->velocity.x *= -0.9;
        }
        if (particle->position.y - particle->radius <= 0) {
            // top border reached
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        } else if (particle->position.y + particle->radius >=
                   Graphics::height()) {
            // reached bottom border
            particle->position.y = Graphics::height() - particle->radius;
            particle->velocity.y *= -0.9;
        }
    }
}

/**
 * Render function (called several times per second to draw objects)
 */
void App::render() {
    // `FF` - full opacity, no transparency
    Graphics::clear_screen(0xFF056263);

    // draw liquid
    // Graphics::draw_fill_rect(liquid.x + liquid.w / 2, liquid.y + liquid.h /
    // 2, liquid.w, liquid.h, 0xFF6E3713);

    if (left_mouse_button_down) {
        Graphics::draw_line(particles[0]->position.x, particles[0]->position.y,
                            mouse_cursor.x, mouse_cursor.y, 0xFF0000FF);
    }

    /*
    for (auto particle : particles) {

        Graphics::draw_fill_circle(particle->position.x, particle->position.y,
                                   particle->radius,
                                   0xFFFFFFFF); // all white circle
    }
    */
    Graphics::draw_fill_circle(particles[0]->position.x,
                               particles[0]->position.y, particles[0]->radius,
                               0xFFAA3300);
    Graphics::draw_fill_circle(particles[1]->position.x,
                               particles[1]->position.y, particles[1]->radius,
                               0xFF00FFFF);
    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void App::destroy() {
    for (auto particle : particles) {
        delete particle;
    }
    Graphics::close_window();
}
