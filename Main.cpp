#include "Game.h"

int main(int argc, char** argv) {
    Game game;

    bool success = game.Initialize();

    if (success) {
        game.RunLoop();
    }

    game.Shutdown();
    return success ? 0 : 1;
}
