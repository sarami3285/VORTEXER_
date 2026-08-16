#pragma once
#include "Actor.h"
#include "Math.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"

class EnemyFragment : public Actor
{
public:
    EnemyFragment(class Game* game, const Vector2& startPos, const Vector2& initialVelocity);
    void UpdateActor(float deltaTime) override;

private:
    class MoveComponent* mMove;
    class SpriteComponent* mSprite;
    float mLifetime;  
};
