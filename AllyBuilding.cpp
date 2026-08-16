#include "AllyBuilding.h"
#include "Game.h"
#include "Actor.h"
#include "BulletComponent.h"

AllyBuilding::AllyBuilding(Game* game, const Vector2& position, int hp, const std::string& texture)
    : Actor(game)
{
    SetPosition(position);
    mHP = new HPComponent(this, hp);
    mHPBar = new HPBarComponent(this, mHP);
    mSprite = new SpriteComponent(this, 1, texture);
    mCollision = new CollisionComponent(this, 120.0f);

    game->AddTargetActor(this);

    if (mSprite && !texture.empty()) {
        mSprite->SetTexture(GetGame()->GetTexture(texture));
    }
}

void AllyBuilding::TakeDamage(int amount) {
    if (mHP) {
        mHP->TakeDamage(amount);
        if (mHP->IsDead()) {
            SetState(Actor::EStop);
        }
    }
}

void AllyBuilding::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);
}
