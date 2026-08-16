#include "Scenes.h"
#include "Game.h"
#include <SDL.h>
#include "ScrollItemComponent.h"
#include "json.hpp"
#include <sstream>
#include "SortieEffect.h"
#include "WhiteFadeIn.h"

// =============== TitleScene 実装 ===============
TitleScene::TitleScene(Game* game, SDL_Renderer* renderer)
    : mGame(game), mRenderer(renderer) {

}

TitleScene::~TitleScene() {
    mBlinkingText.reset();
    if (StartSound) {
        Mix_FreeChunk(StartSound);
        StartSound = nullptr;
    }
    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }
}

void TitleScene::LoadContent()
{
    StartSound = Mix_LoadWAV("Assets/Audio/StartSound.wav");
    mTextureBack = mGame->GetTexture("Assets/TitleBack.png");
    mTextureTitle = mGame->GetTexture("Assets/Title.png");
    if (!mTextureBack || !mTextureTitle) {
        SDL_Log("Failed to load title image");
    }
    mFont = TTF_OpenFont("Assets/NSJP.ttf", 48);
    if (!mFont) { SDL_Log("Failed to load font: %s", TTF_GetError()); }
    if (mFont) {
        mBlinkingText = std::make_unique<BlinkingTextComponent>(
            mRenderer,
            mFont,
            "Press Space to Start",
            0,
            600,
            0.7f
        );
    }
}

void TitleScene::ProcessInput(const Uint8* keyState) {
    int mx, my;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);

    // スペースキー または マウス左クリック
    if (keyState[SDL_SCANCODE_SPACE] || (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))) {
        if (!mIsFadingOut) {
            if (StartSound) {
                Mix_PlayChannel(-1, StartSound, 0);
            }
        }
        mIsFadingOut = true;
    }
}

void TitleScene::Update(float deltaTime) {

    if (mIsFadingOut) {
        mFadeAlpha = static_cast<Uint8>(std::min(255.0f, mFadeAlpha + 255.0f * deltaTime));
        if (mBlinkingText) {
            mBlinkingText->SetBlinkingActive(false);
            mBlinkingText->SetVisible(true);
        }
    }
    else {
        if (mBlinkingText) {
            mBlinkingText->Update(deltaTime);
        }
    }
}

void TitleScene::Draw(SDL_Renderer* renderer) {
    if (mTextureBack) {
        SDL_Rect destRect = { 0, 0, 1024, 768 };
        SDL_RenderCopy(renderer, mTextureBack, nullptr, &destRect);
    }
    if (mTextureTitle) {
        SDL_Rect destRect = { 212, 0, 630, 420 };
        SDL_RenderCopy(renderer, mTextureTitle, nullptr, &destRect);
    }

    if (mBlinkingText) {
        mBlinkingText->Draw();
    }

    if (mIsFadingOut) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, mFadeAlpha);
        SDL_Rect fadeRect = { 0, 0, 1024, 768 };
        SDL_RenderFillRect(renderer, &fadeRect);
    }
}

bool TitleScene::IsFinished() const {
    return mIsFadingOut && mFadeAlpha >= 255;
}

std::unique_ptr<Scene> TitleScene::NextScene() {
    return std::make_unique<MenuScene>(mGame, mGame->GetRenderer());
}
// ==============================================





// =============== MenuScene 実装 ===============
MenuScene::MenuScene(Game* game, SDL_Renderer* renderer)
    : mGame(game)
    , mRenderer(renderer)
{
}


MenuScene::~MenuScene(){
    UnloadContent();
}

