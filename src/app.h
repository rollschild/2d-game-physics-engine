#ifndef APP_H
#define APP_H

#include "graphics.h"

class App {
  private:
    bool running = false;

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
