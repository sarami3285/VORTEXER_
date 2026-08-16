#include "Enemy.h"
#include "Random.h"
#include "Game.h"
#include "EnemyBehaviorComponent.h"
#include "EnemyStateComponent.h" 
#include "DeathEnemy.h"
#include "DeathEffect.h"
#include <string>

Enemy::Enemy(class Game* game)
    : Actor(game)
{
    game->AddEnemy(this);
    SetState(EAlive);
}

Enemy::~Enemy()
{
    if (GetGame())
    {
        GetGame()->RemoveEnemy(this);
    }
}

void Enemy::TakeDamage(float amount, const Vector2& bulletDir) {
    if (!mHPComponent) {
        return;
    }

    mHPComponent->TakeDamage(static_cast<int>(amount));

    if (mStateComponent) {
        mStateComponent->OnDamageTaken();
    }

    if (mSprite) {
        mSprite->FlashRed(0.1f);
    }

    if (mHPComponent->IsDead()) {
        if (GetState() == EAlive) {
            Vector2 launchVel = bulletDir * 800.0f;
            if (IsUnit) {
                new DeathEnemy(GetGame(), GetPosition(), GetRotation(), mTexturePath, NUM_ExpFRAGMENTS, launchVel, Speed);
            }
            else {
                DeathEffect* deathEffect = new DeathEffect(GetGame(), 6);
                deathEffect->SetPosition(GetPosition());
            }
            
            SetState(EStop);
        }
    }
}

void Enemy::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);

    if (mStateComponent) {
        mStateComponent->Update(deltaTime);
    }

    if (mActiveBehaviorComponent) {
        mActiveBehaviorComponent->Execute(deltaTime);
    }
}

void Enemy::SetActiveBehavior(class EnemyBehaviorComponent* behavior) {
    mActiveBehaviorComponent = behavior;
}

void Enemy::SetEnemyState(EnemyStateComponent::EState newState)
{
    if (mStateComponent) {
        mStateComponent->SetState(newState);
    }
}

Vector2 Enemy::GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos)
{
    float x = Random::GetFloatRange(minPos.x, maxPos.x);
    float y = Random::GetFloatRange(minPos.y, maxPos.y);
    return Vector2(x, y);
}
