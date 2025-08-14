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
constexpr long double dt = TIME_PER_TICK * 0.1l;

// grid
constexpr int gridmode = 1; // 0: rectangular grid, 1: spherical grid

constexpr long double RECT_GRID_CELL_SIZE = 0.04l; // world units

constexpr long double GRID_CELL_HEIGHT = M_PI * 2 / 120; // radians
constexpr long double GRID_CELL_WIDTH = M_PI * 2 / 36;

// stars count
constexpr int STAR_COUNT_MIN = 350;
constexpr int STAR_COUNT_MAX = 550;

// stars mass
constexpr long double STAR_MASS_MEAN = 100.0l;
constexpr long double STAR_MASS_VAR = 10.0l;

// gravitation constant
constexpr long double G = 0.0000000001l;

// average velocity
constexpr long double STAR_VELOCITY = 0.01l;


// radius
constexpr long double STAR_RADIUS = 0.003;

// player acceleration
constexpr long double player_acc = 0.00001l;

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
    long double v_norm = vx * vx + vy * vy + vz * vz;
    long double v_per_norm = x * vx + y * vy + z * vz;
    vx -= v_per_norm * x;
    vy -= v_per_norm * y;
    vz -= v_per_norm * z;
    long double n_v_norm = vx * vx + vy * vy + vz * vz;
    if (n_v_norm != 0) {
        v_norm = sqrt(v_norm / n_v_norm);
        vx *= v_norm;
        vy *= v_norm;
        vz *= v_norm;
    }
}

template<class T>
void SpaceObj::fall(const std::vector<T*> objs) {
    for (SpaceObj* obj: objs) {
        if (obj == this) continue;
        long double dx = obj->x - x;
        long double dy = obj->y - y;
        long double dz = obj->z - z;
        long double d = dx * dx + dy * dy + dz * dz;
        // ignore objects too far away
        if (d > 3.0) continue;
        if (d < STAR_RADIUS * STAR_RADIUS * 0.1l) continue;
        d = acos(1 - d/2);

        long double d_per_norm = dx * x + dy * y + dz * z;
        dx -= d_per_norm * x;
        dy -= d_per_norm * y;
        dz -= d_per_norm * z;
        long double d_norm = 1.0l / sqrt(dx * dx + dy * dy + dz * dz);
        dx *= d_norm;
        dy *= d_norm;
        dz *= d_norm;

        long double dv = G * mass * obj->mass / (d * d) * dt;

        vx += dv * dx / mass;
        vy += dv * dy / mass;
        vz += dv * dz / mass;
    }
}

void SpaceObj::move() {
    x += vx * dt;
    y += vy * dt;
    z += vz * dt;
}

Star::Star() {}

Star::Star(long double a_x, long double a_y, long double a_z,
    long double a_vx, long double a_vy, long double a_vz,
    long double a_mass) :
    SpaceObj(a_x, a_y, a_z, a_vx, a_vy, a_vz, a_mass) {
    }

void Star::draw(const glm::mat4& viewProj, const Circle& circle) const {
    // scale
    long double l = STAR_RADIUS / sqrt(x*x+y*y);
    glm::mat4 model(
        y*l, -x*l, 0.0l, 0.0l,
        x*z*l, y*z*l, -x*x*l-y*y*l, 0.0l,
        x, y, z, 0.0l,
        0.0l, 0.0l, 0.0l, 1.0l
    );
    if (mass>STAR_MASS_MEAN * 20)
    circle.draw(model, viewProj, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    else
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

    long double d_norm = 1.0l/sqrt(dirx*dirx+diry*diry+dirz*dirz);

    dirx *= d_norm;
    diry *= d_norm;
    dirz *= d_norm;

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
    zoom = 3000.0;
    player = Player(0, 0, 1, 0, 0, 0, 0, 1, 0, 1);
    ship_triangle = new Triangle({0.0, 10.0, 0.0}, {3.0, -5.0, 0.0}, {-3.0, -5.0, 0.0});
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
        stars.push_back(new Star(
            pos_gen(gen) * 2, pos_gen(gen), pos_gen(gen),
            velocity_gen(gen), velocity_gen(gen), velocity_gen(gen),
            mass_gen(gen)
        ));
    }

    stars.push_back(new Star(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, STAR_MASS_MEAN * 50.0));
    stars.push_back(new Star(-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, STAR_MASS_MEAN * 50.0));

    paused = false;
    p_pressed = false;
}