void MenuScene::LoadContent() {
    new WhiteFadeIn(mGame, false);
    mGame->PlayBGM("Assets/Audio/techno_03.mp3", -1);
    mTexture = mGame->GetTexture("Assets/Base.png");
    if (!mTexture) {
        SDL_Log("Failed to load title image");
    }

    mFont = TTF_OpenFont("Assets/NSJP.ttf", 40);
    if (!mFont) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }

    std::string weaponName = mGame->mPendingWeapon.name;
    WeaponData data;
    data.LoadFromJSON("Assets/Data/WeaponData.json", weaponName);
    mWeaponTexture = mGame->GetTexture(data.weaponIcon);
    mBackTexture = mGame->GetTexture("Assets/BackGround.png");



    startButton = std::make_unique<UIButton>(700, 450, 200, 60, "Battle", mRenderer, mFont);
    storeButton = std::make_unique<UIButton>(100, 450, 200, 60, "Store", mRenderer, mFont);
    customizeButton = std::make_unique<UIButton>(350, 450, 200, 60, "Customize", mRenderer, mFont);



    int currency = mGame->GetDataManager().GetCurrency();

    SDL_Color white = { 255, 255, 255, 255 };
    std::string text = "CREDITS: " + std::to_string(currency); 

    SDL_Surface* surface = TTF_RenderUTF8_Blended(mFont, text.c_str(), white);
    if (!surface) {
        SDL_Log("Failed to create currency surface: %s", TTF_GetError());
    }
    else {
        mCurrencyText = SDL_CreateTextureFromSurface(mRenderer, surface);
        mCurrencyRect = { 50, 50, surface->w, surface->h };
        SDL_FreeSurface(surface);
    }
}

void MenuScene::UnloadContent() {
    startButton.reset();
    storeButton.reset();
    customizeButton.reset();

    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }

    if (mCurrencyText) {
        SDL_DestroyTexture(mCurrencyText);
        mCurrencyText = nullptr;
    }
}

void MenuScene::ProcessEvent(const SDL_Event& e) {
    if (startButton) startButton->HandleEvent(e);
    if (storeButton) storeButton->HandleEvent(e);
    if (customizeButton) customizeButton->HandleEvent(e);
}


void MenuScene::ProcessInput(const Uint8* keyState) {
    if (startButton && startButton->IsClicked()) {
        startButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<MissionScene>(mGame, mRenderer));
            }, false);
    }
    if (storeButton && storeButton->IsClicked()) {
        storeButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<ShopScene>(mGame, mRenderer));
            }, false);
    }
    if (customizeButton && customizeButton->IsClicked()) {
        customizeButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<CustomizeScene>(mGame, mRenderer));
            }, false);
    }
}

void MenuScene::Update(float deltaTime) {
    mGame->UpdateMenu(deltaTime);
}

void MenuScene::Draw(SDL_Renderer* renderer) {
    if (mTexture) {
        SDL_Rect destRect = { 0, 0, 1024, 768 };
        SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
    }

    if (mBackTexture) {
        SDL_Rect destRect = { 0, 620, 276, 148 };
        SDL_RenderCopy(renderer, mBackTexture, nullptr, &destRect);
    }

    if (mWeaponTexture) {
        SDL_Rect destRect = { 10, 630, 256, 128 };
        SDL_RenderCopy(renderer, mWeaponTexture, nullptr, &destRect);
    }

    // ボタンは1回だけ描画
    if (startButton) startButton->Draw(renderer);
    if (storeButton) storeButton->Draw(renderer);
    if (customizeButton) customizeButton->Draw(renderer);

    if (mCurrencyText) {
        SDL_RenderCopy(renderer, mCurrencyText, nullptr, &mCurrencyRect);
    }
    mGame->DrawSpritesSimple(renderer);
}

// ==============================================




// =============== CustomizeScene 実装 ===============
CustomizeScene::CustomizeScene(Game* game, SDL_Renderer* renderer)
    : mGame(game)
    , mRenderer(renderer) {
}

CustomizeScene::~CustomizeScene() {
    UnloadContent();
}

std::string GetIconPath(const std::string& weaponName)
{
    WeaponData data;
    if (data.LoadFromJSON("Assets/Data/WeaponData.json", weaponName)) {
        return data.weaponIcon;
    }
    else {
        SDL_Log("Icon path could not be loaded for %s", weaponName.c_str());
        return "Assets/WeaponDefault.png";
    }
}

