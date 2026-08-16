#pragma once
#include "Actor.h"

class RocketAttack : public Actor {
public:
    RocketAttack(class Game* game, const Vector2& startPos, const Vector2& targetPos);
    void UpdateActor(float dt) override;
    void OnMissileImpact();

private:
    Vector2 mStartPos;
    Vector2 mTargetPos;

    class SpriteComponent* mCircleSprite;
    class Actor* mMissileActor;
    class CollisionComponent* mCollision;

    bool mIsExploding;
    float mExplosionTimer;
    int mExplosionFrame;

    float mTime;
    float mDuration;
};
