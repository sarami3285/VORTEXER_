#pragma once
#include "Component.h"
#include <cstdint>
#include "Math.h"

class Player;
class PatrolComponent;
class OrbitAttackComponent;
class RangedAttackComponent;

class EnemyStateComponent : public Component
{
public:
	enum class EState
	{
		Patrol,
		Attack
	};

	EnemyStateComponent(class Actor* owner, class Player* player);

	void Update(float dt) override;
	void SetState(EState newState);
	void StartBehavior();
	void OnDamageTaken();

	void SetSearchRange(float amount) { mSearchRange = amount ; };

	void SetOrbitAttackComponent(OrbitAttackComponent* comp) { mOrbitAttackComp = comp; }
	void SetRangedAttackComponent(RangedAttackComponent* comp) { mRangedAttackComp = comp; }
	void SetPatrolComponent(PatrolComponent* comp) { mPatrolComp = comp; }

	void SetIsHunter(bool isHunter) { mIsHunter = isHunter; }

	PatrolComponent* mPatrolComp = nullptr;
	OrbitAttackComponent* mOrbitAttackComp = nullptr;
	RangedAttackComponent* mRangedAttackComp = nullptr;

	float mSearchRange = 800.0f;

private:
	void CheckTransitions();
	void HandleState(float dt);
	class Actor* mTarget = nullptr;

	EState mCurrentState;
	class Player* mPlayer;

	float mAttackDuration = 5.0f;
	float mAttackTimer = 0.0f;

	bool mIsHunter = false;
};
