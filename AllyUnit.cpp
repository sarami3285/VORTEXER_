#include "AllyUnit.h"
#include "Game.h"
#include "MoveComponent.h"
#include "AllyAIComponent.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "BulletComponent.h"
#include "DeathEffect.h"

AllyUnit::AllyUnit(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position)
    : Actor(game)
{
    SetPosition(position);

    // 基本コンポーネントの一括生成
    mHP = new HPComponent(this, 50);
    mHPBar = new HPBarComponent(this, mHP);
    mSprite = new SpriteComponent(this, 2, "Assets/AllyDrone.png", true, Vector2{ 4,4 });
    mMove = new MoveComponent(this, 0, 100.0f, 180.0f);
    mAllyAI = new AllyAIComponent(this);
    mCollision = new CollisionComponent(this, 25.0f);
    mBullet = new BulletComponent(this);

    // 初期設定
    if (mAllyAI) mAllyAI->SetMode(mode);
    mBullet->SetFireInterval(0.4f);
    mBullet->SetBulletSpeed(600.0f);
    mBullet->SetDamage(10.0f);

    GetGame()->AddAlly(this);
    GetGame()->AddTargetActor(this);
}

AllyUnit::~AllyUnit() {
    GetGame()->RemoveAlly(this);
}

void AllyUnit::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);
}

void AllyUnit::TakeDamage(int amount) {
    mHP->TakeDamage(amount);
    if (mHP->IsDead() && GetState() == EAlive) {
        DeathEffect* deathEffect = new DeathEffect(GetGame(), 1);
        deathEffect->SetPosition(GetPosition());
        SetState(EStop);
    }
}
