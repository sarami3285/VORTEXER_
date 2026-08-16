#pragma once
#include "ActiveSkill.h"
#include "Game.h"
#include "AllyDrone.h"
#include "Math.h"

class DroneDeploySkill : public ActiveSkill
{
public:
    using ActiveSkill::ActiveSkill;

    void ExecuteSkill() override
    {
        if (!mOwner) return;

        float radius = 50.0f;
        Game* game = mOwner->GetGame();
        Vector2 ownerPos = mOwner->GetPosition();

        for (int i = 0; i < 3; ++i)
        {
            float angle = (Math::TwoPi / 3.0f) * i;
            Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
            AllyDrone* ally = new AllyDrone(game, AllyAIComponent::EFollow, ownerPos + offset);

            if (auto ai = ally->GetComponent<AllyAIComponent>())
            {
                ai->SetOffset(offset);
            }
        }
    }
};
