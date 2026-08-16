#pragma once
#include "Enemy.h"
#include <vector>

class Boss_Heri : public Enemy
{
public:
    enum EPhase
    {
        Tracking,
        GunAttack,
        MissileAttack,
        Artillery
    };

    Boss_Heri(class Game* game);
    void UpdateActor(float deltaTime) override;

private:
    void UpdateTracking(float deltaTime);
    void UpdateGunAttack(float deltaTime);
    void UpdateMissileAttack(float deltaTime);
    void UpdateArtilleryAttack(float deltaTime);

    void FireMissileStep(int step);

    EPhase mCurrentPhase;
    float mPhaseTimer;
    float mAttackTimer;
    int mMissileStep;
    int mArtilleryCount;
    float mOrbitAngle = 0.0f;

    class Player* mPlayer;
    class MoveComponent* mMoveComponent;
    class SpriteComponent* mSprite;
    class CollisionComponent* mCollision;
    class CollisionComponent* GetCircle() const override { return mCollision; }

    const float IDEAL_DIST = 600.0f;
    const float PHASE_DURATION = 5.0f;
};