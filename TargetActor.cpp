#include "TargetActor.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HPBarComponent.h"
#include "CameraComponent.h"
#include "EnemyStateComponent.h"
#include "Math.h"
#include "DeathEffect.h"

TargetActor::TargetActor(Game* game)
    : Enemy(game)
{
    SetRotation(0.0f);

    mHPComponent = new HPComponent(this, 1);

    SpriteComponent* sc = new SpriteComponent(this,1, "Assets/EnergyBuild.png" , true , Vector2{4,4});

    mCollision = new CollisionComponent(this , 35.0f);

    new HPBarComponent(this, mHPComponent);
}

TargetActor::~TargetActor()
{
    
}

void TargetActor::TakeDamage(float amount, const Vector2& bulletDir)
{
    if (mHPComponent) {
        mHPComponent->TakeDamage(static_cast<int>(amount));

        if (mHPComponent->IsDead())
        {
            DeathEffect* deathEffect = new DeathEffect(GetGame(), 20);
            deathEffect->SetPosition(GetPosition());
            SetState(State::EStop);
        }
    }
}

void TargetActor::UpdateActor(float deltaTime) {
    Enemy::UpdateActor(deltaTime);
}

CollisionComponent* TargetActor::GetCircle() const
{
    return mCollision;
}

HPComponent* TargetActor::GetHPComponent() const
{
    return mHPComponent;
}
