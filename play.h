#pragma once

#include <vector>
#include <glm/fwd.hpp>

#include "gameEngine.h"
#include "renderer.h"

struct SpaceObj {
    long double x, y, z;
    long double vx, vy, vz;
    long double mass;
    SpaceObj();
    SpaceObj(long double a_x, long double a_y, long double a_z,
        long double a_vx, long double a_vy, long double a_vz, long double a_mass);

    // normalize the coord and move velocity back to being parallel the surface
    void normalize();

    // simulate gravity
    template<class T>
    void fall(const std::vector<T*> objs);
    void move();
};

struct Star : SpaceObj {
    Star();
    Star(long double a_x, long double a_y, long double a_z,
        long double a_vx, long double a_vy, long double a_vz, long double a_mass);
    void draw(const glm::mat4& viewProj, const Circle& circle) const;
};

struct Player: SpaceObj {
    // ship direction
    long double dirx, diry, dirz;

    // perpendicular direction (to the right)
    long double dirpx, dirpy, dirpz;

    Player();
    Player(long double a_x, long double a_y, long double a_z,
        long double a_vx, long double a_vy, long double a_vz,
        long double a_dirx, long double a_diry, long double a_dirz,
        long double a_mass);

    void normalize();
    glm::mat4 gen_view_matrix() const;
};

class PlayState : public GameState {
    Circle circle;
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