void CustomizeScene::LoadContent() {
    new WhiteFadeIn(mGame, false);
    mGame->PlayBGM("Assets/Audio/techno_02.mp3", -1);
    mTexture = mGame->GetTexture("Assets/Base.png");
    if (!mTexture) {
        SDL_Log("Failed to load title image");
    }

    mFont = TTF_OpenFont("Assets/NSJP.ttf", 32);
    if (!mFont) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }

    quitButton = std::make_unique<UIButton>(800, 700, 200, 60, "Quit", mRenderer, mFont);

    mHUDActor = new HUDActor(mGame);
    mHUDActor->SetPosition(Vector2(0.0f, 0.0f));

    mScrollList = new ScrollItemComponent(mHUDActor, mRenderer, mFont);
    mScrollList->SetItemSize(128, 128);
    mScrollList->SetViewport({ 150, 100, 700, 600 });

    if (mGame->mHasPendingWeapon) {
        mCurrentWeaponName = mGame->mPendingWeapon.name;
    }

    std::ifstream file("Assets/Data/WeaponData.json");
    if (file.is_open()) {
        json j;
        file >> j;
        file.close();

        if (j.contains("weapons")) {
            for (auto& [weaponName, weaponInfo] : j["weapons"].items()) {
                if (weaponInfo.value("GetWeapon", false)) {
                    SDL_Texture* tex = mGame->GetTexture(weaponInfo.value("weaponIcon", "Assets/Bullet.png"));
                    std::string description = weaponInfo.value("description", "説明文がありません");

                    mScrollList->AddItem(
                        tex,
                        weaponName,
                        [this, weaponName]() {
                            WeaponData data;
                            if (data.LoadFromJSON("Assets/Data/WeaponData.json", weaponName)) {
                                mGame->mPendingWeapon = data;
                                mGame->mHasPendingWeapon = true;

                                if (!mCurrentWeaponName.empty()) {
                                    mScrollList->UpdateButtonText(mCurrentWeaponName, "Equip");
                                }

                                mCurrentWeaponName = weaponName;
                                mScrollList->UpdateButtonText(mCurrentWeaponName, "Equipped");
                            }
                        },
                        "Equip",
                        description 
                    );
                }
            }
        }
    }

    if (!mCurrentWeaponName.empty()) {
        mScrollList->UpdateButtonText(mCurrentWeaponName, "Equipped");
    }
}

void CustomizeScene::UnloadContent() {
    quitButton.reset();
    if (mHUDActor) {
        mGame->RemoveActor(mHUDActor);
        mHUDActor = nullptr;
        mScrollList = nullptr;
    }

    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }

    if (mTexture) {
        mTexture = nullptr;
    }
}

void CustomizeScene::ProcessEvent(const SDL_Event& e) {
    if (quitButton) quitButton->HandleEvent(e);
    if (mScrollList) mScrollList->ProcessEvent(e);
}


void CustomizeScene::ProcessInput(const Uint8* keyState) {
    if (quitButton && quitButton->IsClicked()) {
        quitButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<MenuScene>(mGame, mRenderer));
            }, false);
    }
}

void CustomizeScene::Update(float deltaTime) {
    mGame->UpdateMenu(deltaTime);
}

void CustomizeScene::Draw(SDL_Renderer* renderer) {
    if (mTexture) {
        SDL_Rect destRect = { 0, 0, 1024, 768 };
        SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
        if (quitButton) {
            quitButton->Draw(renderer);
        }
    }

    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect scrollBG = { 150, 100, 700, 600 };
    SDL_RenderFillRect(renderer, &scrollBG);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    if (mScrollList) {
        mScrollList->Draw(renderer, Vector2(0, 0));
    }

    mGame->DrawSpritesSimple(renderer);
}
// ==============================================



// ============== = ShopScene 実装 ============== =
using json = nlohmann::json;

ShopScene::ShopScene(Game* game, SDL_Renderer* renderer)
    : mGame(game)
    , mRenderer(renderer) {
}

ShopScene::~ShopScene() {
    UnloadContent();
}

