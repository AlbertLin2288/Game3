// #include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>
#include <random>

#include "play.h"
#include "utils.h"

#define TPS 30
// time per tick in ms
constexpr float TIME_PER_TICK = 1000.0f / TPS;

// stars count
constexpr int STAR_COUNT_MIN = 350;
constexpr int STAR_COUNT_MAX = 550;

// stars mass
constexpr long double STAR_MASS_MEAN = 100.0;
constexpr long double STAR_MASS_VAR = 10.0;

// gravitation constant
constexpr long double G = 1.0;

// average velocity
constexpr long double STAR_VELOCITY = 0.01;

SpaceObj::SpaceObj(){}
SpaceObj::SpaceObj(long double a_x, long double a_y, long double a_z,
    long double a_vx, long double a_vy, long double a_vz, long double a_mass) :
    x(a_x), y(a_y), z(a_z), vx(a_vx), vy(a_vy), vz(a_vz), mass(a_mass) {
    normalize();
}

void SpaceObj::normalize() {
    long double norm = sqrt(x*x+y*y+z*z);
    x /= norm;
    y /= norm;
    z /= norm;
    long double v_per_norm = x * vx + y * vy + z * vz;
    vx -= v_per_norm * x;
    vy -= v_per_norm * y;
    vz -= v_per_norm * z;
}


Star::Star() {}

Star::Star(long double a_x, long double a_y, long double a_z,
    long double a_vx, long double a_vy, long double a_vz,
    long double a_mass) :
    SpaceObj(a_x, a_y, a_z, a_vx, a_vy, a_vz, a_mass) {
    }

void Star::draw(const glm::mat4& viewProj, const Circle& circle) const {
    // scale
    long double s = 0.003;
    long double l = s / sqrt(x*x+y*y);
    glm::mat4 model(
        y*l, -x*l, 0.0l, 0.0l,
        x*z*l, y*z*l, -x*x*l-y*y*l, 0.0l,
        x, y, z, 0.0l,
        0.0l, 0.0l, 0.0l, 1.0l
    );
    circle.draw(model, viewProj, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
}


Player::Player() {};

Player::Player(long double a_x, long double a_y, long double a_z,
    long double a_vx, long double a_vy, long double a_vz,
    long double a_dirx, long double a_diry, long double a_dirz,
    long double a_mass) :
    SpaceObj(a_x, a_y, a_z, a_vx, a_vy, a_vz, a_mass), dirx(a_dirx), diry(a_diry), dirz(a_dirz) {
    normalize();
}

void Player::normalize() {
    SpaceObj::normalize();
    long double d_per_norm = x * dirx + y * diry + z * dirz;
    dirx -= d_per_norm * x;
    diry -= d_per_norm * y;
    dirz -= d_per_norm * z;

    // perpendicular direction (to the right)
    dirpx = diry * z - dirz * y;
    dirpy = dirz * x - dirx * z;
    dirpz = dirx * y - diry * x;
}

glm::mat4 Player::gen_view_matrix() const {
    return glm::lookAt(glm::vec3(0.0l, 0.0l, 0.0l), glm::vec3(-x, -y, -z), glm::vec3(dirx, diry, dirz));
}


PlayState::PlayState(int a_seed) : seed(a_seed) {
    prev_time = 0;
    cx = cy = 0.0;
    player = Player(0, 0, 1, 0, 0, 0, 0, 1, 0, 1);
    stars.clear();
    std::mt19937 gen(seed);

    int star_cnt = STAR_COUNT_MIN + std::binomial_distribution<int>(STAR_COUNT_MAX-STAR_COUNT_MIN)(gen);

    std::gamma_distribution<long double> mass_gen(
        STAR_MASS_MEAN * STAR_MASS_MEAN / STAR_MASS_VAR,
        STAR_MASS_VAR / STAR_MASS_MEAN
    );

    std::normal_distribution<long double> velocity_gen(0.0, STAR_VELOCITY/sqrt(2));

    std::uniform_real_distribution<long double> pos_gen(-1.0, 1.0);

    for (int i=0;i<star_cnt;i++){
        stars.push_back(Star(
            pos_gen(gen), pos_gen(gen), pos_gen(gen),
            velocity_gen(gen), velocity_gen(gen), velocity_gen(gen),
            mass_gen(gen)
        ));
    }

    paused = false;
}

PlayState::~PlayState() {
    ;
}

void PlayState::pause() {
    paused = true;
}

void PlayState::resume() {
    paused = false;
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

    long double dt = TIME_PER_TICK/1000.0l;

    if (k_up) {
        // cy += 0.05;
        player.vx += 0.05l * dt * player.dirx;
        player.vy += 0.05l * dt * player.diry;
        player.vz += 0.05l * dt * player.dirz;
    }
    if (k_down) {
        player.vx -= 0.05l * dt * player.dirx;
        player.vy -= 0.05l * dt * player.diry;
        player.vz -= 0.05l * dt * player.dirz;
    }
    if (k_left ^ k_right) {
        long double ct = cos(0.03), st = sin(0.03);
        if (k_left) st = -st;
        player.dirx = ct * player.dirx + st * player.dirpx;
        player.diry = ct * player.diry + st * player.dirpy;
        player.dirz = ct * player.dirz + st * player.dirpz;
    }
    player.x += player.vx * dt;
    player.y += player.vy * dt;
    player.z += player.vz * dt;
    player.normalize();
}

void PlayState::draw(GameEngine* game) {
    game->window->fill(0.2f, 0.3f, 0.3f, 1.0f);
    glm::mat4 view = player.gen_view_matrix();
    float scale = 3000.0f;
    view = glm::scale(view, glm::vec3(scale));
    float aspect_ratio = game->window->get_aspect_ratio();
    auto size = game->window->get_size();
    int width = size.first;
    int height = size.second;
    glm::mat4 proj = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -scale-1.0f, 0.0f);
    glm::mat4 viewProj = proj * view;
    for (auto& star:stars) {
        star.draw(viewProj, circle);
    }

    game->window->swapBuffers();
}