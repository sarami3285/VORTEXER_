#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HPComponent.h"
#include "HPBarComponent.h"

class AllyBuilding : public Actor {
public:
    AllyBuilding(class Game* game, const Vector2& position, int hp, const std::string& texture);
    void TakeDamage(int amount);
    void UpdateActor(float deltaTime) override;
    class CollisionComponent* GetCircle() { return mCollision; }

private:
    SpriteComponent* mSprite;
    CollisionComponent* mCollision;
    HPComponent* mHP;
    HPBarComponent* mHPBar;
};
