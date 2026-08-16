#pragma once
#include "Component.h"
#include "EnemyBehaviorComponent.h"
#include "Math.h"

class HighSpeedPassComponent : public EnemyBehaviorComponent
{
public:
    enum EAIState { Warning, Approach, Withdraw, Recover };

    HighSpeedPassComponent(class Actor* owner);
    void Execute(float dt) override;

    float mMaxSpeed = 800.0f;
    float mTurnSpeed = 5.0f;
    float mAttackRange = 800.0f;
    float mPassDistance = 150.0f;
    float mWarningTime = 0.2f;

private:
    void CheckMapBounds();
    void FireDoubleMissile();

    Vector2 mFixedTargetPos;
    EAIState mCurrentState;
    float mTimer;
    class Player* mPlayer;
    class LineComponent* mWarningLine;
    bool mHasFiredMissile = false;
};
