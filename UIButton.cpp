#include "UIButton.h"
#include <SDL_mixer.h> 

UIButton::UIButton(int x, int y, int w, int h, const std::string& labelText, SDL_Renderer* renderer, TTF_Font* font)
    : mRect{ x, y, w, h }
    , mLabel(labelText)
    , mRenderer(renderer)
    , mFont(font)
{
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = mFont ? TTF_RenderUTF8_Blended(mFont, mLabel.c_str(), white) : nullptr;
    if (surface) {
        mLabelTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
        SDL_FreeSurface(surface);
    }

    hoverSound = Mix_LoadWAV("Assets/Audio/MouseHover.wav");
    clickSound = Mix_LoadWAV("Assets/Audio/MouseClick.wav");
}

void UIButton::HandleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        int mx = e.motion.x;
        int my = e.motion.y;

        bool isInside = (mx >= mRect.x && mx <= mRect.x + mRect.w &&
            my >= mRect.y && my <= mRect.y + mRect.h);

        if (isInside && !mIsHovered) {
            mIsHovered = true;
            if (hoverSound) {
                Mix_PlayChannel(-1, hoverSound, 0);
            }
            mSoundPlayed = true; 
        }
        else if (!isInside && mIsHovered) {
            mIsHovered = false;
            mSoundPlayed = false;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= mRect.x && mx <= mRect.x + mRect.w &&
            my >= mRect.y && my <= mRect.y + mRect.h) {
            mClicked = true;
            if (clickSound) {
                Mix_PlayChannel(-1, clickSound, 0);
            }
        }
    }
}

bool UIButton::IsClicked() {
    bool wasClicked = mClicked;
    mClicked = false;
    return wasClicked;
}

void UIButton::Draw(SDL_Renderer* renderer) {
    SDL_Rect rect = mRect;
    if (mIsHovered) {
        rect.w = static_cast<int>(mRect.w * 1.2); 
        rect.h = static_cast<int>(mRect.h * 1.2); 
    }

    // ボタンの背景色
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &rect);

    // ボタンの枠
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    if (mLabelTexture) {
        int texW, texH;
        SDL_QueryTexture(mLabelTexture, nullptr, nullptr, &texW, &texH);
        SDL_Rect dst = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
        SDL_RenderCopy(renderer, mLabelTexture, nullptr, &dst);
    }
}

void UIButton::SetRect(int x, int y, int w, int h)
{
    mRect.x = x;
    mRect.y = y;
    mRect.w = w;
    mRect.h = h;
}

UIButton::~UIButton() {
    if (mLabelTexture) {
        SDL_DestroyTexture(mLabelTexture);
        mLabelTexture = nullptr;
    }
    if (hoverSound) {
        Mix_FreeChunk(hoverSound);
        hoverSound = nullptr;
    }
    if (clickSound) {
        Mix_FreeChunk(clickSound);
        clickSound = nullptr;
    }
}

void UIButton::SetText(const std::string& newText) {
    if (mLabel == newText) {
        return;
    }

    mLabel = newText;
    if (mLabelTexture) {
        SDL_DestroyTexture(mLabelTexture);
        mLabelTexture = nullptr;
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = mFont ? TTF_RenderUTF8_Blended(mFont, mLabel.c_str(), white) : nullptr;
    if (surface) {
        mLabelTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
        SDL_FreeSurface(surface);
    }
    else {
        SDL_Log("Failed to render text: %s", TTF_GetError());
    }
}
