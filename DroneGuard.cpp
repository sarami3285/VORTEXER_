#include "DroneGuard.h"
#include "Game.h"
#include "Random.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HPBarComponent.h"
#include "HPComponent.h"
#include "Player.h"
#include "CameraComponent.h"
#include "EnemyStateComponent.h"
#include "PatrolComponent.h"
#include "OrbitAttackComponent.h"
#include "EnemyFragment.h"
#include "DeathEffect.h"
#include "EnemyMissile.h"
#include "Math.h"
#include "RangeAttackComponent.h"
#include "MetalFragment.h"
#include "DeathEnemy.h"

DroneGuard::DroneGuard(Game* game)
	: Enemy(game)
{
	mHPComponent = new HPComponent(this, 30);
	mPlayer = game->GetPlayer();

	Vector2 minPos(CameraComponent::MapOffset, CameraComponent::MapOffset);
	Vector2 maxPos(CameraComponent::mMapWidth - CameraComponent::MapOffset, CameraComponent::mMapHeight - CameraComponent::MapOffset);

	SetPosition(GetRandomPositionInRange(minPos, maxPos));
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	new MoveComponent(this , 0 , 500.0f, 90.0f);

	mTexturePath = "Assets/Heri.png";
	NUM_ExpFRAGMENTS = 3;
	SpriteComponent* sc = new SpriteComponent(this, 100, mTexturePath, true ,Vector2 {50,50});
	mSprite = sc;
	sc->SetColor(1.0f, 1.0f, 1.0f);

	OrbitAttackComponent* orbitComp = new OrbitAttackComponent(this, mPlayer);
	orbitComp->mFireRate = 2.0f;
	orbitComp->mBulletSpeed = 400.0f;
	orbitComp->mBulletLifeTime = 2.0f;
	orbitComp->mBulletDamage = 6;
	orbitComp->mBurstCount = 4;
	orbitComp->mCurrentBurstCount = orbitComp->mBurstCount;
	orbitComp->mBurstDelay = 0.3f;
	orbitComp->mCurrentBurstCount = 4;
	orbitComp->mOrbitDistance = 450.0f;
	orbitComp->mOrbitSpeed = 200.0f;
	orbitComp->mMaxChaseDistance = 700.0f;
	orbitComp->mAttackRange = 470.0f;
	orbitComp->mChaseTurnSpeed;

	mCollision = new CollisionComponent(this , 20.0f);

	new HPBarComponent(this, mHPComponent);

	PatrolComponent* patrolComp = new PatrolComponent(this);

	EnemyStateComponent* stateComp = new EnemyStateComponent(this, mPlayer);
	this->SetStateComponent(stateComp);

	stateComp->SetOrbitAttackComponent(orbitComp);
	stateComp->SetPatrolComponent(patrolComp);

	RangedAttackComponent* missileComp = new RangedAttackComponent(this, mPlayer, true);
	missileComp->mIsMissile = true;
	missileComp->mFireRate = 10.0f;
	missileComp->mBulletSpeed = 300.0f;
    missileComp->mBurstCount = 1;
	missileComp->mBulletLifeTime = 6.0f;
	missileComp->mBulletDamage = 20;
	missileComp->mDisableMovement = true;
	missileComp->mAttackRange = 600.0f;

	missileComp->SetIsActive(true);

	stateComp->SetRangedAttackComponent(missileComp); 

	SetEnemyState(EnemyStateComponent::EState::Attack);
}

DroneGuard::~DroneGuard() {}

void DroneGuard::UpdateActor(float deltaTime)
{
	Enemy::UpdateActor(deltaTime);
}

void DroneGuard::TakeDamage(float amount, const Vector2& bulletDir)
{
	Enemy::TakeDamage(amount, bulletDir);
}
