#pragma once
#include "EnemyStateComponent.h"
#include "Actor.h"
#include <memory>
#include "Math.h"
#include "SpriteComponent.h"

class EnemyBehaviorComponent;
class Player;
class HPComponent;
class CollisionComponent;

class Enemy : public Actor
{
public:
    Enemy(class Game* game);
    ~Enemy();

    void UpdateActor(float deltaTime) override;
    void TakeDamage(float amount, const Vector2& bulletDir);

    void SetActiveBehavior(class EnemyBehaviorComponent* behavior);
    void SetEnemyState(EnemyStateComponent::EState newState);
    void SetStateComponent(class EnemyStateComponent* comp) { mStateComponent = comp; }

protected:
    class HPComponent* mHPComponent = nullptr;
    class CollisionComponent* mCollision = nullptr;
    SpriteComponent* mSprite = nullptr;
    class HPComponent* hpc;            
    class Player* mPlayer = nullptr;
    Vector2 GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos);
    std::string mTexturePath;
    int NUM_ExpFRAGMENTS = 3;
    float Speed = 0.0f;

    bool IsUnit = true;

    class EnemyStateComponent* mStateComponent = nullptr;

private:
    class EnemyBehaviorComponent* mActiveBehaviorComponent = nullptr;
};
