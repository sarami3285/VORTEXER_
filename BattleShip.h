#pragma once
#include "Enemy.h"
#include "Math.h"
#include <vector>

class Game;
class HPComponent;
class CollisionComponent;
class EnemyStateComponent;
class RangedAttackComponent;

class BattleShip : public Enemy
{
public:
    BattleShip(class Game* game, const Vector2& startPos);
    ~BattleShip();

    void UpdateActor(float deltaTime) override;

    void TakeDamage(float amount, const Vector2& bulletDir) override;

    class EnemyStateComponent* GetStateComponent() const { return mStateComponent; }
    class CollisionComponent* GetCircle() const override { return mCollision; }

private:
    CollisionComponent* mCollision;
    EnemyStateComponent* mStateComponent;
    RangedAttackComponent* mMainAttack;

    const float MAX_HP = 500.0f;
    const float MAX_SPEED = 40.0f;
    const float COLLISION_RADIUS = 50.0f;

    void SetupComponents(const Vector2& startPos);
};
