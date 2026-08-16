#include "BlinkingTextComponent.h"
#include <algorithm>
#include <cmath>

BlinkingTextComponent::BlinkingTextComponent(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, float blinkInterval)
    : mRenderer(renderer), mFont(font), mPosX(x), mPosY(y), mBlinkInterval(blinkInterval)
{
    SetText(text, { 255, 255, 255, 255 });
}

BlinkingTextComponent::~BlinkingTextComponent()
{
    if (mTextTexture) {
        SDL_DestroyTexture(mTextTexture);
    }
}

void BlinkingTextComponent::SetText(const std::string& text, const SDL_Color& color)
{
    if (mTextTexture) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }

    if (!mFont) return;

    SDL_Surface* surface = TTF_RenderUTF8_Blended(mFont, text.c_str(), color);

    if (surface) {
        mTextTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
        if (mTextTexture) {
            SDL_QueryTexture(mTextTexture, nullptr, nullptr, &mTextW, &mTextH);
        }
        SDL_FreeSurface(surface);
    }
    else {
        SDL_Log("ERROR: BlinkingTextComponentのテクスチャ生成に失敗: %s", TTF_GetError());
    }
}

void BlinkingTextComponent::Update(float deltaTime)
{
    if (!mIsActive) return;

    mBlinkTimer += deltaTime;

    if (mBlinkTimer >= mBlinkInterval) {
        mIsTextVisible = !mIsTextVisible;
        mBlinkTimer = std::fmod(mBlinkTimer, mBlinkInterval);
    }
}

void BlinkingTextComponent::Draw() const
{
    if (mTextTexture && mIsTextVisible) {
        int screenWidth = 1024;
        int centeredX = (screenWidth - mTextW) / 2;

        SDL_Rect dstRect = { centeredX, mPosY, mTextW, mTextH };
        SDL_RenderCopy(mRenderer, mTextTexture, nullptr, &dstRect);
    }
}

void BlinkingTextComponent::SetVisible(bool visible)
{
    mIsTextVisible = visible;
}
