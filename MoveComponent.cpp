#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"
#include "CameraComponent.h"
#include "Game.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder, float Speed, float AngleSpeed)
	:Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(0.0f)
	, mRightSpeed(0.0f)
	, mMaxForwardSpeed(0.0f)
	, mMaxRightSpeed(0.0f)
{
	SetForwardSpeed(Speed);
	SetMaxSpeed(Speed);
	SetMaxAngularSpeed(Math::TwoPi / 360.0f * AngleSpeed);
}

Vector2 MoveComponent::GetVelocity() const
{
	Vector2 forwardVec = mOwner->GetForward() * mForwardSpeed;
	Vector2 rightVec = mOwner->GetRight() * mRightSpeed;
	return forwardVec + rightVec;
}

void MoveComponent::Update(float deltaTime) {
	if (mMaxAngularSpeed > 0.0f) {
		mAngularSpeed = Math::Clamp(mAngularSpeed, -mMaxAngularSpeed, mMaxAngularSpeed);
	}

	if (!Math::NearZero(mAngularSpeed)) {
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mRightSpeed))
	{
		Vector2 pos = mOwner->GetPosition();

		if (!Math::NearZero(mForwardSpeed))
		{
			pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		}

		if (!Math::NearZero(mRightSpeed))
		{
			pos += mOwner->GetRight() * mRightSpeed * deltaTime;
		}

		if (pos.x < 0.0f) { pos.x = 0.0f; }
		else if (pos.x > CameraComponent::mMapWidth) { pos.x = CameraComponent::mMapWidth; }

		if (pos.y < 0.0f) { pos.y = 0.0f; }
		else if (pos.y > CameraComponent::mMapHeight) { pos.y = CameraComponent::mMapHeight; }

		mOwner->SetPosition(pos);
	}
}
