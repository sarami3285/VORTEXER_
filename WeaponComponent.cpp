#include "WeaponComponent.h"
#include "Game.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Random.h"
#include "Player.h"
#include "Enemy.h"
#include "AudioComponent.h"
#include "Bullet.h"
#include "Missile.h"

WeaponComponent::WeaponComponent(Actor* owner)
    : Component(owner)
    , mCooldown(0.0f)
    , mFireRateMultiplier(1.0f) //ここが 0 だと撃てなくなるので注意
    , mIsBurstFiring(false)
    , mAutoFire(false)
    , mBurstShotFired(0)
    , mBurstTimer(0.0f)
{
    mAudioComponent = new AudioComponent(owner);
    mAudioComponent->LoadSE("shot", "Assets/Audio/sen_ge_gun18.mp3");
    SetDamageRate(1.0f);
}

void WeaponComponent::Update(float deltaTime)
{
    if (mCooldown > 0.0f)
    {
        mCooldown -= deltaTime;
    }

    if (mIsBurstFiring)
    {
        mBurstTimer -= deltaTime;
        if (mBurstTimer <= 0.0f)
        {
            FireSingleShot(mFireDirection);
            mBurstShotFired++;
            if (mBurstShotFired >= mData.burstCount)
            {
                mIsBurstFiring = false;
                mBurstTimer = 0.0f;
            }
            else
            {
                mBurstTimer = mData.burstInterval;
            }
        }
    }
}

void WeaponComponent::Fire(const Vector2& direction)
{
    if (mCooldown <= 0.0f && !mIsBurstFiring)
    {
        //ゼロ除算防止
        float multiplier = (mFireRateMultiplier <= 0.0f) ? 1.0f : mFireRateMultiplier;

        mCooldown = mData.fireRate / multiplier;

        if (mData.burstCount > 1)
        {
            StartBurstFire(direction);
        }
        else
        {
            FireSingleShot(direction);
        }
    }
}

void WeaponComponent::StartBurstFire(const Vector2& direction)
{
    mIsBurstFiring = true;
    mBurstShotFired = 0;
    mBurstTimer = 0.0f;
    mFireDirection = direction;
}

void WeaponComponent::FireSingleShot(const Vector2& direction)
{
    if (mAudioComponent)
    {
        mAudioComponent->PlaySE("shot");
    }

    for (int i = 0; i < mData.pelletCount; ++i)
    {
        float angleOffset = 0.0f;
        if (mData.pelletCount > 1)
        {
            float spreadRad = Math::DegToRad(mData.spreadAngle);
            angleOffset = -spreadRad / 2 + (spreadRad / (float)(mData.pelletCount - 1)) * i;
        }

        float randomAngleDeg = Random::GetFloatRange(-mData.randomSpread, mData.randomSpread);
        float randomAngleRad = Math::DegToRad(randomAngleDeg);

        float finalAngle = angleOffset + randomAngleRad;
        Vector2 pelletDir = RotateVector(direction, finalAngle);

        Bullet* bullet = nullptr;

        // 弾のスプライト名で判断
        if (mData.bulletTexturePath == "Assets/SkillMissile.png")
        {
            Missile* missile = new Missile(GetOwner()->GetGame());
            if (Enemy* target = GetOwner()->GetGame()->GetNearestEnemy(GetOwner()->GetPosition()))
            {
                missile->SetTarget(target);
            }
            bullet = missile;
        }
        else
        {
            bullet = new Bullet(GetOwner()->GetGame());
        }

        if (bullet)
        {
            bullet->SetTexture(mData.bulletTexturePath);
            bullet->SetPosition(GetOwner()->GetPosition());
            bullet->SetRotation(Math::Atan2(pelletDir.y, pelletDir.x));
            bullet->SetBaseDamage(static_cast<float>(mData.baseDamage));
            bullet->SetSpeed(mData.bulletSpeed);
        }
    }
}

bool WeaponComponent::LoadWeapon(const std::string& filepath, const std::string& weaponName)
{
    return mData.LoadFromJSON(filepath, weaponName);
}
