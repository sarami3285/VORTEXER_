#pragma once
#include "SpriteComponent.h"
#include "Actor.h"
#include "SDL.h"
#include "Math.h"

class HUDSpriteComponent : public SpriteComponent {
public:
    HUDSpriteComponent(Actor* owner, int drawOrder = 200)
        : SpriteComponent(owner, drawOrder)
        , mOffset(0.0f, 0.0f)
        , mHPPercent(1.0f)
    {
    }

    void SetOffset(const Vector2& offset) { mOffset = offset; }

    void SetTargetActor(Actor* target)
    {
        mTargetActor = target;
    }

    void SetHPPercent(float percent)
    {
        mHPPercent = Math::Clamp(percent, 0.0f, 1.0f);
    }

    void Draw(SDL_Renderer* renderer, const Vector2& /*cameraPos*/) override
    {
        if (!mTexture || mHPPercent <= 0.0f)
            return;

        SDL_Rect fullSrc = mUseSrcRect ? mSrcRect : SDL_Rect{ 0, 0, mTexWidth, mTexHeight };
        int hpWidth = static_cast<int>(fullSrc.w * mHPPercent);

        SDL_Rect src = fullSrc;
        src.w = hpWidth;

        int w = static_cast<int>(hpWidth * mOwner->GetScale());
        int h = static_cast<int>(fullSrc.h * mOwner->GetScale());

        Vector2 pos = mOwner->GetPosition() + mOffset;

        SDL_Rect dst;
        dst.w = w;
        dst.h = h;
        dst.x = static_cast<int>(pos.x - (fullSrc.w * mOwner->GetScale() / 2)); // 左端固定
        dst.y = static_cast<int>(pos.y - h / 2);

        SDL_RenderCopy(renderer, mTexture, &src, &dst);
    }
protected:
    Vector2 mOffset;

private:
    float mHPPercent; // 0.0～1.0
    class Actor* mTargetActor = nullptr;
};
