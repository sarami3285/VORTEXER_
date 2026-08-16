#pragma once
#include "Math.h"
#include "Component.h"
 
class CollisionComponent : public Component {
public:
	CollisionComponent(class Actor* owner,float collisionSize);

	void SetRadius(float radius) { mRadius = radius; }
	float GetRadius() const;

	const Vector2& GetCenter() const;
private:
	float mRadius;
};

bool Intersect(const CollisionComponent& a, const CollisionComponent& b);
