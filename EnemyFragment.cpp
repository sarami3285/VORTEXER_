#include "EnemyFragment.h"
#include "Game.h"
#include "Math.h"
#include "Random.h"
#include <SDL.h> 

const int FRAGMENT_SIZE = 30;

EnemyFragment::EnemyFragment(class Game* game, const Vector2& startPos, const Vector2& initialVelocity)
    : Actor(game)
    , mLifetime(0.3f + Random::GetFloat() * 0.3f)
{
    SetPosition(startPos);

    mSprite = new SpriteComponent(this, 3 , "Assets/EnemyFragment.png" , false, Vector2{0,0});

    int randFrame = Random::GetIntRange(0, 3);
    int texX = (randFrame % 2) * FRAGMENT_SIZE;
    int texY = (randFrame / 2) * FRAGMENT_SIZE;

    mSprite->SetSourceRect({ texX, texY, FRAGMENT_SIZE, FRAGMENT_SIZE });

    mMove = new MoveComponent(this, 0, initialVelocity.Length() + 500.0f, 0.0f);

    if (initialVelocity.LengthSq() > 0.0f)
    {
        float angle = Math::Atan2(-initialVelocity.y, -initialVelocity.x);
        SetRotation(angle);
    }
}

void EnemyFragment::UpdateActor(float deltaTime)
{
    mLifetime -= deltaTime;
    if (mLifetime <= 0.0f)
    {
        SetState(State::EStop);
    }
    Actor::UpdateActor(deltaTime);
}
