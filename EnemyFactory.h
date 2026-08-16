#pragma once
#include <string>
#include "MissionDataLoader.h"
#include "Math.h"

class Game;
class Enemy;

class EnemyFactory {
public:
    EnemyFactory(Game* game);
    Enemy* CreateEnemy(const EnemyConfig& config, Vector2 pos);

private:
    Game* mGame;
};