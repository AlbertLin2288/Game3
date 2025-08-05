#pragma once

#include <vector>
#include "renderer.h"

class GameEngine;
class GameState;

class GameEngine {
public:
    Window *window;

    GameEngine();
    ~GameEngine();

    void changeState(GameState* state);
    void pushState(GameState* state);
    void popState();

    void handleEvents();
    void update();
    void draw();

    bool running();
    void quit();

private:
    // the stack of states
    std::vector<GameState*> states;

    bool m_running;
};

class GameState {
public:
    virtual ~GameState();

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvents(GameEngine* game) = 0;
    virtual void update(GameEngine* game) = 0;
    virtual void draw(GameEngine* game) = 0;

    void changeState(GameEngine* game, GameState* state);
};
