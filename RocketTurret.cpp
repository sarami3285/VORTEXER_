#include "RocketTurret.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Player.h"
#include "RocketAttack.h"
#include "Random.h"
#include "CollisionComponent.h"
#include "HPComponent.h"
#include "HPBarComponent.h"

RocketTurret::RocketTurret(Game* game)
    : Enemy(game)
    , mAttackCooldown(5.0f)
    , mAttackTimer(2.0f)
    , mShotsRemaining(0)
    , mShotIntervalTimer(0.0f)
{
    mTexturePath = "Assets/RocketTurret.png";
    SpriteComponent* sc = new SpriteComponent(this, 1, mTexturePath);
    mSprite = sc;

    mHPComponent = new HPComponent(this, 150);
    new HPBarComponent(this, mHPComponent);
    mCollision = new CollisionComponent(this, 40.0f);
}

void RocketTurret::UpdateActor(float dt) {
    Enemy::UpdateActor(dt);

    Player* p = GetGame()->GetPlayer();
    if (!p) return;
    Vector2 diff = p->GetPosition() - GetPosition();

    // 攻撃ロジック
    if (mShotsRemaining > 0) {
        // 連射中
        mShotIntervalTimer -= dt;
        if (mShotIntervalTimer <= 0.0f) {
            FireRocket();
            mShotsRemaining--;
            mShotIntervalTimer = 0.3f; // 0.3秒間隔で連射
        }
    }
    else {
        // 待機中
        mAttackTimer -= dt;
        if (mAttackTimer <= 0.0f) {
            // 距離の２乗！ 例: 5000^2 = 25,000,000
            if (diff.LengthSq() < 4000000.0f && diff.LengthSq() > 250000) {
                mShotsRemaining = 3; // 3連射セット
                mShotIntervalTimer = 0.0f;
                mAttackTimer = mAttackCooldown;
            }
        }
    }
}

void RocketTurret::FireRocket() {
    Player* p = GetGame()->GetPlayer();
    if (!p) return;

    Vector2 playerPos = p->GetPosition();
    Vector2 playerForward = p->GetForward();
    float predictionFactor = 1.5f; // 1.5秒先の未来を狙う
    Vector2 targetPos = playerPos + (playerForward * 100.0f * predictionFactor);

    // バラつき
    float scatter = 150.0f;
    targetPos.x += Random::GetFloatRange(-scatter, scatter);
    targetPos.y += Random::GetFloatRange(-scatter, scatter);



    new RocketAttack(GetGame(), GetPosition(), targetPos);
}
