add_executable(main)
find_package(SDL2 REQUIRED)
target_link_libraries(main SDL2::SDL2)
target_sources(
  main
  PRIVATE main.cpp
          app_particle.cpp
          graphics.cpp
          vec2.cpp
          particle.cpp
          force.cpp
          app_rigid_body.cpp
          shape.cpp
          body.cpp)
