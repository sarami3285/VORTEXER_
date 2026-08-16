#pragma once 
#include "RangeAttackComponent.h" 
#include "Math.h" 

class OrbitAttackComponent : public RangedAttackComponent
{
public:
    OrbitAttackComponent(class Actor* owner, class Player* player);
    void Execute(float dt) override;

    float mOrbitDistance = 400.0f;
    float mOrbitSpeed = Math::Pi / 2.0f;
    float mMaxChaseDistance = 1000.0f;
    float mChaseTurnSpeed;
};
