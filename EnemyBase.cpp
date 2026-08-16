#include "EnemyBase.h"
#include "HPComponent.h"
#include "HPBarComponent.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h" 
#include "LowLevelEnemy.h"
#include "Debris.h"

EnemyBase::EnemyBase(Game* game) :Enemy(game){
    IsUnit = false;

    const float SENTRY_SEARCH_RANGE = 1200.0f;

    mHPComponent = new HPComponent(this, 300);
    new HPBarComponent(this, mHPComponent);
    mCollision = new CollisionComponent(this, 70.0f);

    mTexturePath = "Assets/EnemyBase.png";
    SpriteComponent* sc = new SpriteComponent(this, 1, mTexturePath, true, Vector2{ 5,5 });
    mSprite = sc;

    NUM_ExpFRAGMENTS = 5;
}

EnemyBase::~EnemyBase() {
    Debris* d = new Debris(GetGame(), "Assets/EnemyBase_Destroy.png");
    d->SetPosition(this->GetPosition());
}

void EnemyBase::UpdateActor(float dt) {
    mSpawnTimer -= dt;
    if (mSpawnTimer <= 0.0f) {
        mSpawnTimer = mSpawnInterval;

        LowLevelEnemy* spawnedEnemy = new LowLevelEnemy(GetGame());
        spawnedEnemy->SetPosition(GetPosition());

        if (auto state = spawnedEnemy->GetComponent<EnemyStateComponent>()) {
            state->SetIsHunter(true);
        }
    }
}

void EnemyBase::TakeDamage(float amount, const Vector2& bulletDir)
{
    Enemy::TakeDamage(amount, bulletDir);
}
