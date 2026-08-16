#pragma once
#include "Component.h"

enum EnemyState {
	BaseAttack,
	PlayerAttack,
	PlayerIntercept,
	Move
};

class EnemyAIComponent : public Component {
public:
	EnemyAIComponent(class Actor* owner);
	void Update(float deltaTime)override;
	void SetState(EnemyState state);
	EnemyState GetState()const;

private:
	void UpdateState(float deltaTime);
	void BaseAttack();
	void PlayerAttackPassive();
	void PlayerAttackAggressive();
	void Move();

	EnemyState mCurrentState;
	float mStateTime;
	class Player* mPlayer;
	class Base* mBase;
};
