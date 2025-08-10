#include <stdio.h>
#include <random>

#include "gameEngine.h"
#include "play.h"

int main(){
    GameEngine gameEngine;
    // PlayState *playState = new PlayState(std::random_device()());
    PlayState *playState = new PlayState(0);

    gameEngine.pushState(playState);

    while (gameEngine.running()) {
            gameEngine.handleEvents();
            gameEngine.update();
            gameEngine.draw();
    }
    return 0;
}