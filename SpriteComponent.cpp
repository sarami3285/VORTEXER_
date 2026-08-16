#include "SpriteComponent.h"
#include "Actor.h"
#include "SDL.h"
#include "Game.h"
#include "Math.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, std::string texturePath, bool castShadow, Vector2 shadowOffset)
    : Component(owner)
    , mTexture(nullptr)
    , mDrawOrder(drawOrder)
    , mTexWidth(0)
    , mTexHeight(0)
    , mSrcRect{ 0, 0, 0, 0 }
    , mShadowWidth(0)
    , mShadowHeight(0)
    , mAlpha(1.0f)
    , mCastShadow(castShadow) 
    , mShadowOffset(shadowOffset)
{
    mOwner->GetGame()->AddSprite(this);

    if (!texturePath.empty()) {
        SDL_Texture* tex = mOwner->GetGame()->GetTexture(texturePath);
        if (tex) {
            SetTexture(tex);
        }
    }
}

SpriteComponent::~SpriteComponent() {
    if (mOwner && mOwner->GetGame()) {
        mOwner->GetGame()->RemoveSprite(this);
    }
}

void SpriteComponent::SetTexture(SDL_Texture* texture) {
	mTexture = texture;
	if (mTexture) {
		SDL_QueryTexture(mTexture, nullptr, nullptr, &mTexWidth, &mTexHeight);
	}
	else {
		mTexWidth = 0;
		mTexHeight = 0;
	}
}

void SpriteComponent::SetSpriteUV(float u, float v, float uWidth, float vHeight)
{
    mUseSrcRect = true;
    mSrcRect.x = static_cast<int>(u);
    mSrcRect.y = static_cast<int>(v);
    mSrcRect.w = static_cast<int>(uWidth);
    mSrcRect.h = static_cast<int>(vHeight);
}

void SpriteComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    if (this->mTexture)
    {
        SDL_Rect src;
        if (this->mUseSrcRect)
        {
            src = this->mSrcRect;
        }
        else
        {
            src = { 0, 0, this->mTexWidth, this->mTexHeight };
        }

        int w = static_cast<int>(src.w * mOwner->GetScale());
        int h = static_cast<int>(src.h * mOwner->GetScale());
        Vector2 pos = mOwner->GetPosition() - cameraPos;

        SDL_Rect dst;
        dst.w = w;
        dst.h = h;
        dst.x = static_cast<int>(pos.x - w / 2);
        dst.y = static_cast<int>(pos.y - h / 2);

        double angle = mInheritRotation ? Math::ToDegrees(mOwner->GetRotation()) : Math::ToDegrees(mRotation);

        if (mCastShadow) {
            SDL_SetTextureColorMod(mTexture, 0, 0, 0);
            SDL_SetTextureAlphaMod(mTexture, 100);

            SDL_Rect shadowDst = dst;
            shadowDst.x += static_cast<int>(mShadowOffset.x);
            shadowDst.y += static_cast<int>(mShadowOffset.y);

            SDL_RenderCopyEx(renderer, mTexture, &src, &shadowDst, angle, nullptr, SDL_FLIP_NONE);
            SDL_SetTextureColorMod(mTexture, 255, 255, 255);
        }

        if (mFlashTimer > 0.0f) {
            SDL_SetTextureColorMod(mTexture, 255, 0, 0);
        }
        else if (mColor.x > 0.0f || mColor.y > 0.0f || mColor.z > 0.0f) {
            SDL_SetTextureColorMod(mTexture,
                static_cast<Uint8>(mColor.x * 255.0f),
                static_cast<Uint8>(mColor.y * 255.0f),
                static_cast<Uint8>(mColor.z * 255.0f));
        }
        else {
            SDL_SetTextureColorMod(mTexture, 255, 255, 255);
        }

        SDL_SetTextureAlphaMod(mTexture, static_cast<Uint8>(mAlpha * 255));
        SDL_RenderCopyEx(renderer, mTexture, &src, &dst, angle, nullptr, SDL_FLIP_NONE);
        SDL_SetTextureColorMod(mTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(mTexture, 255);

        if (mFlashTimer > 0.0f) {
            mFlashTimer -= 0.016f;
        }
    }
}

void SpriteComponent::SetColor(float r, float g, float b)
{
    Uint8 red = static_cast<Uint8>(r * 255.0f);
    Uint8 green = static_cast<Uint8>(g * 255.0f);
    Uint8 blue = static_cast<Uint8>(b * 255.0f);

    mColor = Vector3(r, g, b);
}
