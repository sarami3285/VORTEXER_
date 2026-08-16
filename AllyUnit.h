#pragma once
#include "Actor.h"
#include <string>
#include "Math.h"
#include "AllyAIComponent.h" 
#include "CollisionComponent.h"
#include "HPBarComponent.h"
#include "HPComponent.h"

class MoveComponent;
class SpriteComponent;
class BulletComponent;

class AllyUnit : public Actor
{
public:
    AllyUnit(class Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position);
    virtual ~AllyUnit();

    void UpdateActor(float deltaTime) override;
    virtual void TakeDamage(int amount);
    class CollisionComponent* GetCircle() { return mCollision; }

    // AIを外部（ミッションローダー等）から操作するためのアクセサ
    class AllyAIComponent* GetAI() { return mAllyAI; }

protected:
    MoveComponent* mMove;
    AllyAIComponent* mAllyAI;
    HPComponent* mHP;
    CollisionComponent* mCollision;
    SpriteComponent* mSprite;
    BulletComponent* mBullet;
    class HPBarComponent* mHPBar;
};
