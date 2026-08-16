#include "AllyTank.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "BulletComponent.h"
#include "Game.h"

AllyTank::AllyTank(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position)
    : AllyUnit(game, mode, position)
{
    mSprite->SetTexture(game->GetTexture("Assets/AllyTank.png"));
    mHP->SetMaxHP(100);
    mHP->Heal(100);
    mMove->SetMaxSpeed(50.0f);
    mAllyAI->SetUnitType(AllyAIComponent::ETank);
    mBullet->SetFireInterval(0.4f);
    mBullet->SetDamage(10.0f);
}
