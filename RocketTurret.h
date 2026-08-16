#pragma once
#include "Enemy.h"

class RocketTurret : public Enemy {
public:
    RocketTurret(class Game* game);
    void UpdateActor(float dt) override;
    class CollisionComponent* GetCircle() const override { return mCollision; }
private:
    float mAttackCooldown;
    float mAttackTimer;
    int mShotsRemaining;
    float mShotIntervalTimer;

    void FireRocket();
};
