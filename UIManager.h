#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Game.h"

class UIManager {
public:
    UIManager();
    ~UIManager();

    bool Initialize(SDL_Renderer* renderer);
    void Shutdown();

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    GameState GetCurrentState() const { return mCurrentState; }
    void ProcessInput(const uint8_t* keyState);
    void SetState(GameState state);

private:
    GameState mCurrentState;
    std::vector<std::string> mMenuItems;
    int mSelectedIndex;

    TTF_Font* mFont;
    SDL_Renderer* mRenderer;

    void RenderTitle();
    void RenderBaseMenu();
    void RenderMissionStart();
    void RenderInMissionHUD();
    void RenderResult();
};
