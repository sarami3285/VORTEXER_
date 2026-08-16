#include "PatrolComponent.h"
#include "MoveComponent.h"
#include "Random.h"
#include "Math.h"
#include "CameraComponent.h"
#include "Actor.h"

PatrolComponent::PatrolComponent(Actor* owner)
	: EnemyBehaviorComponent(owner)
	, mWaitTimer(0.0f)
	, mPatrolCenter(owner->GetPosition())
{
	Vector2 minPos = mPatrolCenter - Vector2(mPatrolRadius, mPatrolRadius);
	Vector2 maxPos = mPatrolCenter + Vector2(mPatrolRadius, mPatrolRadius);
	mTargetPos = GetRandomPositionInRange(minPos, maxPos);

	class MoveComponent* moveComp = mOwner->GetComponent<MoveComponent>();
	if (moveComp)
	{
		moveComp->SetForwardSpeed(mPatrolSpeed);
	}
}

void PatrolComponent::SetIsActive(bool active)
{
	EnemyBehaviorComponent::SetIsActive(active);

	class MoveComponent* moveComp = mOwner->GetComponent<MoveComponent>();

	if (!active)
	{
		if (moveComp)
		{
			moveComp->SetForwardSpeed(0.0f);
			moveComp->SetAngularSpeed(0.0f);
		}
	}
	else
	{
		if (moveComp)
		{
			moveComp->SetForwardSpeed(mPatrolSpeed);
		}
	}
}

void PatrolComponent::SetFormationTarget(class Actor* target, const Vector2& offset)
{
	mTargetFlagship = target;
	mFormationOffset = offset;

	if (mTargetFlagship)
	{
		mTargetPos = mTargetFlagship->GetPosition() + mFormationOffset;
		mWaitTimer = 0.0f;
	}
}

void PatrolComponent::Execute(float dt)
{
	if (!IsActive() || !mIsEnabled) return;

	class MoveComponent* moveComp = mOwner->GetComponent<MoveComponent>();
	if (!moveComp) return;

    if (mTargetFlagship)
    {
        mTargetPos = mTargetFlagship->GetPosition() + mFormationOffset;
    }

    Vector2 currentPos = mOwner->GetPosition();
    Vector2 toTarget = mTargetPos - currentPos;
    float distance = toTarget.Length();

    if (distance < mReachThreshold)
    {
        if (mTargetFlagship)
        {
            moveComp->SetAngularSpeed(0.0f);
            moveComp->SetForwardSpeed(0.0f);
        }
        else
        {
            if (mWaitTimer <= 0.0f)
            {
                moveComp->SetForwardSpeed(0.0f);
                moveComp->SetAngularSpeed(0.0f);
                mWaitTimer = mWaitDuration;
            }
            else
            {
                mWaitTimer -= dt;
                if (mWaitTimer <= 0.0f)
                {
                    Vector2 minPos = mPatrolCenter - Vector2(mPatrolRadius, mPatrolRadius);
                    Vector2 maxPos = mPatrolCenter + Vector2(mPatrolRadius, mPatrolRadius);
                    mTargetPos = GetRandomPositionInRange(minPos, maxPos);
                    moveComp->SetForwardSpeed(mPatrolSpeed);
                }
            }
        }
    }
    else
    {
        mWaitTimer = 0.0f;

        float targetAngle = atan2(toTarget.y, toTarget.x);
        float currentAngle = mOwner->GetRotation();
        float diff = Math::WrapAngle(targetAngle - currentAngle);
        float idealAngularSpeed = diff / dt;
        float rotationInput = Math::Clamp(idealAngularSpeed, -mPatrolTurnSpeed, mPatrolTurnSpeed);

        moveComp->SetAngularSpeed(rotationInput);
        moveComp->SetForwardSpeed(mPatrolSpeed);
    }
}

Vector2 PatrolComponent::GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos)
{
	float x = Random::GetFloatRange(minPos.x, maxPos.x);
	float y = Random::GetFloatRange(minPos.y, maxPos.y);
	x = Math::Clamp(x, 0.0f, CameraComponent::mMapWidth);
	y = Math::Clamp(y, 0.0f, CameraComponent::mMapHeight);

	return Vector2(x, y);
}
