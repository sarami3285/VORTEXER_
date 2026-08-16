#pragma once
#include "EnemyBehaviorComponent.h"
#include "Math.h"
#include "Actor.h" 

class PatrolComponent : public EnemyBehaviorComponent
{
public:
    PatrolComponent(class Actor* owner);

    void Execute(float dt) override;
    void SetIsActive(bool active) override;

    void SetEnabled(bool enabled) { mIsEnabled = enabled; }
    bool IsEnabled() const { return mIsEnabled; }

    float mPatrolRadius = 200.0f;
    float mPatrolSpeed = 50.0f;
    float mPatrolTurnSpeed = 3.0f;

    void SetFormationTarget(class Actor* target, const Vector2& offset);

private:
    bool mIsEnabled = true;
    float mWaitTimer;

    Vector2 mPatrolCenter;
    Vector2 mTargetPos;

    float mWaitDuration = 5.0f;
    float mReachThreshold = 10.0f;

    Vector2 GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos);

    class Actor* mTargetFlagship = nullptr; 
    Vector2 mFormationOffset = Vector2::Zero;
};
