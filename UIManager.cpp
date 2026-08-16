#include "UIManager.h"
#include <SDL_image.h>
// UIManager implementation is compiled from this source file only.
#include <SDL_ttf.h>

UIManager::UIManager()
    : mFont(nullptr)
    , mRenderer(nullptr)
    , mSelectedIndex(0)
    , mCurrentState(GameState::Title) {
}

UIManager::~UIManager() {
    Shutdown();
}

bool UIManager::Initialize(SDL_Renderer* renderer) {
    mRenderer = renderer;
    mFont = TTF_OpenFont("Assets/font.ttf", 24);
    if (!mFont) return false;

    mMenuItems = { "出撃", "カスタム", "ショップ", "セーブ", "終了" };
    return true;
}

void UIManager::Shutdown() {
    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }
}

void UIManager::SetState(GameState state) {
    mCurrentState = state;
    mSelectedIndex = 0;
}

void UIManager::Update(float deltaTime) {
    // 状態に応じたUIアニメなどを今後ここに追加
}

void UIManager::ProcessInput(const uint8_t* keyState) {
    if (mCurrentState == GameState::BaseMenu) {
        if (keyState[SDL_SCANCODE_DOWN]) mSelectedIndex++;
        if (keyState[SDL_SCANCODE_UP])   mSelectedIndex--;
        if (mSelectedIndex < 0) mSelectedIndex = 0;
        if (mSelectedIndex >= static_cast<int>(mMenuItems.size())) mSelectedIndex = static_cast<int>(mMenuItems.size()) - 1;
    }
}

void UIManager::Render(SDL_Renderer* renderer) {
    switch (mCurrentState) {
    case GameState::Title:        RenderTitle(); break;
    case GameState::BaseMenu:     RenderBaseMenu(); break;
    case GameState::MissionStart: RenderMissionStart(); break;
    case GameState::InMission:    RenderInMissionHUD(); break;
    case GameState::Result:       RenderResult(); break;
    default: break;
    }
}

// ↓↓↓ 各ステートの描画処理 ↓↓↓

void UIManager::RenderTitle() {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Blended(mFont, "2D ARMORED CORE", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_Rect dst = { 300, 100, surface->w, surface->h };
    SDL_RenderCopy(mRenderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    if ((SDL_GetTicks() / 500) % 2 == 0) {
        SDL_Surface* press = TTF_RenderText_Blended(mFont, "Press Enter", white);
        SDL_Texture* tPress = SDL_CreateTextureFromSurface(mRenderer, press);
        SDL_Rect dst2 = { 350, 400, press->w, press->h };
        SDL_RenderCopy(mRenderer, tPress, nullptr, &dst2);
        SDL_FreeSurface(press);
        SDL_DestroyTexture(tPress);
    }
}

void UIManager::RenderBaseMenu() {
    SDL_Color normal = { 200, 200, 200, 255 };
    SDL_Color selected = { 255, 255, 0, 255 };

    for (int i = 0; i < static_cast<int>(mMenuItems.size()); ++i) {
        SDL_Color color = (i == mSelectedIndex) ? selected : normal;
        SDL_Surface* surf = TTF_RenderText_Blended(mFont, mMenuItems[i].c_str(), color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
        SDL_Rect dst = { 300, 200 + i * 40, surf->w, surf->h };
        SDL_RenderCopy(mRenderer, tex, nullptr, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void UIManager::RenderMissionStart() {
    // ミッション名、ブリーフィング表示
}

void UIManager::RenderInMissionHUD() {
    // HPバー、ロックインジケータ、武装HUDなど
}

void UIManager::RenderResult() {
    // 結果、報酬、スコアなど
}
