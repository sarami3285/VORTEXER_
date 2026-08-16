#pragma once
#include "Component.h"
#include "CameraComponent.h"

class MoveComponent : public Component {
public:
	MoveComponent(class Actor*owner, int updateOrder = 200, float Speed = 800.0f , float AngleSpeed = 1.0f);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	float GetRightSpeed() const { return mRightSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
	void SetRightSpeed(float speed) { mRightSpeed = speed; }
	void SetMaxSpeed(float maxSpeed) { mMaxSpeed = maxSpeed;  }
	float GetMaxSpeed() const { return mMaxSpeed; }
	void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }

	float GetMaxForwardSpeed() const { return mMaxForwardSpeed; }
	float GetMaxRightSpeed() const { return mMaxRightSpeed; }
	void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
	void SetMaxRightSpeed(float speed) { mMaxRightSpeed = speed; }

	Vector2 GetVelocity() const;

private:
	CameraComponent* mCameraComponent;
	float mAngularSpeed;
	float mForwardSpeed;
	float mRightSpeed;
protected:
	float mMaxForwardSpeed;
	float mMaxRightSpeed;
	float mMaxSpeed;
	float mMaxAngularSpeed;
};