void ShopScene::LoadContent() {
    new WhiteFadeIn(mGame, false);
    mGame->PlayBGM("Assets/Audio/techno_02.mp3", -1);
    mTexture = mGame->GetTexture("Assets/Shop.png");
    mSoldOutTexture = mGame->GetTexture("Assets/SOLD OUT.png");
    BuySound = Mix_LoadWAV("Assets/Audio/BuySound.wav");
    mFont = TTF_OpenFont("Assets/NSJP.ttf", 32);
    quitButton = std::make_unique<UIButton>(800, 700, 200, 60, "Quit", mRenderer, mFont);
    mHUDActor = new HUDActor(mGame);
    mHUDActor->SetPosition(Vector2(0.0f, 0.0f));
    mScrollList = new ScrollItemComponent(mHUDActor, mRenderer, mFont);
    mScrollList->SetItemSize(128, 128); 
    mScrollList->SetViewport({ 150, 100, 700, 600 });
    Refresh();
}

void ShopScene::UnloadContent() {
    quitButton.reset();
    if (mHUDActor) {
        mGame->RemoveActor(mHUDActor);
        mHUDActor = nullptr;
        mScrollList = nullptr;
    }

    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }
    if (mCurrencyText) {
        SDL_DestroyTexture(mCurrencyText);
        mCurrencyText = nullptr;
    }
    if (BuySound) {
        Mix_FreeChunk(BuySound);
        BuySound = nullptr;
    }
    mTexture = nullptr;
}

void ShopScene::Refresh() {
    if (mCurrencyText) {
        SDL_DestroyTexture(mCurrencyText);
        mCurrencyText = nullptr;
    }
    int currency = mGame->GetDataManager().GetCurrency();
    std::string text = "CREDITS: " + std::to_string(currency);
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderUTF8_Blended(mFont, text.c_str(), white);
    if (surface) {
        mCurrencyText = SDL_CreateTextureFromSurface(mRenderer, surface);
        mCurrencyRect = { 50, 50, surface->w, surface->h };
        SDL_FreeSurface(surface);
    }
    bool allWeaponsSoldOut = true;
    if (mScrollList) {
        mScrollList->ClearItems();
        std::ifstream file("Assets/Data/WeaponData.json");
        if (file.is_open()) {
            json j;
            file >> j;
            file.close();
            if (j.contains("weapons")) {
                for (auto& [weaponName, weaponInfo] : j["weapons"].items()) {
                    bool isWeaponObtained = weaponInfo.value("GetWeapon", false);
                    if (!isWeaponObtained) {
                        allWeaponsSoldOut = false;

                        int price = weaponInfo.value("price", 0);
                        std::string iconPath = weaponInfo.value("weaponIcon", "Assets/Bullet.png");
                        SDL_Texture* tex = mGame->GetTexture(iconPath);
                        std::string description = weaponInfo.value("description", "説明文がありません");

                        mScrollList->AddItem(
                            tex,
                            weaponName,
                            [this, weaponName, price]() { 
                                if (mGame->GetDataManager().GetCurrency() >= price) {
                                    if (this->mGame->GetDataManager().UseCurrency(price)) {
                                        this->mGame->AddObtainedWeapon(weaponName);
                                        if (BuySound) Mix_PlayChannel(-1, BuySound, 0);
                                        this->Refresh();
                                    }
                                }
                            },
                            "Buy",
                            price,
                            description
                        );

                    }
                }
            }
        }
        mScrollList->SortItemsByPrice();
    }
    mAllWeaponsSoldOut = allWeaponsSoldOut;
}

void ShopScene::SelectWeapon(const std::string& weaponName, SDL_Texture* texture, const std::string& description) {
    WeaponData data;
    if (!data.LoadFromJSON("Assets/Data/WeaponData.json", weaponName)) {
        return;
    }
    int price = data.price;
    mScrollList->AddItem(
        texture,
        weaponName,
        [this, weaponName]() {
            WeaponData data;
            if (data.LoadFromJSON("Assets/Data/WeaponData.json", weaponName)) {
                if (this->mGame->GetDataManager().UseCurrency(data.price)) {
                    Mix_PlayChannel(-1, this->BuySound, 0);
                    this->mGame->AddObtainedWeapon(weaponName);
                    this->Refresh();
                }
            }
        },
        "Buy",
        price,
        description
    );
}

