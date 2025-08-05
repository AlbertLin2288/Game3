#include <stdio.h>

#include "gameEngine.h"
#include "play.h"

int main(){
    GameEngine gameEngine;
    PlayState *playState = new PlayState;

    gameEngine.pushState(playState);

    while (gameEngine.running()) {
            gameEngine.handleEvents();
            gameEngine.update();
            gameEngine.draw();
    }
    return 0;
}