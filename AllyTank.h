#pragma once
#include "AllyUnit.h"
class AllyTank : public AllyUnit {
public:
    AllyTank(class Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position);
};
