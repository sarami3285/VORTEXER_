#pragma once
#include "AllyUnit.h"
class AllyDrone : public AllyUnit {
public:
    AllyDrone(Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position);
};