void ShopScene::ProcessEvent(const SDL_Event& e) {
    if (quitButton) quitButton->HandleEvent(e);
    if (mScrollList) {
        mScrollList->ProcessEvent(e);
    }
}

void ShopScene::ProcessInput(const Uint8* keyState) {
    if (quitButton && quitButton->IsClicked()) {
        quitButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<MenuScene>(mGame, mRenderer));
            }, false);
    }
}

void ShopScene::Update(float deltaTime) {
    mGame->UpdateMenu(deltaTime);
}

void ShopScene::Draw(SDL_Renderer* renderer) {
    if (mTexture) {
        SDL_Rect destRect = { 0, 0, 1024, 768 };
        SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
    }
    if (quitButton) {
        quitButton->Draw(renderer);
    }
    if (mAllWeaponsSoldOut) {
        if (mSoldOutTexture) {
            int texW = 0, texH = 0;
            SDL_QueryTexture(mSoldOutTexture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dstRect = { (1024 - texW) / 2, (768 - texH) / 2, texW, texH };
            SDL_RenderCopy(renderer, mSoldOutTexture, nullptr, &dstRect);
        }
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect scrollBG = { 150, 100, 700, 600 };
        SDL_RenderFillRect(renderer, &scrollBG);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        if (mScrollList) {
            mScrollList->Draw(renderer, Vector2(0, 0));
        }
    }
    if (mCurrencyText) {
        SDL_RenderCopy(renderer, mCurrencyText, nullptr, &mCurrencyRect);
    }

    mGame->DrawSpritesSimple(renderer);
}

// ==============================================


// =============== MissionScene 実装 ===============
MissionScene::MissionScene(Game* game, SDL_Renderer* renderer)
    : mGame(game), mRenderer(renderer), mSelectedID(-1) {
}

MissionScene::~MissionScene() {
    mDepartButton.reset();
    quitButton.reset();
    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }
}

void MissionScene::ProcessEvent(const SDL_Event& e) {
    if (quitButton) quitButton->HandleEvent(e);
    if (mSelectedID != -1 && mDepartButton) {
        mDepartButton->HandleEvent(e);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePos = { mx, my };

        bool hitMission = false;
        for (const auto& mission : mMissions) {
            if (SDL_PointInRect(&mousePos, &mission.rect)) {
                mSelectedID = mission.id;
                hitMission = true;
                break;
            }
        }

        if (!hitMission) {
            SDL_Rect panelRect = { 700, 380, 310, 370 };
            if (mSelectedID != -1 && !SDL_PointInRect(&mousePos, &panelRect)) {
                mSelectedID = -1;
            }
        }
    }
}

void MissionScene::ProcessInput(const Uint8* keyState) {
    if (mSelectedID != -1 && mDepartButton && mDepartButton->IsClicked()) {
        bool canDepart = false;
        for (const auto& m : mMissions) {
            if (m.id == mSelectedID && (m.isUnlocked || m.isCleared)) {
                canDepart = true;
                break;
            }
        }

        if (canDepart) {
            mDepartButton = nullptr;
            new SortieEffect(mGame, [this]() {
                auto nextScene = std::make_unique<GameScene>(mGame, mRenderer, mSelectedID);
                mGame->ChangeScene(std::move(nextScene));
                }, true);
        }
    }
    if (quitButton && quitButton->IsClicked()) {
        quitButton = nullptr;
        new SortieEffect(mGame, [this]() {
            mGame->ChangeScene(std::make_unique<MenuScene>(mGame, mRenderer));
            }, false);
    }
}

