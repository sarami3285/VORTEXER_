#include "AllyFighter.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "BulletComponent.h"
#include "Game.h"

AllyFighter::AllyFighter(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position)
    : AllyUnit(game, mode, position)
{
    mSprite->SetTexture(game->GetTexture("Assets/AllyInterceptor.png"));
    mMove->SetMaxSpeed(600.0f);
    mAllyAI->SetUnitType(AllyAIComponent::EFighter);
    mBullet->SetFireInterval(0.15f);
    mBullet->SetDamage(5.0f);
    mBullet->SetBulletSpeed(1200.0f);
}
