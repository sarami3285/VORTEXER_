#include "BulletComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Bullet.h"
#include "AllyUnit.h"

BulletComponent::BulletComponent(class Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
{
}

void BulletComponent::Update(float deltaTime)
{
    mFireTimer -= deltaTime;
}

class Bullet* BulletComponent::Fire(float deltaTime)
{
    if (mFireTimer <= 0.0f)
    {
        Bullet* bullet = new Bullet(mOwner->GetGame());
        bullet->SetPosition(mOwner->GetPosition());
        bullet->SetRotation(mOwner->GetRotation());
        bullet->SetBaseDamage(mDamage);
        bullet->SetSpeed(mBulletSpeed);

        if (dynamic_cast<AllyUnit*>(mOwner)) {
            bullet->SetOwnerType(Bullet::EAlly);
            bullet->SetTexture("Assets/AllyBullet.png");
        }
        else {
            bullet->SetOwnerType(Bullet::EPlayer);
        }

        mFireTimer = mFireInterval;
        return bullet;
    }
    return nullptr;
}