void MissionScene::LoadContent() {
    new WhiteFadeIn(mGame, false);
    mMapTexture = mGame->GetTexture("Assets/Mission_map.png");
    mIconSheet = mGame->GetTexture("Assets/Mission_banner.png");
    mFont = TTF_OpenFont("Assets/NSJP.ttf", 24);

    const std::vector<int>& clearedIDs = mGame->GetDataManager().GetClearedMissions();

    std::ifstream file("Assets/Data/MissionData.json");
    if (file.is_open()) {
        nlohmann::json data;
        file >> data;
        file.close();

        for (const auto& m : data["missions"]) {
            MissionNode node;
            node.id = m["id"];
            node.title = m["name"];
            node.description = m["description"];

            int x = 0;
            int y = 0;
            if (m.contains("map_x")) {
                x = m["map_x"].get<int>();
            }
            else {
                SDL_Log("Mission ID %d: map_x not found!", node.id);
            }

            if (m.contains("map_y")) {
                y = m["map_y"].get<int>();
            }
            node.rect = { x, y, 100, 100 };

            node.iconIndex = 0;
            std::string type = m.value("target_type", "");
            if (type == "Elimination") node.iconIndex = 0;
            else if (type == "Destruction") node.iconIndex = 1;
            else if (type == "Defense") node.iconIndex = 2;

            if (m.contains("prev_mission_ids")) {
                for (int pid : m["prev_mission_ids"]) {
                    node.prevMissionIDs.push_back(pid);
                }
            }

            auto it = std::find(clearedIDs.begin(), clearedIDs.end(), node.id);
            node.isCleared = (it != clearedIDs.end());

            if (node.prevMissionIDs.empty()) {
                node.isUnlocked = true;
            }
            else {
                bool allCleared = true;
                for (int pid : node.prevMissionIDs) {
                    if (std::find(clearedIDs.begin(), clearedIDs.end(), pid) == clearedIDs.end()) {
                        allCleared = false;
                        break;
                    }
                }
                node.isUnlocked = allCleared;
            }

            node.lineAnimationProgress = node.isCleared ? 1.0f : 0.0f;
            mMissions.push_back(node);
        }
    }
    else {
        SDL_Log("Failed to open MissionData.json");
    }

    mDepartButton = std::make_unique<UIButton>(800, 600, 150, 50, "出撃", mRenderer, mFont);
    quitButton = std::make_unique<UIButton>(800, 700, 200, 60, "Quit", mRenderer, mFont);
}

void MissionScene::Update(float deltaTime) {
    for (auto& m : mMissions) {
        if (m.isUnlocked && m.lineAnimationProgress < 1.0f) {
            m.lineAnimationProgress += deltaTime * 1.5f;
            if (m.lineAnimationProgress > 1.0f) m.lineAnimationProgress = 1.0f;
        }
    }

    mGame->UpdateMenu(deltaTime);
}

