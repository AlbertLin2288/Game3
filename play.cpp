// #include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>

#include "play.h"
#include "utils.h"

#define TPS 30
// time per tick in ms
constexpr float TIME_PER_TICK = 1000.0f / TPS;


PlayState::PlayState() {
    prev_time = 0;
    cx = cy = 0.0;
}

PlayState::~PlayState() {
    ;
}

void PlayState::pause() {
    ;
}

void PlayState::resume() {
    prev_time = 0.0;
}

void PlayState::handleEvents(GameEngine* game) {
    game->window->pollEvents();
    if (game->window->windowShouldClose()){
        game->quit();
        return;
    }
    if (game->window->getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        game->quit();
        return;
    }
    if (game->window->getKey(GLFW_KEY_UP) == GLFW_PRESS) {
        k_up = true;
    } else {
        k_up = false;
    }
    if (game->window->getKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
        k_down = true;
    } else {
        k_down = false;
    }
    if (game->window->getKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
        k_left = true;
    } else {
        k_left = false;
    }
    if (game->window->getKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
        k_right = true;
    } else {
        k_right = false;
    }
}

void PlayState::update(GameEngine* game) {
    double cur_time = game->window->getTime();
    if (prev_time != 0){
        prev_time = prev_time + TIME_PER_TICK;
    } else {
        prev_time = cur_time;
    }
    if (prev_time >= cur_time) {
        sleepms(prev_time - cur_time);
    } else {
        fprintf(stderr, "Can't keep up\n");
    }
    if (k_up) {
        cy += 0.05;
    }
    if (k_down) {
        cy -= 0.05;
    }
    if (k_left) {
        cx -= 0.05;
    }
    if (k_right) {
        cx += 0.05;
    }
}

void PlayState::draw(GameEngine* game) {
    game->window->fill(0.2f, 0.3f, 0.3f, 1.0f);
    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    model = glm::translate(model, glm::vec3(cx, cy, 0.0f));
    glm::mat4 view(1.0f);
    float aspect_ratio = game->window->get_aspect_ratio();
    glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f / aspect_ratio, 1.0f / aspect_ratio);
    glm::mat4 viewProj = proj * view;
    circle.draw(model, viewProj, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    game->window->swapBuffers();
}