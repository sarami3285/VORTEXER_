#include "Missile.h"
#include "Game.h"
#include "Enemy.h"
#include "Math.h"
#include "CollisionComponent.h"
#include "DeathEffect.h"

Missile::Missile(Game* game)
	: Bullet(game)
	, mTarget(nullptr)
{
	float lifeTime = 3.0f;
	SetMaxLifeTime(lifeTime);
	mDeathTimer = lifeTime;

	SetScale(0.9f);
	SetSpeed(400.0f);
	SetTexture("Assets/SkillMissile.png");
	SetBaseDamage(20.0f);
	SetDecayRate(0.0f);
	isDecay = false;
}

void Missile::UpdateActor(float deltaTime)
{
	for (auto actor : GetGame()->GetActors())
	{
		Enemy* enemy = dynamic_cast<Enemy*>(actor);

		if (enemy && enemy->GetState() == Actor::EAlive)
		{
			if (Intersect(*mCollision, *enemy->GetCircle()))
			{
				DeathEffect* effect = new DeathEffect(GetGame(), 1);
				effect->SetPosition(GetPosition());
				effect->SetScale(0.7f);

				enemy->TakeDamage(mDamage * DamageRate, GetForward());
				SetState(EStop);
				return;
			}
		}
	}

	Bullet::UpdateActor(deltaTime);

	if (mTarget && mTarget->GetState() == Actor::EAlive)
	{
		Vector2 toTarget = mTarget->GetPosition() - GetPosition();
		toTarget.Normalize();

		Vector2 currentDir = GetForward();
		float crossProduct = currentDir.x * toTarget.y - currentDir.y * toTarget.x;

		float turnSpeed = Math::Pi * 2.2f;
		float newRotation = GetRotation();

		if (crossProduct > 0.0f) { newRotation += turnSpeed * deltaTime; }
		else { newRotation -= turnSpeed * deltaTime; }

		SetRotation(newRotation);
	}

	if (isEffect) {
		DeathEffect* effect = new DeathEffect(GetGame() , 1);
		effect->SetPosition(GetPosition());
		effect->SetScale(0.7f);
	}
}

void Missile::SetTarget(Actor* target)
{
	mTarget = target;
}
