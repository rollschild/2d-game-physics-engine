#ifndef APP_H
#define APP_H

#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <vector>

class App {
  private:
    bool running = false;
    std::vector<Particle *> particles;

    Vec2 push_force = Vec2(0, 0); // controlled by keyboard

  public:
    App() = default;
    ~App() = default;
    bool is_running();
    void setup();
    void input();
    void update();
    void render();
    void destroy();
};

#endif
