#pragma once
#include "Component.h"
#include "Math.h"

class AllyAIComponent : public Component
{
public:
	enum EAllyUnitType { EDrone, ETank, EFighter ,ETurret};
	enum EAllyMode { EStay, EFollow, EAttack };

	AllyAIComponent(class Actor* owner);
	void Update(float deltaTime) override;

	void SetUnitType(EAllyUnitType type) { mUnitType = type; }
	void SetMode(EAllyMode mode) { mMode = mode; }
	void SetGuardTarget(class Actor* target) { mGuardTarget = target; }
	void SetOffset(const Vector2& offset) { mOffset = offset; }

private:
	void AttackProcess(float deltaTime);
	void OrbitProcess(float deltaTime);   // ドローン用
	void TurretProcess(float deltaTime);
	void TankProcess(float deltaTime);    // 戦車用
	void FighterProcess(float deltaTime); // 航空機用

	class Actor* SearchNearestEnemy();

	EAllyUnitType mUnitType = EDrone;
	EAllyMode mMode = EStay;
	class Actor* mGuardTarget = nullptr;

	float mAttackRange = 500.0f;
	float mOrbitAngle;
	float mAttackCooldown = 0.4f;
	Vector2 mOffset = Vector2::Zero;

	// 戦車用
	Vector2 mTankDest = Vector2::Zero;
	float mTankTimer = 0.0f;

	// 航空機用
	bool mIsPassing = true;
};
