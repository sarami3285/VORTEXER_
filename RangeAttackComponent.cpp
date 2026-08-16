#include "RangeAttackComponent.h"
#include "Actor.h"
#include "Player.h"
#include "MoveComponent.h"
#include "Math.h"
#include "LineComponent.h"
#include "Game.h"
#include "EnemyMissile.h"

RangedAttackComponent::RangedAttackComponent(Actor* owner, Player* player, bool useLaserSight)
	: EnemyBehaviorComponent(owner, 100)
	, mPlayer(player)
	, mFireCooldown(0.0f)
	, mUseLaserSight(useLaserSight)
	, mBurstCount(1)
	, mCurrentBurstCount(0)
	, mBurstDelay(0.1f)
	, mBurstDelayTimer(0.0f)
	, mChaseTurnSpeed(Math::TwoPi)
{
	SetBulletImage("Assets/Bullet.png");
}

void RangedAttackComponent::FireBullet(const Vector2& toPlayerDir)
{
	float rotation = Math::Atan2(toPlayerDir.y, toPlayerDir.x);

	if (mIsMissile)
	{
		EnemyMissile* missile = new EnemyMissile(
			mOwner->GetGame(),
			mOwner->GetPosition(),
			rotation,
			mBulletSpeed,
			mBulletLifeTime,
			mBulletDamage
		);

		mOwner->GetGame()->AddActor(missile);
	}
	else
	{
		EnemyBullet* bullet = new EnemyBullet(
			mOwner->GetGame(),
			mOwner->GetPosition(),
			rotation,
			mBulletSpeed,
			mBulletLifeTime,
			mBulletDamage,
			mBulletTexturePath
		);

		mOwner->GetGame()->AddActor(bullet);
	}
}

void RangedAttackComponent::UpdateLaserSight(float dist)
{
	if (!mUseLaserSight) return;

	if (!mLineComp) {
		mLineComp = mOwner->GetComponent<LineComponent>();
	}

	if (mLineComp)
	{
		if (dist <= mAttackRange)
		{
			mLineComp->SetTargetPos(mPlayer->GetPosition());
			mLineComp->SetVisibility(true);
		}
		else
		{
			mLineComp->SetVisibility(false);
		}
	}
}

void RangedAttackComponent::Execute(float dt)
{
	bool moveEnabled = true;
	if (mDisableMovement) {
		moveEnabled = false;
	}

	// 1. ターゲットの動的決定
	// 基本は一番近いターゲット（プレイヤーまたは防衛対象）を取得
	Actor* target = mOwner->GetGame()->GetNearestTarget(mOwner->GetPosition());

	// ターゲットがいない、または自身の状態が異常なら終了
	if (!mIsEnabled || !target || mOwner->GetState() != Actor::EAlive) return;

	mFireCooldown -= dt;

	// ターゲットへのベクトルを算出（mPlayerをtargetに置換）
	Vector2 toTarget = target->GetPosition() - mOwner->GetPosition();
	float dist = toTarget.Length();

	class MoveComponent* mc = mOwner->GetComponent<MoveComponent>();
	if (!mc) return;

	Vector2 dir = toTarget;
	dir.Normalize();

	// ターゲットの方を向く回転制御
	float targetAngle = Math::Atan2(dir.y, dir.x);
	float currentAngle = mOwner->GetRotation();
	float diff = Math::WrapAngle(targetAngle - currentAngle);
	float idealAngularSpeed = diff / dt;

	float rotationInput = Math::Clamp(idealAngularSpeed, -mChaseTurnSpeed, mChaseTurnSpeed);
	mc->SetAngularSpeed(rotationInput);

	if (!mDisableMovement)
	{
		if (dist > mAttackRange)
		{
			mc->SetForwardSpeed(mChaseSpeed);
			mCurrentBurstCount = 0;
			mBurstDelayTimer = 0.0f;
		}
		else
		{
			mc->SetForwardSpeed(0.0f);
		}
	}

	// ターゲットが射程内なら攻撃
	if (dist <= mAttackRange)
	{
		if (mCurrentBurstCount > 0)
		{
			mBurstDelayTimer -= dt;
			if (mBurstDelayTimer <= 0.0f)
			{
				FireBullet(dir);

				mCurrentBurstCount--;

				if (mCurrentBurstCount > 0)
				{
					mBurstDelayTimer = mBurstDelay;
				}
				else
				{
					mFireCooldown = mFireRate;
					mBurstDelayTimer = 0.0f;
				}
			}
		}
		else if (mFireCooldown <= 0.0f)
		{
			mCurrentBurstCount = mBurstCount;
			mBurstDelayTimer = 0.0f;
		}
	}

	if (mUseLaserSight)
	{
		// レーザーサイトもターゲットの位置に合わせる
		if (!mLineComp) {
			mLineComp = mOwner->GetComponent<LineComponent>();
		}
		if (mLineComp)
		{
			if (dist <= mAttackRange)
			{
				mLineComp->SetTargetPos(target->GetPosition());
				mLineComp->SetVisibility(true);
			}
			else
			{
				mLineComp->SetVisibility(false);
			}
		}
	}
}

void RangedAttackComponent::SetBulletImage(std::string BulletPath) {
	mBulletTexturePath = BulletPath;
}
