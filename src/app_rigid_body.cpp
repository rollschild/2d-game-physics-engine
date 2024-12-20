#include "app_rigid_body.h"
#include "body.h"
#include "constants.h"
#include "force.h"
#include "graphics.h"
#include "shape.h"
#include "vec2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_timer.h>
#include <vector>

bool AppRigidBody::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void AppRigidBody::setup() {
    running = Graphics::open_window();

    // CircleShape(50) is on stack, which will go out of scope once this setup
    // function ends thus we need the `.clone()` method
    Body *body = new Body(CircleShape(50), Graphics::width() / 2.0,
                          Graphics::height() / 2.0, 1.0);
    bodies.push_back(body);
}

/**
 * Input processing
 */
void AppRigidBody::input() {
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
                push_force.y = -50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                push_force.x = 50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                push_force.y = 50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                push_force.x = -50 * PIXELS_PER_METER;
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
            }
            break;
        }
    }
}

/**
 * Update function (called several times per second to update objects)
 */
void AppRigidBody::update() {
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

    // apply g force
    /*
    Vec2 g_force =
        Force::generate_g_force(*bodies[0], *bodies[1], 1000.0, 5, 100);
    bodies[0]->apply_force(g_force);
    bodies[1]->apply_force(-g_force);
    */

    if (bodies.size() > 0) {
        bodies[bodies.size() - 1]->apply_force(push_force);
    }

    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * 9.8 * PIXELS_PER_METER);
        body->apply_force(weight);
    }

    for (auto body : bodies) {
        body->integrate(delta_time);
    }

    for (auto body : bodies) {
        if (body->shape->get_type() == CIRCLE) {
            CircleShape *circle_shape = (CircleShape *)body->shape;
            if (body->position.x - circle_shape->radius <= 0) {
                // left border reached
                body->position.x = circle_shape->radius;
                body->velocity.x *= -0.9;
            } else if (body->position.x + circle_shape->radius >=
                       Graphics::width()) {
                // reached right border
                body->position.x = Graphics::width() - circle_shape->radius;
                body->velocity.x *= -0.9;
            }
            if (body->position.y - circle_shape->radius <= 0) {
                // top border reached
                body->position.y = circle_shape->radius;
                body->velocity.y *= -0.9;
            } else if (body->position.y + circle_shape->radius >=
                       Graphics::height()) {
                // reached bottom border
                body->position.y = Graphics::height() - circle_shape->radius;
                body->velocity.y *= -0.9;
            }
        }
    }
}

/**
 * Render function (called several times per second to draw objects)
 */
void AppRigidBody::render() {
    // `FF` - full opacity, no transparency
    Graphics::clear_screen(0xFF056263);

    static float angle = 0.0;

    for (auto body : bodies) {
        if (body->shape->get_type() == CIRCLE) {
            CircleShape *circle_shape = (CircleShape *)body->shape;
            // Graphics::draw_fill_circle(body->position.x, body->position.y,
            // circle_shape->radius, 0xFFFFFFFF);

            // rotation
            Graphics::draw_circle(body->position.x, body->position.y,
                                  circle_shape->radius, angle, 0xFFFFFFFF);
        }
    }

    angle += 0.01;

    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void AppRigidBody::destroy() {
    for (auto body : bodies) {
        delete body;
    }
    Graphics::close_window();
}
