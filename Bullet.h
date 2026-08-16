#pragma once
#include "Actor.h"
#include "Math.h"
#include <memory>
#include "ObjectPool.h"

class MoveComponent;
class SpriteComponent;
class CollisionComponent;

class Bullet : public Actor
{
public:
	void* operator new(size_t size);
	void operator delete(void* p);


	enum EBulletOwner
	{
		EPlayer,
		EAlly,
		EEnemy
	};

	Bullet(class Game* game);
	~Bullet() = default;
	void Init();

	void UpdateActor(float deltaTime) override;

	void SetSpeed(float speed);
	void SetMaxLifeTime(float time) { mMaxLifeTime = time; }
	void SetBaseDamage(float damage) { mDamage = damage; }
	void SetDecayRate(float rate) { mDecayRate = rate; }
	void SetDamageRate(float amount) { DamageRate = amount; };
	void SetTexture(const std::string& path);
	void SetOwnerType(EBulletOwner owner) { mOwnerType = owner; }

	EBulletOwner GetOwnerType() const { return mOwnerType; }
	class SpriteComponent* GetSprite() const { return mSpriteComponent; }
	class CollisionComponent* GetCircle() const { return mCollision; } 
	float GetBaseDamage() const { return mDamage; }

protected:
	bool isDecay = true;
	bool isEffect = false;
	float mDeathTimer = 1.0f;
	float mDamage = 10.0f;
	float DamageRate = 1.0f;
	CollisionComponent* mCollision;

private:
	SpriteComponent* mSpriteComponent;
	MoveComponent* mMoveComponent;

	float mMaxLifeTime = 1.0f;  
	float mDecayRate = 0.0f;  

	bool mIsRicocheting = false;
	float mRicochetTimer = 0.0f;

	EBulletOwner mOwnerType = EPlayer;
};
