#include "AllyTurret.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "BulletComponent.h"
#include "Game.h"

AllyTurret::AllyTurret(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position)
    : AllyUnit(game, mode, position)
{
    mSprite->SetTexture(game->GetTexture("Assets/AllyTurret.png"));
    mHP->SetMaxHP(300);
    mHP->Heal(300);
    mMove->SetMaxSpeed(0.0f);
    mAllyAI->SetUnitType(AllyAIComponent::ETurret);
    mBullet->SetFireInterval(0.05f);
    mBullet->SetDamage(3.0f);
}
