#include "LockOnComponent.h"
#include "Game.h"

LockOnEffect::LockOnEffect(Game* game, Actor* target)
    : Actor(game), mTarget(target)
{
    mSprite = new SpriteComponent(this , 250 , "Assets/LockOn.png" , false , Vector2{0,0});
}

void LockOnEffect::UpdateActor(float deltaTime)
{
    if (mTarget)
    {
        SetPosition(mTarget->GetPosition() + mOffset);
    }
}

Actor* LockOnEffect::GetTarget() const
{
    return mTarget;
}