PlayState::~PlayState() {
    for (auto star: stars) {
        delete star;
    }
    delete ship_triangle;
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
    if (game->window->getKey(GLFW_KEY_P)==GLFW_PRESS && !p_pressed){
        paused=!paused;
        p_pressed=true;
    }
    if (game->window->getKey(GLFW_KEY_P)==GLFW_RELEASE){
        p_pressed=false;
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
    if (game->window->getKey(GLFW_KEY_EQUAL) == GLFW_PRESS) {
        k_plus = true;
    } else {
        k_plus = false;
    }
    if (game->window->getKey(GLFW_KEY_MINUS) == GLFW_PRESS) {
        k_minus = true;
    } else {
        k_minus = false;
    }
}

void PlayState::update(GameEngine* game) {
    double cur_time = game->window->getTime();
    if(paused) { 
        prev_time = cur_time;
        return;
    }
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
        // cy += 0.05;
        player.vx += player_acc * dt * player.dirx;
        player.vy += player_acc * dt * player.diry;
        player.vz += player_acc * dt * player.dirz;
    }
    if (k_down) {
        player.vx -= player_acc * dt * player.dirx;
        player.vy -= player_acc * dt * player.diry;
        player.vz -= player_acc * dt * player.dirz;
    }
    if (k_left ^ k_right) {
        long double ct = cos(0.03), st = sin(0.03);
        if (k_left) st = -st;
        player.dirx = ct * player.dirx + st * player.dirpx;
        player.diry = ct * player.diry + st * player.dirpy;
        player.dirz = ct * player.dirz + st * player.dirpz;
    }
    if (k_plus && zoom < 50000.0) zoom *= 1.01;
    if (k_minus && zoom > 400.0) zoom *= 0.99;
    for (Star* star: stars) {
        // star->fall(stars);
    }
    player.fall(stars);
    for (Star* star: stars) {
        // star->move();
        star->normalize();
    }
    player.move();
    player.normalize();
}

void PlayState::draw(GameEngine* game) {
    game->window->fill(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 view = player.gen_view_matrix();
    view = glm::scale(view, glm::vec3(zoom));
    float aspect_ratio = game->window->get_aspect_ratio();
    auto size = game->window->get_size();
    int width = size.first;
    int height = size.second;
    glm::mat4 proj = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -zoom * 2, 0.0f);
    glm::mat4 viewProj = proj * view;

    // Gridline code, for here rn but wrap in own function later
    {
        if(gridmode==1){
            static Shader gridShader("../shaders/gridShader.vs","../shaders/gridShader.fs");
            gridShader.use();
            gridShader.setMat4f("viewProj", viewProj);
            int seg = 100;
            for (long double phi = GRID_CELL_HEIGHT; phi < M_PI - GRID_CELL_HEIGHT + 1e-9; phi += GRID_CELL_HEIGHT) {
                std::vector<float> line;
                for (int j = 0; j <= seg; j++) {
                    long double theta = 2 * M_PI * j / seg;
                    float a = sin(phi) * cos(theta);
                    float b = sin(phi) * sin(theta);
                    float c = cos(phi);
                    line.push_back(a);
                    line.push_back(b);
                    line.push_back(c);
                }
                GLuint VBO, VAO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, line.size() * sizeof(float), line.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_LINE_STRIP, 0, seg + 1);
                glDeleteBuffers(1, &VBO);
                glDeleteVertexArrays(1, &VAO);
            }
            for (long double theta = 0; theta < 2 * M_PI; theta += GRID_CELL_WIDTH) {
                std::vector<float> line;
                for (int j = 0; j <= seg; j++) {
                    long double phi = M_PI * j / seg;
                    float a = sin(phi) * cos(theta);
                    float b = sin(phi) * sin(theta);
                    float c = cos(phi);
                    line.push_back(a);
                    line.push_back(b);
                    line.push_back(c);
                }
                GLuint VBO, VAO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, line.size() * sizeof(float), line.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_LINE_STRIP, 0, seg + 1);
                glDeleteBuffers(1, &VBO);
                glDeleteVertexArrays(1, &VAO);
            }
        } else {
            static Shader rectGridShader("../shaders/rectGrid.vs","../shaders/rectGrid.fs");
            rectGridShader.use();
            glm::mat4 invViewProj = glm::inverse(viewProj);
            rectGridShader.setMat4f("invViewProj", invViewProj);
            rectGridShader.setFloat("cellSize", RECT_GRID_CELL_SIZE);
            // Pass the player's world position so that the grid remains in world space.
            rectGridShader.setVec2f("gridOffset", glm::vec2(player.x, player.y));
            
            static GLuint VAO = 0, VBO;
            if (VAO == 0) {
                float quad[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
            }
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }
    }
    for (auto star:stars) {
        star->draw(viewProj, circle);
    }
    glm::mat4 E(1.0);
    ship_triangle->draw(E, proj, {0.6, 0.6, 0.6, 1.0});

    game->window->swapBuffers();
}