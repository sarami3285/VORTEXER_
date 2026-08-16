#pragma once
#include "Enemy.h"

class CollisionComponent;
class LineComponent;

class SentryGun : public Enemy {
public:
    SentryGun(class Game* game);
    ~SentryGun();

    void UpdateActor(float deltaTime) override;
    void TakeDamage(float amount, const Vector2& bulletDir) override;
    class CollisionComponent* GetCircle() const override { return mCollision; }

private:
    const float mSearchRange = 1800.0f;
    class LineComponent* mLineComp;
};
