#pragma once
#include "Component.h"
#include "Math.h"
#include "Bullet.h"

class BulletComponent : public Component
{
public:
    BulletComponent(class Actor* owner, int updateOrder = 100);

    void Update(float deltaTime) override;

    class Bullet* Fire(float deltaTime);

    void SetDamage(float damage) { mDamage = damage; }
    void SetBulletSpeed(float speed) { mBulletSpeed = speed; }
    void SetFireInterval(float interval) { mFireInterval = interval; }

    float GetDamage() const { return mDamage; }
    float GetBulletSpeed() const { return mBulletSpeed; }
    float GetFireInterval() const { return mFireInterval; }

    float mFireInterval;
    float mDamage;
    float mBulletSpeed;

private:
    float mFireTimer = 0.0f;
};
