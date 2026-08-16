#pragma once
#include "Component.h"
#include "SDL.h"
#include "Math.h"
#include <string>
#include <Math.h>

class SpriteComponent : public Component {
public:
    SpriteComponent(class Actor* owner, int drawOrder = 100 , std::string TexturePath = "", bool castShadow = false, Vector2 shadowOffset = Vector2(0.0f, 0.0f));
    ~SpriteComponent();

    virtual void Draw(SDL_Renderer* renderer, const Vector2& cameraPos);
    virtual void SetTexture(SDL_Texture* texture);

    void SetSourceRect(const SDL_Rect& rect) { mSrcRect = rect; mUseSrcRect = true; }
    void ClearSourceRect() { mUseSrcRect = false; }

    void SetDrawOrder(int amount) { mDrawOrder = amount; }
    int GetDrawOrder() const { return mDrawOrder; }
    int GetTexHeight() const { return mTexHeight; }
    int GetTexWidth() const { return mTexWidth; }
    Actor* GetOwner() const { return mOwner; }
    void SetSpriteUV(float u, float v, float uWidth, float vHeight);

    void SetCastShadow(bool cast) { mCastShadow = cast; }
    bool GetCastShadow() const { return mCastShadow; }

    void SetExplicitRotation(float rad) { mRotation = rad; mInheritRotation = false; }

    void SetAlpha(float alpha) { mAlpha = alpha; }
    float GetAlpha() const { return mAlpha; }

    void SetColor(float r, float g, float b);

    void FlashRed(float duration) { mFlashTimer = duration; }


    int mShadowWidth;
    int mShadowHeight;

protected:
    SDL_Texture* mTexture;
    int mDrawOrder;
    int mTexWidth;
    int mTexHeight;
    float mAlpha;
    Vector3 mColor;
    Vector2 mShadowOffset;

    float mRotation = 0.0f;
    bool mInheritRotation = true;

    SDL_Rect mSrcRect;
    bool mUseSrcRect = false;

    bool mCastShadow;

private:
    float mFlashTimer = 0.0f;
};
