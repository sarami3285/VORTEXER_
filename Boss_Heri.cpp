#include "Boss_Heri.h"
#include "Game.h"
#include "Player.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "EnemyBullet.h"
#include "EnemyMissile.h"
#include "ArtilleryAttack.h"
#include "Random.h"
#include "Math.h"
#include "HPComponent.h"
#include "HPBarComponent.h"

Boss_Heri::Boss_Heri(Game* game)
    : Enemy(game)
    , mCurrentPhase(Tracking)
    , mPhaseTimer(PHASE_DURATION)
    , mAttackTimer(0.0f)
    , mMissileStep(0)
    , mArtilleryCount(0)
{
    mPlayer = game->GetPlayer();
    mSprite = new SpriteComponent(this, 100, "Assets/Boss_Heri.png");
    mMoveComponent = new MoveComponent(this);
    mHPComponent = new HPComponent(this, 1000);
    new HPBarComponent(this, mHPComponent);
    mCollision = new CollisionComponent(this, 120.0f);
}

void Boss_Heri::UpdateActor(float deltaTime)
{
    Enemy::UpdateActor(deltaTime);
    if (!mPlayer || mPlayer->GetState() != Actor::EAlive) return;

    Vector2 pos = GetPosition();
    Vector2 pPos = mPlayer->GetPosition();
    Vector2 toPlayer = pPos - pos;
    float dist = toPlayer.Length();

    // --- 1. 目標座標（TargetPoint）の更新 ---
    mOrbitAngle += 0.4f * deltaTime;
    Vector2 targetPos = pPos + Vector2(
        Math::Cos(mOrbitAngle) * 500.0f,
        Math::Sin(mOrbitAngle) * 500.0f
    );

    // --- 2. 移動制御（目標地点への追従） ---
    Vector2 moveVec = targetPos - pos;
    float distToTarget = moveVec.Length();
    if (distToTarget > 5.0f)
    {
        moveVec.Normalize();
        // 巨体ゆえの慣性を表現するため、急加速・急停止を避ける
        float targetSpeed = Math::Min(250.0f, distToTarget * 1.5f);
        mMoveComponent->SetForwardSpeed(targetSpeed);

        // 移動方向への旋回は行わず、移動ベクトルのみをMoveComponentに適用
        // ※MoveComponentが前方移動のみを想定している場合、内部的にmoveVec方向へ移動するよう調整
    }
    else
    {
        mMoveComponent->SetForwardSpeed(0.0f);
    }

    // --- 3. 旋回制御（常にプレイヤーを向く） ---
    // 巨体であることを表現するため、回転速度に制限（Clamp）をかける
    float targetAngle = Math::Atan2(toPlayer.y, toPlayer.x);
    float currentRot = GetRotation();
    float angleDiff = Math::WrapAngle(targetAngle - currentRot);

    // 旋回性能の限界（1秒間に約60度程度）を設定
    float maxRotationStep = Math::DegToRad(60.0f) * deltaTime;
    float rotationStep = Math::Clamp(angleDiff, -maxRotationStep, maxRotationStep);
    SetRotation(currentRot + rotationStep);

    // --- 4. 攻撃実行 ---
    // （フェーズ管理と攻撃ロジックは継続。常にプレイヤーを向いているため射撃方向も安定する）
}

void Boss_Heri::UpdateTracking(float deltaTime)
{
    Vector2 toPlayer = mPlayer->GetPosition() - GetPosition();
    float dist = toPlayer.Length();

    float targetAngle = Math::Atan2(toPlayer.y, toPlayer.x);
    SetRotation(Math::Lerp1(GetRotation(), targetAngle, 0.05f));

    Vector2 moveDir = Vector2::Zero;
    if (dist > IDEAL_DIST + 50.0f) moveDir = toPlayer;
    else if (dist < IDEAL_DIST - 50.0f) moveDir = toPlayer * -1.0f;

    if (moveDir.LengthSq() > 0.0f)
    {
        moveDir.Normalize();
        mMoveComponent->SetForwardSpeed(300.0f);
        SetPosition(GetPosition() + moveDir * 300.0f * deltaTime);
    }
    else
    {
        mMoveComponent->SetForwardSpeed(0.0f);
    }
}

void Boss_Heri::UpdateGunAttack(float deltaTime)
{
    mAttackTimer -= deltaTime;
    if (mAttackTimer <= 0.0f) {
        for (int i : { -1, 1 }) {
            Vector2 right(Math::Cos(GetRotation() + Math::Pi / 2.0f), Math::Sin(GetRotation() + Math::Pi / 2.0f));
            Vector2 muzzle = GetPosition() + right * (float)i * 35.0f;
            new EnemyBullet(GetGame(), muzzle, GetRotation(), 900.0f, 1.5f, 5);
        }
        mAttackTimer = 0.08f; // 高速掃射
    }
}
void Boss_Heri::UpdateMissileAttack(float deltaTime)
{
    UpdateTracking(deltaTime);

    if (mMissileStep < 3)
    {
        mAttackTimer -= deltaTime;
        if (mAttackTimer <= 0.0f)
        {
            FireMissileStep(mMissileStep);
            mMissileStep++;
            mAttackTimer = 0.3f;
        }
    }
}

void Boss_Heri::FireMissileStep(int step)
{
    float angles[] = { 15.0f, 45.0f, 75.0f };
    float angle = angles[step];

    for (float side : { -1.0f, 1.0f })
    {
        float launchAngle = GetRotation() + Math::DegToRad(angle * side);
        new EnemyMissile(GetGame(), GetPosition(), launchAngle, 500.0f, 4.0f, 15);
    }
}

void Boss_Heri::UpdateArtilleryAttack(float deltaTime)
{
    UpdateTracking(deltaTime);

    mAttackTimer -= deltaTime;
    if (mAttackTimer <= 0.0f && mArtilleryCount < 30)
    {
        float r = Random::GetFloatRange(0.0f, 250.0f);
        float theta = Random::GetFloatRange(0.0f, Math::TwoPi);
        Vector2 targetPos = mPlayer->GetPosition() + Vector2(r * Math::Cos(theta), r * Math::Sin(theta));

        new ArtilleryAttack(GetGame(), GetPosition(), targetPos);

        mArtilleryCount++;
        mAttackTimer = 0.08f;
    }
}