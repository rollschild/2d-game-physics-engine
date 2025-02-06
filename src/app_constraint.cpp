#include "app_constraint.h"
#include "body.h"
#include "constants.h"
#include "constraint.h"
#include "graphics.h"
#include "shape.h"
#include "vec2.h"
#include "world.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>

bool AppConstraint::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void AppConstraint::setup() {
    running = Graphics::open_window();

    world = new World(-9.8);

    /*
    const int NUM_BODIES = 8;
    for (int i = 0; i < NUM_BODIES; i++) {
        float mass = (i == 0) ? 0.0 : 1.0;
        Body *body = new Body(BoxShape(30, 30),
                              Graphics::width() / 2.0 - (i * 40), 100, mass);
        body->set_texture("./assets/crate.png");
        world->add_body(body);
    }

    for (int j = 0; j < NUM_BODIES - 1; j++) {
        Body *a = world->get_bodies()[j];
        Body *b = world->get_bodies()[j + 1];
        JointConstraint *joint = new JointConstraint(a, b, a->position);
        world->add_constraint(joint);
    }
    */

    Body *big_ball = new Body(CircleShape(64), Graphics::width() / 2.0,
                              Graphics::height() / 2.0, 0.0);
    big_ball->set_texture("./assets/blowingball.png");
    world->add_body(big_ball);

    Body *floor =
        new Body(BoxShape(Graphics::width() - 50, 50), Graphics::width() / 2.0,
                 Graphics::height() - 50, 0.0);
    Body *left_wall = new Body(BoxShape(50, Graphics::height() - 100), 50,
                               Graphics::height() / 2.0 - 25, 0.0);
    Body *right_wall =
        new Body(BoxShape(50, Graphics::height() - 100), Graphics::width() - 50,
                 Graphics::height() / 2.0 - 25, 0.0);
    floor->restitution = 0.7;
    left_wall->restitution = 0.2;
    right_wall->restitution = 0.2;
    world->add_body(floor);
    world->add_body(left_wall);
    world->add_body(right_wall);
}

/**
 * Input processing
 */
void AppConstraint::input() {
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
            if (event.key.keysym.sym == SDLK_d) {
                debug = !debug;
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
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *ball = new Body(CircleShape(64), x, y, 1.0);
                ball->set_texture("./assets/basketball.png");
                ball->restitution = 0.7;
                world->add_body(ball);
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *box = new Body(BoxShape(140, 140), x, y, 1.0);
                box->set_texture("./assets/crate.png");
                box->restitution = 0.2;
                world->add_body(box);
            }
            break;
        }
    }
}

/**
 * Update function (called several times per second to update objects)
 */
void AppConstraint::update() {
    // clear screen here, instead of in `render` function
    Graphics::clear_screen(0xFF056263);

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

    world->update(delta_time);
}

/**
 * Render function (called several times per second to draw objects)
 */
void AppConstraint::render() {
    // `FF` - full opacity, no transparency
    // Graphics::clear_screen(0xFF056263);

    // draw a line between joint objects
    for (auto joint : world->get_constraints()) {
        const Vec2 pa = joint->a->localspace_to_worldspace(joint->a_point);
        const Vec2 pb = joint->b->localspace_to_worldspace(joint->a_point);
        Graphics::draw_line(pa.x, pa.y, pb.x, pb.y, 0xFF555555);
    }

    for (auto body : world->get_bodies()) {
        // Uint32 color = body->is_colliding ? 0xFF0000FF : 0xFFFFFFFF;
        Uint32 color = 0xFF0000FF;
        if (body->shape->get_type() == CIRCLE) {
            CircleShape *circle_shape = (CircleShape *)body->shape;
            // Graphics::draw_fill_circle(body->position.x, body->position.y,
            // circle_shape->radius, color);

            if (!debug && body->texture) {
                Graphics::draw_texture(body->position.x, body->position.y,
                                       circle_shape->radius * 2,
                                       circle_shape->radius * 2, body->rotation,
                                       body->texture);
            } else {
                Graphics::draw_circle(body->position.x, body->position.y,
                                      circle_shape->radius, body->rotation,
                                      color);
            }
        }
        if (body->shape->get_type() == BOX) {
            BoxShape *box_shape = (BoxShape *)body->shape;
            if (!debug && body->texture) {
                Graphics::draw_texture(body->position.x, body->position.y,
                                       box_shape->width, box_shape->height,
                                       body->rotation, body->texture);
            } else {
                Graphics::draw_polygon(body->position.x, body->position.y,
                                       box_shape->world_vertices, color);
            }
        }
        if (body->shape->get_type() == POLYGON) {
            PolygonShape *polygon_shape = (PolygonShape *)body->shape;
            if (!debug) {
                Graphics::draw_fill_polygon(body->position.x, body->position.y,
                                            polygon_shape->world_vertices,
                                            color);
            } else {
                Graphics::draw_polygon(body->position.x, body->position.y,
                                       polygon_shape->world_vertices, color);
            }
        }
    }

    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void AppConstraint::destroy() {
    delete world;
    Graphics::close_window();
}
