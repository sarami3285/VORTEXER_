#include "DustEffect.h"
#include "SpriteComponent.h"
#include "Game.h"

DustEffect::DustEffect(Game* game, const Vector2& position, float rotation)
    : Actor(game)
    , mLifeTime(0.0f)
{
    SetPosition(position);
    SetRotation(rotation + Math::Pi);

    mSprite = new SpriteComponent(this, 50);
    mSprite->SetTexture(game->GetTexture("Assets/DustEffect.png"));

    mSprite->SetSpriteUV(0.0f, 0.0f, 64.0f, 64.0f);
}

void DustEffect::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);

    mLifeTime += deltaTime;

    const float totalDuration = 0.3f;
    const int numFrames = 4;
    const float frameWidth = 64.0f;
    const float frameHeight = 64.0f;

    // 進行度
    float progress = mLifeTime / totalDuration;
    int currentFrame = static_cast<int>(progress * numFrames);

    if (currentFrame < numFrames) {
        mSprite->SetSpriteUV(0, currentFrame * frameHeight, frameWidth, frameHeight);
    }
    else {
        // 【重要】テクスチャ剥奪ではなく「透明化」で対応
        // これならSDLのエラーも起きず、確実に画面から消えます
        mSprite->SetAlpha(0.0f);
        SetState(EStop);
    }
}