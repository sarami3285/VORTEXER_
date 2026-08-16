#pragma once
#include "Actor.h"
#include "SpriteComponent.h"

class LockOnEffect : public Actor
{
public:
    LockOnEffect(Game* game, Actor* target);
    void UpdateActor(float deltaTime) override;
    void SetTarget(Actor* target) { mTarget = target; }
    Actor* GetTarget() const;
private:
    Actor* mTarget; 
    SpriteComponent* mSprite;
    Vector2 mOffset = Vector2(0.0f, -0.0f);
    float mDeltaTime;
};
