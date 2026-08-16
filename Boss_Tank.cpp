#include "Boss_Tank.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Player.h"
#include "EnemyBullet.h"
#include "Random.h"
#include "CameraComponent.h"
#include "HPBarComponent.h"
#include "ArtilleryAttack.h" // 新設

Boss_Tank::Boss_Tank(Game* game)
    : Enemy(game), mFireTimer(0.0f), mIsRightBarrel(true), mMoveTimer(0.0f)
    , mTargetPos(Vector2::Zero), mMaxTurnSpeed(Math::Pi / 8.0f) {

    mBaseSprite = new SpriteComponent(this, 10, "Assets/Boss_Tank_body.png");
    mTurretSprite = new SpriteComponent(this, 11, "Assets/Boss_Tank_barrel.png");
    mHPComponent = new HPComponent(this, 400);
    new HPBarComponent(this, mHPComponent);
    mCollision = new CollisionComponent(this, 60.0f);
    mMoveComponent = new MoveComponent(this);
    mPlayer = game->GetPlayer();
}

void Boss_Tank::UpdateActor(float deltaTime) {
    Enemy::UpdateActor(deltaTime);
    if (!mPlayer) return;

    Vector2 toPlayer = mPlayer->GetPosition() - GetPosition();
    float dist = toPlayer.Length();

    float projectileTime = 0.9f;
    Vector2 pPos = mPlayer->GetPosition();
    Vector2 pVel = mPlayer->GetVelocity();
    Vector2 leadPos = pPos + (pVel * projectileTime);

    mMoveTimer -= deltaTime;
    if (mMoveTimer <= 0.0f) {
        float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
        mTargetPos = GetPosition() + Vector2(Math::Cos(angle), Math::Sin(angle)) * 400.0f;
        mTargetPos.x = Math::Clamp(mTargetPos.x, 100.0f, CameraComponent::mMapWidth - 100.0f);
        mTargetPos.y = Math::Clamp(mTargetPos.y, 100.0f, CameraComponent::mMapHeight - 100.0f);
        mMoveTimer = Random::GetFloatRange(3.0f, 5.0f);
        mMoveComponent->SetForwardSpeed(40.0f);
    }

    Vector2 toTarget = mTargetPos - GetPosition();
    if (toTarget.LengthSq() > 100.0f) {
        float diff = Math::Atan2(toTarget.y, toTarget.x) - GetRotation();
        while (diff > Math::Pi) diff -= Math::TwoPi;
        while (diff < -Math::Pi) diff += Math::TwoPi;
        SetRotation(GetRotation() + Math::Clamp(diff, -mMaxTurnSpeed * deltaTime, mMaxTurnSpeed * deltaTime));
    }

    float turretAngle = Math::Atan2(toPlayer.y, toPlayer.x);
    mTurretSprite->SetExplicitRotation(turretAngle);

    if (mBurstActive) {
        mBurstTimer -= deltaTime;
        if (mBurstTimer <= 0.0f) {
            float offset = mIsRightBarrel ? 22.0f : -22.0f;
            Vector2 fwd(Math::Cos(turretAngle), Math::Sin(turretAngle));
            Vector2 right(-fwd.y, fwd.x);
            Vector2 muzzle = GetPosition() + (fwd * 80.0f) + (right * offset);

            Vector2 currentLead = mPlayer->GetPosition() + (mPlayer->GetVelocity() * projectileTime);
            new ArtilleryAttack(GetGame(), muzzle, currentLead);

            mBurstActive = false;
            mIsRightBarrel = !mIsRightBarrel;
        }
    }

    mFireTimer -= deltaTime;
    if (mFireTimer <= 0.0f && !mBurstActive) {
        float offset = mIsRightBarrel ? 22.0f : -22.0f;
        Vector2 fwd(Math::Cos(turretAngle), Math::Sin(turretAngle));
        Vector2 right(-fwd.y, fwd.x);
        Vector2 muzzle = GetPosition() + (fwd * 80.0f) + (right * offset);

        if (dist > 500.0f && dist < 1200.0f) {
            Vector2 midPoint = (pPos + leadPos) * 0.5f;
            new ArtilleryAttack(GetGame(), muzzle, midPoint);

            mBurstActive = true;
            mBurstTimer = 0.3f;
            mFireTimer = 2.5f;
        }
        else if (dist <= 500.0f) {
            float spread = Random::GetFloatRange(-0.25f, 0.25f);
            EnemyBullet* b = new EnemyBullet(GetGame(), muzzle, turretAngle + spread, 800.0f, 1.0f, 5, "Assets/Bullet.png");
            b->SetPosition(muzzle);
            mFireTimer = 0.08f;
        }
        mIsRightBarrel = !mIsRightBarrel;
    }
}