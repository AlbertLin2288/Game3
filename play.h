#pragma once

#include "gameEngine.h"
#include "renderer.h"

class PlayState : public GameState {
    Circle circle;
    double prev_time;
    bool k_left,k_right,k_up,k_down;
    float cx,cy;
public:
    PlayState();
    ~PlayState();

    void pause();
    void resume();

    void handleEvents(GameEngine* game);
    void update(GameEngine* game);
    void draw(GameEngine* game);
};
