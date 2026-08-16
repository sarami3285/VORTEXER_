#pragma once
#include <SDL.h>
#include "SDL_ttf.h"
#include <string>
#include <SDL_mixer.h> 

class UIButton {
public:
    UIButton(int x, int y, int w, int h, const std::string& labelText, SDL_Renderer* renderer, TTF_Font* font);
    ~UIButton();
    UIButton(const UIButton&) = delete;
    UIButton& operator=(const UIButton&) = delete;

    void HandleEvent(const SDL_Event& e);
    void Draw(SDL_Renderer* renderer);
    bool IsClicked();
    void SetRect(int x, int y, int w, int h);
    void SetText(const std::string& newText);
    void ResetClick() { mClicked = false; }

    bool IsHovered() const { return mIsHovered; }

private:
    SDL_Rect mRect;
    std::string mLabel;
    SDL_Texture* mLabelTexture = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    TTF_Font* mFont = nullptr;
    bool mClicked = false;

    bool mIsHovered = false;        // ホバー判定用フラグ
    bool mSoundPlayed = false;      // 効果音再生判定
    Mix_Chunk* hoverSound = nullptr;  // 効果音
    Mix_Chunk* clickSound = nullptr;  // 効果音
};
