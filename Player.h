#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "vector"
#include "Enemy.h" 
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "WeaponComponent.h"
#include "HPComponent.h"
#include "Bullet.h"
#include "UpgradeData.h"
#include "Game.h"
#include "Player.h"

class LockOnEffect;

enum class AimMode {
	Auto,
	Manual
};

class Player : public Actor
{
public:
	Player(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;
	void TakeDamage(int damage);
	void TakeRepair(int amount);

	void SetWeapon(const WeaponData& weapon);
	const WeaponData& GetWeapon() const;
	const std::string& GetCurrentWeaponName() const;

	void AddExperience(int amount);

	class Enemy* GetNearestEnemy(const std::vector<class Actor*>& actors);
	Vector2 ScreenToWorld(int screenX, int screenY);
	CameraComponent* GetCameraComponent() const { return mCameraComponent; }
	class CollisionComponent* GetCircle() { return mCollision; }

	const Vector2& GetVelocity() const { return mLastVelocity; }

	HPComponent* mHPComponent;
	InputComponent* ic;

	void SetBarrierActive(bool active) { BarrierRate = active ? 0.05f : 1.0f;}

	class Enemy* GetCurrentLockTarget() const { return mCurrentLockTarget; }
	AimMode GetAimMode() const { return mAimMode; }

	WeaponComponent* GetWeaponComponent() const { return mWeaponComponent; }

	int GetCurrentLevel() const { return mCurrentLevel; }
	int GetCurrentEXP() const { return mCurrentEXP; }
	int GetEXPToNextLevel() const { return mEXPToNextLevel; }
	bool UseSkill();

	void IncreaseDamage(float amount);
private:
	void UpdateSpriteBasedOnDirection(const Vector2& targetPos);

	Vector2 mLastVelocity;
	float mBrakeCooldown = 0.0f;

	float mDustTimer = 0.0f;
	bool mDustRightSideToggle = true;

	float lockOnRange = 350.0f;
	bool prevTabState = false;

	int mCurrentLevel = 1;     
	int mCurrentEXP = 0;       
	int mEXPToNextLevel = 100; 

	float DeffenceRate = 1.0f;
	float BarrierRate = 1.0f;

	bool mIsUpgrading = false;

	void LevelUp();
	int CalculateEXPForLevel(int level);
	void ApplyUpgrade(const UpgradeOption& opt);

	struct UpgradeUI {
		class HUDSpriteComponent* bg;
	};
	std::vector<UpgradeUI> mActiveUpgradeUIs;
	bool mIsWaitingForUpgrade = false;
	std::vector<UpgradeOption> mCurrentOptions; 
	std::vector<class Actor*> mUpgradeUIActors;

	Game* game;
	WeaponData CurrentWeapon;
	WeaponData nextWeapon;
	WeaponComponent* mWeaponComponent;
	CollisionComponent* mCollision;
	SpriteComponent* mSpriteComponent;
	LockOnEffect* mLockOnEffect = nullptr;
	class Enemy* mCurrentLockTarget = nullptr;
	AimMode mAimMode;
	CameraComponent* mCameraComponent;
	Bullet* bullet;
};
