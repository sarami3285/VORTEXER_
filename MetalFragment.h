#pragma once
#include "Actor.h"
#include "Game.h"
#include <Math.h> 

class MetalFragment : public Actor {
public:
	MetalFragment(class Game* game, const Vector2& startPos);
	~MetalFragment();

	void UpdateActor(float deltaTime) override;
	int GetExperienceValue() const { return mExperienceValue; }
	class CollisionComponent* GetCircle() const override { return mCollision; }

	float SUCTION_RADIUS = 100.0f;
	float SUCTION_RADIUS_SQ = SUCTION_RADIUS * SUCTION_RADIUS;
	float SUCTION_ACCELERATION = 300.0f; 

private:
	int mExperienceValue;
	float mTimeToLive;

	class SpriteComponent* mSprite;
	class CollisionComponent* mCollision;
	class MoveComponent* mMove;
};
