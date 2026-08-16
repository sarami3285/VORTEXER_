#include "BattleShip.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "HPComponent.h"
#include "PatrolComponent.h"
#include "RangeAttackComponent.h"
#include "EnemyStateComponent.h"
#include "Game.h"
#include "Player.h"
#include "HPBarComponent.h"
#include "DeathEffect.h"
#include "EnemyFragment.h"
#include "Random.h"
#include "MetalFragment.h"
#include "DeathEnemy.h"

BattleShip::BattleShip(class Game* game, const Vector2& startPos)
	: Enemy(game)
{
	SetupComponents(startPos);
	NUM_ExpFRAGMENTS = 10;
}

BattleShip::~BattleShip()
{
}

void BattleShip::SetupComponents(const Vector2& startPos)
{
	SetPosition(startPos);
	mTexturePath = "Assets/LargeBattleShip.png";
	SpriteComponent* sc = new SpriteComponent(this, 3, mTexturePath, true, Vector2{10, 10});
	mSprite = sc;

	MoveComponent* mc = new MoveComponent(this , 0 , 50.0f , 1.0f);

	hpc = new HPComponent(this, 500);
	mHPComponent = hpc;
	new HPBarComponent(this, hpc);
	mCollision = new CollisionComponent(this,50.0f);

	Player* player = GetGame()->GetPlayer();

	RangedAttackComponent* rangedAttack = new RangedAttackComponent(this, player, false);
	rangedAttack->mAttackRange = 1100.0f;
	rangedAttack->mChaseRange = 800.0f;
	rangedAttack->mFireRate = 2.5f;
	rangedAttack->mBulletSpeed = 800.0f;
	rangedAttack->mCurrentBurstCount = 3;
	rangedAttack->mIsMissile = false;
	rangedAttack->mBurstCount = 3;
	rangedAttack->mChaseSpeed = 20.0f;
	rangedAttack->SetBulletImage("Assets/StrongBullet.png");

	PatrolComponent* patrol = new PatrolComponent(this);
	patrol->mPatrolRadius = 50.0f;
	patrol->mPatrolSpeed = 10.0f;

	mStateComponent = new EnemyStateComponent(this, player);
	mStateComponent->mRangedAttackComp = rangedAttack;
	mStateComponent->mPatrolComp = patrol;
	mStateComponent->mSearchRange = 600.0f;

	Speed = rangedAttack->mChaseSpeed;

	SetState(EAlive);
}

void BattleShip::UpdateActor(float deltaTime)
{
	Enemy::UpdateActor(deltaTime);
}

void BattleShip::TakeDamage(float amount, const Vector2& bulletDir) {
	if (!mHPComponent)
	{
		return;
	}
	Enemy::TakeDamage(amount, bulletDir);
}
