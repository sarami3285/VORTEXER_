#pragma once
#include "Enemy.h"
#include "Math.h"
#include "AudioComponent.h"
#include "SpriteComponent.h"
class Player;
class HUDSpriteComponent;

class LowLevelEnemy : public Enemy
{
public:
	LowLevelEnemy(class Game* game);
	~LowLevelEnemy();
	void UpdateActor(float deltaTime) override;

	void TakeDamage(float amount, const Vector2& bulletDir) override;
	void SetHPBar(HUDSpriteComponent* sprite);

	class CollisionComponent* GetCircle() const override { return mCollision; }
private:
	void Patrol(float dt);
	void Attack(float dt);

	class MoveComponent* mMoveComponent;
	SpriteComponent* mSpriteComponent;
	HUDSpriteComponent* mHPBarSprite = nullptr;

	Vector2 mTargetPos;
	Vector2 mPatrolCenter;
	float MapOffset = 250.0f;
	float mPatrolRadius;
	float mWaitTimer;
	float mFireCooldown = 0.0f;

	bool mIsDamaged = false;
	Vector3 mFixedColor = Vector3(1.0f, 1.0f, 1.0f);
};