void MissionScene::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, mMapTexture, nullptr, nullptr);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 180);
    for (const auto& mission : mMissions) {
        if (!mission.isUnlocked && !mission.isCleared) continue;

        for (int pid : mission.prevMissionIDs) {
            auto it = std::find_if(mMissions.begin(), mMissions.end(), [pid](const MissionNode& n) {
                return n.id == pid;
                });

            if (it != mMissions.end()) {
                float startX = it->rect.x + it->rect.w / 2.0f;
                float startY = it->rect.y + it->rect.h / 2.0f;
                float targetX = mission.rect.x + mission.rect.w / 2.0f;
                float targetY = mission.rect.y + mission.rect.h / 2.0f;

                float t = mission.isCleared ? 1.0f : mission.lineAnimationProgress;
                int endX = static_cast<int>(startX + (targetX - startX) * t);
                int endY = static_cast<int>(startY + (targetY - startY) * t);

                SDL_RenderDrawLine(renderer, (int)startX, (int)startY, endX, endY);
            }
        }
    }

    for (const auto& mission : mMissions) {
        SDL_Rect srcRect = { mission.iconIndex * 256, 0, 256, 256 };

        if (!mission.isUnlocked && !mission.isCleared) {
            SDL_SetTextureColorMod(mIconSheet, 100, 100, 100);
        }
        else {
            SDL_SetTextureColorMod(mIconSheet, 255, 255, 255);
        }

        SDL_RenderCopy(renderer, mIconSheet, &srcRect, &mission.rect);
        SDL_SetTextureColorMod(mIconSheet, 255, 255, 255);

        if (mSelectedID == mission.id) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &mission.rect);
        }
    }

    if (quitButton) quitButton->Draw(renderer);

    if (mSelectedID != -1) {
        const MissionNode* selectedMission = nullptr;
        for (const auto& m : mMissions) {
            if (m.id == mSelectedID) {
                selectedMission = &m;
                break;
            }
        }

        if (selectedMission) {
            SDL_Rect panelRect = { 700, 380, 310, 370 };
            SDL_SetRenderDrawColor(renderer, 10, 10, 20, 230);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &panelRect);

            SDL_Color white = { 255, 255, 255, 255 };

            SDL_Surface* nameSurf = TTF_RenderUTF8_Blended(mFont, selectedMission->title.c_str(), white);
            if (nameSurf) {
                SDL_Texture* nameTex = SDL_CreateTextureFromSurface(renderer, nameSurf);
                SDL_Rect dRect = { panelRect.x + 15, panelRect.y + 15, nameSurf->w, nameSurf->h };
                SDL_RenderCopy(renderer, nameTex, nullptr, &dRect);
                SDL_FreeSurface(nameSurf);
                SDL_DestroyTexture(nameTex);
            }

            SDL_Surface* descSurf = TTF_RenderUTF8_Blended_Wrapped(mFont, selectedMission->description.c_str(), white, panelRect.w - 30);
            if (descSurf) {
                SDL_Texture* descTex = SDL_CreateTextureFromSurface(renderer, descSurf);
                SDL_Rect dRect = { panelRect.x + 15, panelRect.y + 70, descSurf->w, descSurf->h };
                SDL_RenderCopy(renderer, descTex, nullptr, &dRect);
                SDL_FreeSurface(descSurf);
                SDL_DestroyTexture(descTex);
            }

            if (mDepartButton && (selectedMission->isUnlocked || selectedMission->isCleared)) {
                mDepartButton->Draw(renderer);
            }
        }
    }

    mGame->DrawSpritesSimple(renderer);
}
// ==============================================

// =============== GameScene 実装 ===============
GameScene::GameScene(Game* game, SDL_Renderer* renderer, int missionID)
    : mGame(game)
    , mMissionID(missionID)
{
}

void GameScene::LoadContent() {
    new WhiteFadeIn(mGame, true);
    mGame->PlayBGM("Assets/Audio/bgm.mp3", -1);
}

void GameScene::LoadData() {
    mGame->LoadData(mMissionID);
}

void GameScene::ProcessInput(const Uint8* keyState) {
    mGame->GameInput();
}

void GameScene::Update(float deltaTime) {
    mGame->GameUpdate();
}

void GameScene::Draw(SDL_Renderer* renderer) {
    mGame->GameGenerate();
}
// ==============================================


// ============== ResultScene 実装 ==============
ResultScene::ResultScene(Game* game, SDL_Renderer* renderer, bool isWin, int earnedCurrency)
    : mGame(game)
    , mRenderer(renderer)
    , mIsWin(isWin)
    , mEarnedCurrency(earnedCurrency)
    , mFont(nullptr)
    , mResultTextTexture(nullptr)
    , mCurrencyTextTexture(nullptr)
{
}

ResultScene::~ResultScene() {
    UnloadContent();
}

