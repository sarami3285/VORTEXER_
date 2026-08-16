#pragma once
#include "EnemyBullet.h"
#include "Math.h"

class Player;

class EnemyMissile : public EnemyBullet
{
public:
    EnemyMissile(class Game* game, const Vector2& startPos, float rotation, float speed, float lifeTime, int damage);

    virtual void UpdateActor(float deltaTime) override;
    void Explode();

private:
    class Player* mTarget;
    float mTurnSpeed = Math::Pi;
    const float mExplosionRadius = 50.0f;

    float mAnimTimer = 0.0f;
    const float mAnimFPS = 0.0f; 
    int mCurrentFrame = 0;
    const int mNumFrames = 3; 
    const int mFrameWidth = 88; 
};
