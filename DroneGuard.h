#pragma once 
#include "Enemy.h"
#include "CollisionComponent.h" 

class DroneGuard : public Enemy
{
public:
    DroneGuard(class Game* game);
    virtual ~DroneGuard();

    void UpdateActor(float deltaTime) override;
    void TakeDamage(float amount, const Vector2& bulletDir) override;
    class CollisionComponent* GetCircle() const override { return mCollision; }

private:
};
