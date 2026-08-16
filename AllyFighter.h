#pragma once
#include "AllyUnit.h"
class AllyFighter : public AllyUnit {
public:
    AllyFighter(class Game* game, AllyAIComponent::EAllyMode mode, const Vector2& position);
};
