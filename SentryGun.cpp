#include "SentryGun.h"
#include "Game.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HPBarComponent.h"
#include "Player.h"
#include "CameraComponent.h"
#include "EnemyStateComponent.h"
#include "RangeAttackComponent.h"
#include "LineComponent.h"
#include "Math.h"
#include "DeathEffect.h"
#include "EnemyFragment.h"
#include "MetalFragment.h"
#include "DeathEnemy.h"

SentryGun::SentryGun(Game* game)
    : Enemy(game)
    , mLineComp(nullptr)
{
    const float SENTRY_SEARCH_RANGE = 1200.0f;

    mHPComponent = new HPComponent(this, 150);
    mCollision = new CollisionComponent(this, 35.0f);
    mPlayer = game->GetPlayer();

    SetPosition(Vector2(512.0f, 384.0f));
    SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));
    new MoveComponent(this, 0, 0.0f, 90.0f);

    mTexturePath = "Assets/Turret.png";
    SpriteComponent* sc = new SpriteComponent(this, 0 , mTexturePath, true , Vector2{5,5});
    mSprite = sc;
    sc->SetColor(1.0f, 1.0f, 1.0f);

    new HPBarComponent(this, mHPComponent);


    RangedAttackComponent* attackComp = new RangedAttackComponent(this, mPlayer, true);
    attackComp->mDisableMovement = false;
    attackComp->SetIsActive(true);

    attackComp->mAttackRange = SENTRY_SEARCH_RANGE;

    attackComp->mFireRate = 4.0f;
    attackComp->mChaseRange = SENTRY_SEARCH_RANGE;
    attackComp->mBulletSpeed = 1000.0f;
    attackComp->mBulletLifeTime = 3.0f;
    attackComp->mBulletDamage = 10;
    attackComp->SetBulletImage("Assets/StrongBullet.png");
    attackComp->mDisableMovement = true; 
    attackComp->SetIsActive(true);      

    EnemyStateComponent* stateComp = new EnemyStateComponent(this, mPlayer);

    this->SetStateComponent(stateComp);

    stateComp->SetSearchRange(SENTRY_SEARCH_RANGE); 
    stateComp->SetState(EnemyStateComponent::EState::Attack);

    NUM_ExpFRAGMENTS = 5;
}

SentryGun::~SentryGun()
{

}

void SentryGun::TakeDamage(float amount, const Vector2& bulletDir)
{
    Enemy::TakeDamage(amount, bulletDir);
}

void SentryGun::UpdateActor(float deltaTime) {
    Enemy::UpdateActor(deltaTime);
}
