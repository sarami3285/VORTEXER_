#pragma once
#include "Enemy.h"

class CollisionComponent;

class EnemyBase : public Enemy {
public:
    EnemyBase(class Game* game);
    ~EnemyBase();
    void UpdateActor(float dt) override;
    void TakeDamage(float amount, const Vector2& bulletDir) override;
    class CollisionComponent* GetCircle() const override { return mCollision; }
private:
    float mSpawnTimer = 0.0f;
    float mSpawnInterval = 9.0f;
};
