#include "gameEngine.h"

GameEngine::GameEngine() {
    m_running = true;
    window = new Window();
}

GameEngine::~GameEngine() {
    delete window;
}

void GameEngine::changeState(GameState *state) {
    if (!states.empty()) {
        delete states.back();
    }
    states.push_back(state);
}

void GameEngine::pushState(GameState *state) {
    if (!states.empty()) {
        states.back()->pause();
    }
    states.push_back(state);
}

void GameEngine::popState() {
    if (!states.empty()) {
        delete states.back();
        states.pop_back();
    }
    if (!states.empty()) {
        states.back()->resume();
    }
}

void GameEngine::handleEvents() {
    if (!states.empty()){
        states.back() -> handleEvents(this);
    }
}

void GameEngine::update() {
    if (!states.empty()){
        states.back()->update(this);
    }
}

void GameEngine::draw() {
    if (!states.empty()){
        states.back()->draw(this);
    }
}

bool GameEngine::running() {
    return m_running;
}

void GameEngine::quit() {
    m_running = false;
}

GameState::~GameState(){}

void GameState::changeState(GameEngine* game, GameState* state) {
    game->changeState(state);
}