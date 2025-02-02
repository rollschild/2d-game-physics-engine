#include "app_constraint.h"
#include "app_particle.h"
#include "app_rigid_body.h"
#include "app_world.h"

int main(/* int argc, char *args[] */) {
    // AppParticle app;
    // AppRigidBody app;
    // AppWorld app;
    AppConstraint app;
    app.setup();

    while (app.is_running()) {
        app.input();
        app.update();
        app.render();
    }

    app.destroy();

    return 0;
}
