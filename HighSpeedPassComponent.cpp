#include "HighSpeedPassComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "MoveComponent.h"
#include "LineComponent.h"
#include "RangeAttackComponent.h"
#include "Interceptor.h"
#include "CameraComponent.h"

HighSpeedPassComponent::HighSpeedPassComponent(class Actor* owner)
    : EnemyBehaviorComponent(owner)
    , mCurrentState(Recover)
    , mTimer(0.0f)
{
    mPlayer = owner->GetGame()->GetPlayer();
    mWarningLine = new LineComponent(owner);
    mWarningLine->SetColor(255, 0, 0);
    mWarningLine->SetVisibility(false);
}

void HighSpeedPassComponent::Execute(float deltaTime)
{
    if (!mPlayer || mOwner->GetState() != Actor::EAlive) return;

    auto mc = mOwner->GetComponent<MoveComponent>();
    if (!mc) return;

    Vector2 pos = mOwner->GetPosition();
    Vector2 vToPlayer = mPlayer->GetPosition() - pos;

    // 常にワープ判定を行う
    CheckMapBounds();

    switch (mCurrentState)
    {
    case Recover:
        // 振り向き時のみ低速で回転。向きが合ったら突撃準備
        mc->SetForwardSpeed(mMaxSpeed * 0.4f);
        {
            float targetAngle = Math::Atan2(vToPlayer.y, vToPlayer.x);
            float currentRot = mOwner->GetRotation();
            float angleDiff = Math::WrapAngle(targetAngle - currentRot);
            mOwner->SetRotation(currentRot + angleDiff * mTurnSpeed * deltaTime);

            // 向きが概ね合い、かつ一定距離あれば警告へ
            if (Math::Abs(angleDiff) < 0.1f && vToPlayer.Length() > mAttackRange * 0.5f) {
                mCurrentState = Warning;
                mTimer = mWarningTime;
                mFixedTargetPos = mPlayer->GetPosition();
            }
        }
        break;

    case Warning:
        // 完全に停止して狙いを定める（直進の予備動作）
        mc->SetForwardSpeed(0.0f);
        mWarningLine->SetVisibility(true);
        mWarningLine->SetTargetPos(mFixedTargetPos);

        mTimer -= deltaTime;
        if (mTimer <= 0.0f) {
            mCurrentState = Approach;
            mWarningLine->SetVisibility(false);
        }
        break;

    case Approach:
        // 設定されたターゲット位置に向かってフルスピードで直進
        mc->SetForwardSpeed(mMaxSpeed);
        {
            Interceptor* interceptor = dynamic_cast<Interceptor*>(mOwner);
            if (interceptor) interceptor->FireMachineGun();

            // プレイヤーを通り過ぎたら自動的にWithdraw（さらに加速して直進）へ
            if (vToPlayer.Length() < mPassDistance) {
                mCurrentState = Withdraw;
            }
        }
        break;

    case Withdraw:
        // 画面端に消えるまで最高速で「ただ直進」する
        mc->SetForwardSpeed(mMaxSpeed * 1.5f);
        break;
    }
}

void HighSpeedPassComponent::CheckMapBounds()
{
    Vector2 pos = mOwner->GetPosition();
    // ここはハードコード（例：1920x1080）でも良いですが、Cameraから取得
    float width = CameraComponent::mMapWidth;
    float height = CameraComponent::mMapHeight;

    // 画面の外に完全に出てから判定（マージンを大きく取る）
    float margin = 80.0f;

    bool teleported = false;
    Vector2 newPos = pos;

    // X軸のワープ（端を越えたら反対の端+マージンへ）
    if (pos.x < -margin) { newPos.x = width + margin - 10.0f; teleported = true; }
    else if (pos.x > width + margin) { newPos.x = -margin + 10.0f; teleported = true; }

    // Y軸のワープ
    if (pos.y < -margin) { newPos.y = height + margin - 10.0f; teleported = true; }
    else if (pos.y > height + margin) { newPos.y = -margin + 10.0f; teleported = true; }

    if (teleported)
    {
        mOwner->SetPosition(newPos);
        // ワープした瞬間、即座に次の獲物を探すフェーズへ
        mCurrentState = Recover;
    }
}

void HighSpeedPassComponent::FireDoubleMissile()
{
    auto rc = mOwner->GetComponent<RangedAttackComponent>();
    if (rc && !mHasFiredMissile) {
        float rot = mOwner->GetRotation();
        Vector2 forward(Math::Cos(rot), Math::Sin(rot));
        Vector2 right(-forward.y, forward.x);

        // 両翼から発射
        rc->FireBullet(forward);
        mHasFiredMissile = true;
    }
}
