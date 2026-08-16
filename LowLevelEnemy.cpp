#include "LowLevelEnemy.h"
#include "Game.h"
#include "Random.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "HPBarComponent.h"
#include "HPComponent.h"
#include "Player.h"
#include "CameraComponent.h"
#include "EnemyStateComponent.h"
#include "PatrolComponent.h"
#include "RangeAttackComponent.h"
#include "Math.h"
#include "DeathEffect.h"
#include "EnemyFragment.h"
#include "SpriteComponent.h"
#include "MetalFragment.h"
#include "DeathEnemy.h"

LowLevelEnemy::LowLevelEnemy(Game* game) : Enemy(game) {
    mHPComponent = new HPComponent(this, 75); mPlayer = game->GetPlayer();
    new HPBarComponent(this, mHPComponent);

    SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

    new MoveComponent(this, 0, 400.0f, 720.0f);
    mTexturePath = "Assets/Tank1.png";
    NUM_ExpFRAGMENTS = 3;
    SpriteComponent* sc = new SpriteComponent(this, 3, mTexturePath, true, Vector2{ 5,5 });
    mSprite = sc;
    sc->SetColor(1.0f, 1.0f, 1.0f);

    mCollision = new CollisionComponent(this, 25.0f);

    PatrolComponent* patrolComp = new PatrolComponent(this);
    RangedAttackComponent* attackComp = new RangedAttackComponent(this, mPlayer, false);
    EnemyStateComponent* stateComp = new EnemyStateComponent(this, mPlayer);

    this->SetStateComponent(stateComp);

    stateComp->SetRangedAttackComponent(attackComp);
    stateComp->SetPatrolComponent(patrolComp);
    stateComp->SetSearchRange(500.0f);

    attackComp->mChaseRange = 500.0f;
    attackComp->mAttackRange = 300.0f;
    attackComp->mChaseSpeed = 150.0f;
    attackComp->mFireRate = 1.0f;
    attackComp->mBulletSpeed = 600.0f;
    attackComp->mBulletDamage = 5;
}

LowLevelEnemy::~LowLevelEnemy() {}

void LowLevelEnemy::TakeDamage(float amount, const Vector2& bulletDir)
{
    Enemy::TakeDamage(amount, bulletDir);
}

void LowLevelEnemy::UpdateActor(float deltaTime)
{
    Enemy::UpdateActor(deltaTime);
}
