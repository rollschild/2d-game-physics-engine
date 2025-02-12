#include "app_constraint.h"
#include "body.h"
#include "constants.h"
#include "constraint.h"
#include "graphics.h"
#include "shape.h"
#include "vec2.h"
#include "world.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <vector>

bool AppConstraint::is_running() { return running; }

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void AppConstraint::setup() {
    running = Graphics::open_window();

    world = new World(-9.8);

    SDL_Surface *bg_surface = IMG_Load("./assets/angrybirds/background.png");
    if (bg_surface) {
        bg_texture =
            SDL_CreateTextureFromSurface(Graphics::renderer, bg_surface);
        SDL_FreeSurface(bg_surface);
    }

    // add bird
    Body *bird =
        new Body(CircleShape(45), 100, Graphics::height() / 2.0 + 220, 3.0);
    bird->set_texture("./assets/angrybirds/bird-red.png");
    world->add_body(bird);

    Body *floor =
        new Body(BoxShape(Graphics::width() - 50, 50), Graphics::width() / 2.0,
                 Graphics::height() / 2.0 + 350, 0.0);
    floor->restitution = 0.1;
    Body *left_fence = new Body(BoxShape(50, Graphics::height() - 200), 0,
                                Graphics::height() / 2.0 - 35, 0.0);
    Body *right_fence =
        new Body(BoxShape(50, Graphics::height() - 200), Graphics::width(),
                 Graphics::height() / 2.0 - 35, 0.0);
    world->add_body(floor);
    world->add_body(left_fence);
    world->add_body(right_fence);

    // add stack of boxes
    for (int i = 1; i <= 4; i++) {
        float mass = 10.0 / (float)i;
        Body *box = new Body(BoxShape(50, 50), Graphics::width() / 2.0 - 300,
                             floor->position.y - i * 55, mass);
        box->set_texture("./assets/angrybirds/wood-box.png");
        box->friction = 0.9;
        box->restitution = 0.1;
        world->add_body(box);
    }

    // add structure with blocks
    Body *plank1 = new Body(BoxShape(50, 150), Graphics::width() / 2.0 + 20,
                            floor->position.y - 100, 5.0);
    Body *plank2 = new Body(BoxShape(50, 150), Graphics::width() / 2.0 + 180,
                            floor->position.y - 100, 5.0);
    Body *plank3 = new Body(BoxShape(250, 25), Graphics::width() / 2.0 + 100.0f,
                            floor->position.y - 200, 2.0);
    // plank3->restitution = 0.1;
    plank1->set_texture("./assets/angrybirds/wood-plank-solid.png");
    plank2->set_texture("./assets/angrybirds/wood-plank-solid.png");
    plank3->set_texture("./assets/angrybirds/wood-plank-cracked.png");

    world->add_body(plank1);
    world->add_body(plank2);
    world->add_body(plank3);

    // add triangle polygon
    std::vector<Vec2> triangle_vertices = {Vec2(30, 30), Vec2(-30, 30),
                                           Vec2(0, -30)};
    Body *triangle = new Body(PolygonShape(triangle_vertices),
                              plank3->position.x, plank3->position.y - 50, 0.5);
    triangle->set_texture("./assets/angrybirds/wood-triangle.png");
    world->add_body(triangle);

    // add pyramid of boxes
    int num_rows = 5;
    for (int col = 0; col < num_rows; col++) {
        for (int row = 0; row < col; row++) {
            float x =
                (plank3->position.x + 200.0f) + col * 50.0f - (row * 25.0f);
            float y = (floor->position.y - 50.0f) - row * 52.0f;
            float mass = (5.0f / (row + 1.0f));
            Body *box = new Body(BoxShape(50, 50), x, y, mass);
            box->friction = 0.9;
            box->restitution = 0.0;
            box->set_texture("./assets/angrybirds/wood-box.png");
            world->add_body(box);
        }
    }

    // add a bridge of connected steps and joints
    int num_steps = 10;
    int spacing = 33;
    Body *start_step = new Body(BoxShape(80, 20), 200, 200, 0.0);
    start_step->set_texture("./assets/angrybirds/rock-bridge-anchor.png");
    world->add_body(start_step);
    Body *last = floor;
    for (int i = 1; i <= num_steps; i++) {
        float x = start_step->position.x + 30 + (i * spacing);
        float y = start_step->position.y + 20;
        float mass = (i == num_steps) ? 0.0 : 3.0;
        Body *step = new Body(CircleShape(15), x, y, mass);
        step->set_texture("./assets/angrybirds/wood-bridge-step.png");
        world->add_body(step);
        JointConstraint *joint =
            new JointConstraint(last, step, step->position);
        world->add_constraint(joint);
        last = step;
    }

    Body *end_step = new Body(BoxShape(80, 20), last->position.x + 60,
                              last->position.y - 20, 0.0);
    end_step->set_texture("./assets/angrybirds/rock-bridge-anchor.png");
    world->add_body(end_step);

    // add pigs
    Body *pig1 = new Body(CircleShape(30), plank1->position.x + 80,
                          floor->position.y - 50, 3.0);
    Body *pig2 = new Body(CircleShape(30), plank2->position.x + 400,
                          floor->position.y - 50, 3.0);
    Body *pig3 = new Body(CircleShape(30), plank2->position.x + 460,
                          floor->position.y - 50, 3.0);
    Body *pig4 = new Body(CircleShape(30), 220, 130, 1.0);
    pig1->set_texture("./assets/angrybirds/pig-1.png");
    pig2->set_texture("./assets/angrybirds/pig-2.png");
    pig3->set_texture("./assets/angrybirds/pig-1.png");
    pig4->set_texture("./assets/angrybirds/pig-2.png");
    world->add_body(pig1);
    world->add_body(pig2);
    world->add_body(pig3);
    world->add_body(pig4);
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
                world->get_bodies()[0]->apply_impulse_linear(Vec2(0.0, -600.0));
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                world->get_bodies()[0]->apply_impulse_linear(Vec2(+400.0, 0.0));
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                world->get_bodies()[0]->apply_impulse_linear(Vec2(-400.0, 0.0));
            }
            if (event.key.keysym.sym == SDLK_d) {
                debug = !debug;
            }
            break;
            /*
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
            */
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *rock = new Body(CircleShape(30), x, y, 1.0);
                rock->set_texture("./assets/angrybirds/rock-round.png");
                rock->friction = 0.4;
                world->add_body(rock);
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Body *box = new Body(BoxShape(60, 60), x, y, 1.0);
                box->set_texture("./assets/angrybirds/rock-box.png");
                box->angular_vel = 0.0;
                box->friction = 0.9;
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
    Graphics::draw_texture(Graphics::width() / 2.0, Graphics::height() / 2.0,
                           Graphics::width(), Graphics::height(), 0.0f,
                           bg_texture);

    // draw a line between joint objects
    // for (auto joint : world->get_constraints()) {
    // const Vec2 pa = joint->a->localspace_to_worldspace(joint->a_point);
    // const Vec2 pb = joint->b->localspace_to_worldspace(joint->a_point);
    // Graphics::draw_line(pa.x, pa.y, pb.x, pb.y, 0xFF555555);
    // }

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
            } else if (debug) {
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
            } else if (debug) {
                Graphics::draw_polygon(body->position.x, body->position.y,
                                       box_shape->world_vertices, color);
            }
        }
        if (body->shape->get_type() == POLYGON) {
            PolygonShape *polygon_shape = (PolygonShape *)body->shape;
            if (!debug && body->texture) {
                Graphics::draw_texture(
                    body->position.x, body->position.y, polygon_shape->width,
                    polygon_shape->height, body->rotation, body->texture);
            } else if (debug) {
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
    SDL_DestroyTexture(bg_texture);
    delete world;
    Graphics::close_window();
}
