#pragma once

#include <vector>
#include <glm/fwd.hpp>

#include "myvec.h"
#include "gameEngine.h"
#include "renderer.h"

struct SpaceObj {
    myvec::vec3 pos, v, ndpos, ndv;
    myvec::vec3 npos, nv, derpos, derv;
    long double mass;
    SpaceObj();
    SpaceObj(long double a_mass, const myvec::vec3 &a_pos, const myvec::vec3 &a_v);

    // normalize the coord and move velocity back to being parallel the surface
    void normalize();

    // simulate gravity

    // get derivate
    // put result in derpos and derv
    template<class T>
    void get_der(const std::vector<T*> &objs);
    // Runge–Kutta methods
    template<class T>
    void gety1(const std::vector<T*> &objs);
    template<class T>
    void getk1(const std::vector<T*> &objs);
    template<class T>
    void gety2(const std::vector<T*> &objs);
    template<class T>
    void getk2(const std::vector<T*> &objs);
    template<class T>
    void gety3(const std::vector<T*> &objs);
    template<class T>
    void getk3(const std::vector<T*> &objs);
    template<class T>
    void gety4(const std::vector<T*> &objs);
    template<class T>
    void getk4(const std::vector<T*> &objs);
    void move();
};

struct Star : SpaceObj {
    Star();
    Star(long double a_mass, const myvec::vec3 &a_pos, const myvec::vec3 &a_v);
    void draw(const glm::mat4& viewProj, const Circle& circle) const;
};

struct Player: SpaceObj {
    // ship direction
    myvec::vec3 dir;

    // perpendicular direction (to the right)
    myvec::vec3 dirp;

    // direction of gridline
    myvec::vec3 dirg;
    myvec::vec3 dirpg;

    // position of gridline
    myvec::vec3 ceng;
    myvec::vec2 difg;

    Player();
    Player(long double a_mass,
        const myvec::vec3 &a_pos = {0.0, 0.0, 1.0},
        const myvec::vec3 &a_v = {0.0, 0.0, 0.0},
        const myvec::vec3 &a_dir = {0.0, 1.0, 0.0},
        const myvec::vec3 &a_dirg = {0.0, 1.0, 0.0},
        const myvec::vec3 &a_ceng = {0.0, 0.0, 1.0},
        const myvec::vec2 &a_difg = {0.0, 0.0}
    );

    void normalize();
    glm::mat4 gen_view_matrix() const;
};

class PlayState : public GameState {
    Circle circle;
    Grid grid;
    Triangle *ship_triangle;
    double prev_time;
    bool k_left,k_right,k_up,k_down,k_plus,k_minus;
    double cx,cy;
    Player player;
    std::vector<Star*> stars;
    float zoom;
    int seed;
    bool paused;
    bool space_pressed;
public:
    PlayState(int a_seed);
    ~PlayState();

    void pause();
    void resume();

    void handleEvents(GameEngine* game);
    void update(GameEngine* game);
    void draw(GameEngine* game);
};
