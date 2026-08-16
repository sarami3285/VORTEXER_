#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class BlinkingTextComponent {
public:
    BlinkingTextComponent(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, float blinkInterval = 0.5f);
    ~BlinkingTextComponent();

    void Update(float deltaTime);
    void Draw() const;
    void SetText(const std::string& text, const SDL_Color& color);
    void SetBlinkingActive(bool active) { mIsActive = active; }
    void SetVisible(bool visible);

private:
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;
    SDL_Texture* mTextTexture = nullptr;

    int mPosX, mPosY;
    int mTextW = 0, mTextH = 0;
    float mBlinkTimer = 0.0f;
    float mBlinkInterval;
    bool mIsTextVisible = true;
    bool mIsActive = true;
};
