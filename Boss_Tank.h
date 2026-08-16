#pragma once
#include "Enemy.h"

class Boss_Tank : public Enemy
{
public:
	Boss_Tank(class Game* game);
	void UpdateActor(float deltaTime) override;
	class CollisionComponent* GetCircle() const override { return mCollision; }
private:
	class SpriteComponent* mBaseSprite;
	class SpriteComponent* mTurretSprite;
	class MoveComponent* mMoveComponent;

	float mFireTimer;
	bool mIsRightBarrel;

	//主砲２連射用タイマー
	float mBurstTimer = 0.0f;
	bool mBurstActive = false;

	float mMoveTimer;
	Vector2 mTargetPos;
	float mMaxTurnSpeed;
};
