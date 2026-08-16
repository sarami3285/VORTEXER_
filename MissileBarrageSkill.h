#pragma once
#include "ActiveSkill.h"
#include "Missile.h"
#include "Game.h"
#include "Random.h"
#include "Enemy.h"
#include <vector>

class MissileBarrageSkill : public ActiveSkill
{
public:
	using ActiveSkill::ActiveSkill;

	void ExecuteSkill() override
	{
		int count = 18; 
		float searchRangeSq = 1200.0f * 1200.0f;

		std::vector<Enemy*> candidates;
		for (auto actor : mOwner->GetGame()->GetActors())
		{
			if (auto enemy = dynamic_cast<Enemy*>(actor))
			{
				if (enemy->GetState() == Actor::EAlive)
				{
					float distSq = (enemy->GetPosition() - mOwner->GetPosition()).LengthSq();
					if (distSq <= searchRangeSq)
					{
						candidates.push_back(enemy);
					}
				}
			}
		}

		for (int i = 0; i < count; ++i)
		{
			Missile* m = new Missile(mOwner->GetGame());
			m->SetPosition(mOwner->GetPosition());

			float angle = (static_cast<float>(i) / static_cast<float>(count)) * Math::TwoPi;
			angle += Random::GetFloatRange(-0.1f, 0.1f);

			m->SetRotation(angle);
			m->SetOwnerType(Bullet::EPlayer);

			if (!candidates.empty())
			{
				int randomIndex = Random::GetIntRange(0, static_cast<int>(candidates.size()) - 1);
				m->SetTarget(candidates[randomIndex]);
			}
		}
	}
};
