#include "app_rigid_body.h"
#include "body.h"
#include "collision_detection.h"
#include "constants.h"
#include "contact.h"
#include "force.h"
#include "graphics.h"
#include "shape.h"
#include "vec2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
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
    // Body *body = new Body(CircleShape(50), Graphics::width() / 2.0,
    // Graphics::height() / 2.0, 1.0);

    /*
    Body *box = new Body(BoxShape(200, 100), Graphics::width() / 2.0,
                         Graphics::height() / 2.0, 1.0);
    */

    // Body *big_ball = new Body(CircleShape(200), Graphics::width() / 2.0,
    // Graphics::height() / 2.0, 0.0);
    // Body *small_ball = new Body(CircleShape(50), 500, 100, 1.0);

    /*
    Body *box_a = new Body(BoxShape(200, 200), Graphics::width() / 2.0,
                           Graphics::height() / 2.0, 1.0);
    Body *box_b = new Body(BoxShape(200, 200), Graphics::width() / 2.0,
                           Graphics::height() / 2.0, 1.0);
    box_a->angular_vel = 0.4;
    box_b->angular_vel = 0.1;
    */

    Body *floor =
        new Body(BoxShape(Graphics::width() - 100, 50), Graphics::width() / 2.0,
                 Graphics::height() - 25, 0.0);
    floor->restitution = 0.5;
    Body *left_wall = new Body(BoxShape(50, Graphics::height()), 25,
                               Graphics::height() / 2.0, 0.0);
    left_wall->restitution = 0.2;
    Body *right_wall =
        new Body(BoxShape(50, Graphics::height()), Graphics::width() - 25,
                 Graphics::height() / 2.0, 0.0);
    right_wall->restitution = 0.2;
    bodies.push_back(floor);
    bodies.push_back(left_wall);
    bodies.push_back(right_wall);

    Body *big_box = new Body(BoxShape(200, 200), Graphics::width() / 2.0,
                             Graphics::height() / 2.0, 0.0);
    big_box->restitution = 0.7;
    big_box->rotation = 1.4;
    big_box->set_texture("./assets/crate.png");

    // Body *ball = new Body(CircleShape(50), Graphics::width() / 2.0,
    // Graphics::height() / 2.0, 1.0);
    // ball->restitution = 0.1;

    bodies.push_back(big_box);
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
            /*
        case SDL_MOUSEMOTION:
            int a, b;
            SDL_GetMouseState(&a, &b);
            bodies[1]->position.x = a;
            bodies[1]->position.y = b;
            break;
            */
            /*
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
            */
            /*
            case SDL_MOUSEBUTTONUP:
                if (left_mouse_button_down &&
                    event.button.button == SDL_BUTTON_LEFT) {
                    left_mouse_button_down = false;
                }
                break;
            case SDL_MOUSEMOTION:
                int x, y;
                SDL_GetMouseState(&x, &y);
                bodies[0]->position.x = x;
                bodies[0]->position.y = y;
                break;
            */
            /*
            case SDL_MOUSEBUTTONDOWN:
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *small_ball = new Body(CircleShape(40), x, y, 1.0);
                small_ball->restitution = 0.9;
                bodies.push_back(small_ball);
                break;
            */
            /*
        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);
            Body *ball = new Body(CircleShape(50), x, y, 1.0);
            ball->restitution = 0.5;
            ball->friction = 0.4;
            bodies.push_back(ball);
            break;
            */
            /*
        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);
            std::vector<Vec2> vertices = {Vec2(20, 60), Vec2(-40, 20),
                                          Vec2(-20, -60), Vec2(20, -60),
                                          Vec2(40, 20)};
            Body *poly = new Body(PolygonShape(vertices), x, y, 2.0);
            poly->restitution = 0.1;
            poly->friction = 0.7;
            bodies.push_back(poly);
            break;
            */
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *ball = new Body(CircleShape(30), x, y, 1.0);
                ball->set_texture("./assets/basketball.png");
                ball->restitution = 0.5;
                bodies.push_back(ball);
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *box = new Body(BoxShape(60, 60), x, y, 1.0);
                box->set_texture("./assets/crate.png");
                box->restitution = 0.2;
                bodies.push_back(box);
            }
            break;
        }
    }
}

/**
 * Update function (called several times per second to update objects)
 */
void AppRigidBody::update() {
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

        // Torque
        // float torque = 200;
        // body->apply_torque(torque);

        // wind
        // Vec2 wind = Vec2(2.0 * PIXELS_PER_METER, 0.0);
        // body->apply_force(wind);
    }

    for (auto body : bodies) {
        body->update(delta_time);
    }

    for (auto body : bodies) {
        body->is_colliding = false;
    }

    // Collision detection
    // check all rigidbodies with other rigidbodies
    for (size_t i = 0; i <= bodies.size() - 1; i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            Body *a = bodies[i];
            Body *b = bodies[j];

            Contact contact;

            // check bodies[i] with bodies[j]
            if (CollisionDetection::is_colliding(a, b, contact)) {
                contact.resolve_collision();

                if (debug) {
                    Graphics::draw_fill_circle(contact.start.x, contact.start.y,
                                               3, 0xFFFF00FF);
                    Graphics::draw_fill_circle(contact.end.x, contact.end.y, 3,
                                               0xFFFF00FF);
                    // only draw 15px of the line between two contact points
                    Graphics::draw_line(contact.start.x, contact.start.y,
                                        contact.normal.x * 15 + contact.start.x,
                                        contact.normal.y * 15 + contact.start.y,
                                        0xFFFF00FF);
                    // collision!
                    a->is_colliding = true;
                    b->is_colliding = true;
                }
            }
        }
    }

    /*
     * Check for boundaries
     */
    /*
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
    */
}

/**
 * Render function (called several times per second to draw objects)
 */
void AppRigidBody::render() {
    // `FF` - full opacity, no transparency
    // Graphics::clear_screen(0xFF056263);

    for (auto body : bodies) {
        Uint32 color = body->is_colliding ? 0xFF0000FF : 0xFFFFFFFF;
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
void AppRigidBody::destroy() {
    for (auto body : bodies) {
        delete body;
    }
    Graphics::close_window();
}
