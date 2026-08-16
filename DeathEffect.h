#pragma once
#include "Actor.h"

class DeathEffect : public Actor
{
public:
    DeathEffect(class Game* game, int explosionCount = 3);
    void UpdateActor(float deltaTime) override;

private:
    int mRemainingCount;
    float mSpawnTimer;
};

class ExplosionParticle : public Actor {
public:
    ExplosionParticle(class Game* game);
    void UpdateActor(float deltaTime) override;
private:
    class SpriteComponent* mSprite;
    float mFrameTimer;
    int mCurrentFrame;
};
