#pragma once
#include "Enemy.h"
#include "CollisionComponent.h"

class Interceptor : public Enemy
{
public:
    Interceptor(class Game* game);
    virtual ~Interceptor() {}
    void UpdateActor(float deltaTime) override;

    virtual void TakeDamage(float amount, const Vector2& bulletDir) override {
        Enemy::TakeDamage(amount, bulletDir);
    }

    class CollisionComponent* GetCircle() const override { return mCollision; }
    void FireMachineGun();

private:
    CollisionComponent* mCollision;
    class HighSpeedPassComponent* mAI;
    class RangedAttackComponent* mMissile;

    float mFireTimer;
    float mFireInterval;
};