void ResultScene::LoadContent() {
    mGame->PlayBGM("Assets/Audio/techno_03.mp3", -1);

    if (mIsWin) {
        mBackTexture = mGame->GetTexture("Assets/GameClear.png");
    }
    else {
        mBackTexture = mGame->GetTexture("Assets/GameOver.png");
    }

    mFont = TTF_OpenFont("Assets/NSJP.ttf", 40);
    if (!mFont) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }

    int missionID = mGame->GetLastMissionID();
    MissionData missionData;

    if (MissionDataLoader::LoadMission(missionID, missionData))
    {
        if (mIsWin)
        {
            int baseCredits = missionData.Rewards.BaseCredits;
            float difficultyMultiplier = missionData.Rewards.BonusMultiplier;
            mEarnedCurrency = (int)(baseCredits * difficultyMultiplier);
        }
        else
        {
            mEarnedCurrency = 0;
        }
    }
    else
    {
        SDL_Log("ERROR: Mission Data Load Failed for reward calculation. ID: %d", missionID);
        mEarnedCurrency = 0;
    }

    mGame->GetDataManager().AddCurrency(mEarnedCurrency);

    if (mIsWin) {
        mGame->GetDataManager().AddClearedMission(missionID, "Assets/Data/SaveData.json", mGame->mPendingWeapon.name);
    }

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color red = { 255, 0, 0, 255 };
    SDL_Color green = { 0, 255, 0, 255 };

    std::string resultStr = mIsWin ? "MISSION SUCCESS" : "MISSION FAILED";
    SDL_Color resultColor = mIsWin ? green : red;

    SDL_Surface* resultSurface = TTF_RenderUTF8_Blended(mFont, resultStr.c_str(), resultColor);
    if (!resultSurface) {
        SDL_Log("Failed to create result surface: %s", TTF_GetError());
    }
    else {
        mResultTextTexture = SDL_CreateTextureFromSurface(mRenderer, resultSurface);
        mResultTextRect = { (1024 - resultSurface->w) / 2, 100, resultSurface->w, resultSurface->h };
        SDL_FreeSurface(resultSurface);
    }

    std::string currencyText = "CREDITS EARNED: " + std::to_string(mEarnedCurrency);

    SDL_Surface* currencySurface = TTF_RenderUTF8_Blended(mFont, currencyText.c_str(), white);
    if (!currencySurface) {
        SDL_Log("Failed to create currency surface: %s", TTF_GetError());
    }
    else {
        mCurrencyTextTexture = SDL_CreateTextureFromSurface(mRenderer, currencySurface);
        mCurrencyTextRect = { (1024 - currencySurface->w) / 2, 200, currencySurface->w, currencySurface->h };
        SDL_FreeSurface(currencySurface);
    }

    menuButton = std::make_unique<UIButton>(412, 550, 300, 90, "Back to Menu", mRenderer, mFont);
}

void ResultScene::UnloadContent() {
    menuButton.reset();

    if (mFont) {
        TTF_CloseFont(mFont);
        mFont = nullptr;
    }

    if (mResultTextTexture) {
        SDL_DestroyTexture(mResultTextTexture);
        mResultTextTexture = nullptr;
    }

    if (mCurrencyTextTexture) {
        SDL_DestroyTexture(mCurrencyTextTexture);
        mCurrencyTextTexture = nullptr;
    }
}

void ResultScene::ProcessEvent(const SDL_Event& e) {
    if (menuButton) menuButton->HandleEvent(e);
}


void ResultScene::ProcessInput(const Uint8* keyState) {
    if (menuButton && menuButton->IsClicked()) {
        mGame->ChangeScene(std::make_unique<MenuScene>(mGame, mRenderer));
        return;
    }
}

void ResultScene::Update(float deltaTime) {

}

void ResultScene::Draw(SDL_Renderer* renderer) {
    if (mBackTexture) {
        SDL_Rect destRect = { 0, 0, 1024, 768 };
        SDL_RenderCopy(renderer, mBackTexture, nullptr, &destRect);
    }

    if (mResultTextTexture) {
        SDL_RenderCopy(renderer, mResultTextTexture, nullptr, &mResultTextRect);
    }

    if (mCurrencyTextTexture) {
        SDL_RenderCopy(renderer, mCurrencyTextTexture, nullptr, &mCurrencyTextRect);
    }

    if (menuButton) menuButton->Draw(renderer);
    mGame->DrawSpritesSimple(renderer);
}
// ==============================================
