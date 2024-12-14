add_executable(main)
find_package(SDL2 REQUIRED)
target_link_libraries(main SDL2::SDL2)
target_sources(main PRIVATE main.cpp app.cpp graphics.cpp vec2.cpp particle.cpp)
