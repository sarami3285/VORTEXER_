#include "CollisionComponent.h"
#include "Actor.h"
#include "Game.h"

CollisionComponent::CollisionComponent(class Actor* owner, float collisionSize)
	:Component(owner)
	, mRadius(0.0f)
{
	SetRadius(collisionSize);
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

float CollisionComponent::GetRadius() const
{
	return mOwner->GetScale() * mRadius;
}

bool Intersect(const CollisionComponent& a, const CollisionComponent& b)
{
	Vector2 diff = a.GetCenter() - b.GetCenter();
	float distSq = diff.LengthSq();
	float sumRadii = a.GetRadius() + b.GetRadius();
	float radiiSq = sumRadii * sumRadii;

	return distSq <= radiiSq;
}
