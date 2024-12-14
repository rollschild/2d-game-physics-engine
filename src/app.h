#ifndef APP_H
#define APP_H

#include "graphics.h"
#include "particle.h"

class App {
  private:
    bool running = false;
    Particle *particle;

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
