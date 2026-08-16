// AllyDrone.h
#include "AllyDrone.h"

AllyDrone::AllyDrone(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position)
    : AllyUnit(game, mode, position) {
    mAllyAI->SetUnitType(AllyAIComponent::EDrone);
}
