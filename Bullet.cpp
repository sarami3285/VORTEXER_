#include "Bullet.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Enemy.h"
#include "Game.h"
#include "Random.h"
#include "CameraComponent.h"
#include "Math.h"
#include "AllyUnit.h"
#include <memory>

static ObjectPool<Bullet> sBulletPool(256);

void* Bullet::operator new(size_t size)
{
	if (size != sizeof(Bullet)) {
		return ::operator new(size);
	}
	return sBulletPool.Allocate();
}

void Bullet::operator delete(void* p)
{
	if (sBulletPool.Owns(static_cast<Bullet*>(p))) {
		sBulletPool.Free(static_cast<Bullet*>(p));
	}
	else {
		::operator delete(p);
	}
}

Bullet::Bullet(Game* game) : Actor(game) {
	if (mComponents.empty()) {
		mSpriteComponent = new SpriteComponent(this, 20, "Assets/AllyBullet.png", false, Vector2{ 0,0 });
		mMoveComponent = new MoveComponent(this, 0, 800.0f, 0.0f);
		mCollision = new CollisionComponent(this, 11.0f);
	}
	Init();
}

void Bullet::Init() {
	SetState(EAlive);
	mDeathTimer = mMaxLifeTime;
	mIsRicocheting = false;
}

void Bullet::SetSpeed(float speed)
{
	if (mMoveComponent)
	{
		mMoveComponent->SetForwardSpeed(speed);
	}
}

void Bullet::UpdateActor(float deltaTime) {
	const float MAX_RICOCHET_TIME = 0.5f;
	const float RICOSHET_THRESHOLD = mDamage / 2.0f;
	const float MAX_OFFSET_RAD = Math::Pi / 3.0f;

	if (mIsRicocheting) {
		mRicochetTimer -= deltaTime;
		if (mRicochetTimer <= 0.0f) {
			SetState(EStop);
			return;
		}
	}

	if (!mIsRicocheting) {
		mDeathTimer -= deltaTime;
		if (mDeathTimer <= 0.0f) {
			isEffect = true;
			SetState(EStop);
			return;
		}
	}

	float effectiveDamage = mDamage * powf((mDeathTimer / mMaxLifeTime), mDecayRate);

	if (mSpriteComponent && isDecay) {
		float damageRatio = effectiveDamage / mDamage;
		float alphaValue = damageRatio;

		if (alphaValue < 0.0f) {
			alphaValue = 0.0f;
		}
		mSpriteComponent->SetAlpha(alphaValue);
	}

	Vector2 pos = GetPosition();
	if (pos.x <= 0.0f || pos.x >= CameraComponent::mMapWidth ||
		pos.y <= 0.0f || pos.y >= CameraComponent::mMapHeight)
	{
		SetState(EStop);
		return;
	}

	if (!mIsRicocheting) {
		Vector2 bulletDirection = mMoveComponent->GetVelocity();
		if (bulletDirection.LengthSq() > 0.0f)
		{
			bulletDirection.Normalize();
		}

		if (mOwnerType == EPlayer || mOwnerType == EAlly)
		{
			for (auto enemy : GetGame()->GetEnemies())
			{
				if (enemy->GetState() == Actor::EAlive)
				{
					CollisionComponent* enemyCC = enemy->GetCircle();
					if (enemyCC != nullptr && Intersect(*mCollision, *enemyCC))
					{
						enemy->TakeDamage(effectiveDamage * DamageRate, bulletDirection);

						if (effectiveDamage < RICOSHET_THRESHOLD)
						{
							mIsRicocheting = true;
							mRicochetTimer = MAX_RICOCHET_TIME;
							float randomOffset = Random::GetFloat() * (2.0f * MAX_OFFSET_RAD) - MAX_OFFSET_RAD;
							float newRotation = GetRotation() + Math::Pi + randomOffset;
							SetRotation(newRotation);
							break;
						}
						else
						{
							SetState(EStop);
							break;
						}
					}
				}
			}
		}

		if (mOwnerType == EEnemy)
		{
			for (auto actor : GetGame()->GetActors())
			{
				if (auto ally = dynamic_cast<AllyUnit*>(actor))
				{
					CollisionComponent* allyCC = ally->GetCircle();

					if (ally->GetState() == Actor::EAlive && allyCC != nullptr && Intersect(*mCollision, *allyCC))
					{
						ally->TakeDamage(effectiveDamage * DamageRate);
						SetState(EStop);
						return;
					}
				}
			}
		}
	}
}

void Bullet::SetTexture(const std::string& path)
{
	if (mSpriteComponent)
	{
		mSpriteComponent->SetTexture(GetGame()->GetTexture(path));
	}
}
