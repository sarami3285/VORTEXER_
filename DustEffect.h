#pragma once
#include "Actor.h"

class DustEffect : public Actor {
public:
    DustEffect(class Game* game, const Vector2& position, float rotation);
    void UpdateActor(float deltaTime) override;

private:
    class SpriteComponent* mSprite;
    float mLifeTime;
};