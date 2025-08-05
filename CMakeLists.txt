cmake_minimum_required(VERSION 3.10.0)
project(Game3 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(glad/include)
add_executable(Game3
    # main
    main.cpp
    # graphic
    shader.cpp
    renderer.cpp
    # graphic library
    glad/src/glad.c
    # game engine
    gameEngine.cpp
    # game
    play.cpp
    #utilities
    utils.cpp
)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.4)
# find_package(glfw3 REQUIRED)
target_link_libraries(Game3 glfw)

add_subdirectory(glm-1.0.1)
# find_package(glm CONFIG REQUIRED)
target_link_libraries(Game3 glm::glm)

find_package(OpenGL REQUIRED)
target_link_libraries(Game3 OpenGL::GL)

