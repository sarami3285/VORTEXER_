#pragma once
#include "AllyUnit.h"
class AllyTurret : public AllyUnit {
public:
    AllyTurret(class Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position);
};
