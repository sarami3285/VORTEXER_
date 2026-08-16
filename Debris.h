#pragma once
#include "Actor.h"
#include "Game.h"
#include "SpriteComponent.h"
#include <string>

class Debris : public Actor {
public:
	Debris(Game* game , std::string TexturePath);

private:
	SpriteComponent* sprite;
};
