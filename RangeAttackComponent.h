#pragma once
#include "EnemyBehaviorComponent.h"
#include "Math.h"

#include <string>

class Player;
class LineComponent;
class EnemyBullet; 

class RangedAttackComponent : public EnemyBehaviorComponent
{
public:
	RangedAttackComponent(class Actor* owner, class Player* player, bool useLaserSight);

	void Execute(float dt) override;

	void SetIsActive(bool active) override
	{
		Component::SetIsActive(active);
		mIsEnabled = active;
	}
	void SetBulletImage(std::string BulletPath);

	float mChaseRange = 500.0f;
	float mAttackRange = 300.0f;
	float mChaseSpeed = 150.0f;
	float mFireRate = 2.0f;
	float mBulletSpeed = 600.0f;
	float mBulletLifeTime = 3.0f;
	int mBulletDamage = 5;
	bool mIsMissile = false;
	bool mDisableMovement = false;
	int mBurstCount = 1;
	float mBurstDelay = 0.15f;
	int mCurrentBurstCount = 0;
	float mBurstDelayTimer = 0.0f;
	bool mIsEnabled = false;

	void FireBullet(const Vector2& toPlayerDir);

protected:
	void UpdateLaserSight(float dist);

	class Player* mPlayer;
	float mFireCooldown = 1.0f;
	bool mUseLaserSight;
	float mChaseTurnSpeed = 3.0f;
	class LineComponent* mLineComp = nullptr;
	std::string mBulletTexturePath;
};
