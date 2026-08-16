#pragma once
#include "Actor.h"
#include <vector>

class ArtilleryAttack : public Actor {
public:
    ArtilleryAttack(class Game* game, const Vector2& startPos, const Vector2& targetPos);
    void UpdateActor(float dt) override;
    void OnImpact(); // 弾が着弾した時の処理

private:
    Vector2 mStartPos;
    Vector2 mTargetPos;
    class SpriteComponent* mMarkerSprite;
    float mLifeTime;
    float mMaxPreAttackTime; // 予告時間
    bool mIsExploding;
    float mExplosionTimer;
    int mExplosionFrame;
};
