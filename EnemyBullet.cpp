#include "EnemyBullet.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "Game.h"
#include "CameraComponent.h" 
#include "Math.h" 
#include "AllyUnit.h"
#include "AllyBuilding.h"

EnemyBullet::EnemyBullet(class Game* game, const Vector2& startPos, float rotation, float speed, float lifeTime, int damage, const std::string& texturePath)
    : Actor(game)
    , mDeathTimer(lifeTime)
    , mCollision(nullptr)
    , mAudioComponent(nullptr)
    , Damage(damage)
    , mBulletPath(texturePath)
{
    SetPosition(startPos);
    SetRotation(rotation);

    SpriteComponent* sc = new SpriteComponent(this);
    sc->SetTexture(game->GetTexture(mBulletPath));
    MoveComponent* mc = new MoveComponent(this, 0, speed, 0.0f);

    mAudioComponent = new AudioComponent(this);
    mAudioComponent->LoadSE("shot", "Assets/Audio/shotgun.mp3");

    mCollision = new CollisionComponent(this , 11.0f);

    mAudioComponent->PlaySE("shot");
}

void EnemyBullet::UpdateActor(float deltaTime) {
    mDeathTimer -= deltaTime;
    if (mDeathTimer <= 0.0f) {
        SetState(EStop);
        return;
    }

    Vector2 pos = GetPosition();
    if (pos.x <= 0.0f || pos.x >= CameraComponent::mMapWidth ||
        pos.y <= 0.0f || pos.y >= CameraComponent::mMapHeight) {
        SetState(EStop);
        return;
    }

    // 1. プレイヤー判定
    Player* player = GetGame()->GetPlayer();
    if (player && player->GetState() == Actor::EAlive) {
        if (Intersect(*mCollision, *(player->GetCircle()))) {
            player->TakeDamage(Damage);
            SetState(EStop);
            return;
        }
    }

    // 2. ターゲット（味方・建物）判定
    // GetTargetActors() に AllyUnit や AllyBuilding が入っていることが前提
    for (auto target : GetGame()->GetTargetActors()) {
        if (!target || target->GetState() != Actor::EAlive) continue;

        // 型に依らず、CollisionComponent を持っているか直接確認
        auto targetCol = target->GetComponent<CollisionComponent>();
        if (targetCol && Intersect(*mCollision, *targetCol)) {
            // ダメージ処理を呼ぶ（TakeDamage が仮想関数、または各クラスで実装されている必要あり）
            // 共通の基底クラスがない場合は、無理やり cast せず個別に確認
            if (auto ally = dynamic_cast<AllyUnit*>(target)) {
                ally->TakeDamage(Damage);
            }
            else if (auto build = dynamic_cast<AllyBuilding*>(target)) {
                build->TakeDamage(Damage);
            }

            SetState(EStop);
            return;
        }
    }
}
