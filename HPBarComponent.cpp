#include "HPBarComponent.h"
#include "HPComponent.h"
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

class Game;

HPBarComponent::HPBarComponent(Actor* owner, HPComponent* hpComp)
    : Component(owner)
    , mHP(hpComp)
{
    if (owner && hpComp)
    {
        mHPBarActor = new Actor(owner->GetGame());
        owner->GetGame()->AddActor(mHPBarActor);

        mHPBarActor->SetPosition(owner->GetPosition() + Vector2(0.0f, -50.0f));
        mHPBarActor->SetRotation(0.0f);

        mBackgroundSprite = new SpriteComponent(mHPBarActor, 199 , "Assets/HPBarBack.png" , false,Vector2{0,0});

        mBarSprite = new SpriteComponent(mHPBarActor, 200 , "Assets/HPBar1.png" , false , Vector2{0,0});
    }
}

HPBarComponent::~HPBarComponent()
{
    if (mHPBarActor && mHPBarActor->GetGame())
    {
        mHPBarActor->SetState(Actor::EStop);
        mHPBarActor = nullptr; 
    }
}


void HPBarComponent::Update(float deltaTime)
{
    if (!mHP || !mBarSprite || !mBackgroundSprite || !mOwner) return;

    if (mOwner->GetState() == Actor::EStop)
    {
        if (mHPBarActor)
        {
            mHPBarActor->SetState(Actor::EStop);
            mHPBarActor = nullptr;
        }
        return;
    }

    float hpPercent = static_cast<float>(mHP->GetHP()) / mHP->GetMaxHP();
    int fullWidth = mBarSprite->GetTexWidth();
    int height = mBarSprite->GetTexHeight();
    int newWidth = static_cast<int>(fullWidth * hpPercent);

    SDL_Rect src{ 0, 0, newWidth, height };
    mBarSprite->SetSourceRect(src);

    Vector2 offset(0.0f, -50.0f);
    mHPBarActor->SetPosition(mOwner->GetPosition() + offset);
}


void HPBarComponent::DrawHPBar(SDL_Renderer* renderer)
{
    if (!mHP || !mBarSprite || !mBackgroundSprite) return;

    float hpPercent = static_cast<float>(mHP->GetHP()) / mHP->GetMaxHP();

    int fullWidth = mBarSprite->GetTexWidth();
    int height = mBarSprite->GetTexHeight();
    int newWidth = static_cast<int>(fullWidth * hpPercent);

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = newWidth;
    src.h = height;
    mBarSprite->SetSourceRect(src);

    Vector2 basePos = mOwner->GetPosition();
    Vector2 offset(0.0f, -50.0f);
    Vector2 barPos = basePos + offset;

    mBackgroundSprite->GetOwner()->SetPosition(barPos);
    mBackgroundSprite->GetOwner()->SetRotation(2.0f);

    mBarSprite->GetOwner()->SetPosition(barPos);
    mBarSprite->GetOwner()->SetRotation(0.0f);
}

