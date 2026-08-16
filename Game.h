#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "Math.h"
#include "CameraComponent.h"
#include "Player.h"
#include <SDL_mixer.h>
#include "SDL_ttf.h"
#include "HUDActor.h"
#include "HPComponent.h"
#include "HUDSpriteComponent.h"
#include "Scene.h"
#include "DataManager.h"
#include "BlinkingTextComponent.h"
#include "WeaponData.h"
#include "Enemy.h"
#include "LineComponent.h"
#include "MissionDataLoader.h"
#include "TargetIndicatorComponent.h"
#include "UIButton.h"
#include "EnemyFactory.h"

class Actor;
class Player;
class Enemy;
class CameraComponent;
class SpriteComponent;
class HUDActor;
class HUDSpriteComponent;
class HPComponent;
class Scene;
class BlinkingTextComponent;
class TargetActor;
struct UpgradeOption;

enum class GameState {
    Title,
    BaseMenu,
    MissionStart,
    InMission,
    Result,
    ReturnToBase
};

class Game {
public:
    Game();
    ~Game();
    bool Initialize();
    void RunLoop();
    void Shutdown();

    void LoadData(int missionID);
    void UnloadData();

    void GameInput();
    void GameUpdate();
    void GameGenerate();

    void UpdateMenu(float deltaTime);
    void DrawSpritesSimple(SDL_Renderer* renderer);

    void ChangeScene(std::unique_ptr<Scene> newScene);
    void RequestSceneChange(std::unique_ptr<Scene> newScene);

    void PlayBGM(const std::string& fileName, int loop = -1);
    void StopBGM();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);
    void AddEnemy(class Enemy* enemy);
    void RemoveEnemy(class Enemy* enemy);
    void AddTargetActor(class Actor* actor);
    void AddAlly(class AllyUnit* ally);
    void RemoveAlly(class AllyUnit* ally);
    void AddLineComponent(class LineComponent* line);
    void RemoveLineComponent(class LineComponent* line);

    void InstanceEnemy(const EnemyConfig& config, Enemy*& enemy, Vector2 pos);
    void AddImmediateDraw(std::function<void(SDL_Renderer*, const Vector2&)> func) {
        mImmediateDraws.emplace_back(func);
    }

    void UpdateObtainedWeapons();
    void AddObtainedWeapon(const std::string& weaponName);

    class Enemy* GetNearestEnemy(const Vector2& pos);
    class Actor* GetNearestTarget(const Vector2& pos);

    const std::vector<class Actor*>& GetTargetActors() const { return mTargetActors; }
    const std::vector<class Actor*>& GetActors() const { return mActors; }
    const std::vector<class Enemy*>& GetEnemies() const { return mEnemies; }
    const std::vector<class AllyUnit*>& GetAllies() const { return mAllies; }
    DataManager& GetDataManager() { return dataManager; }
    SDL_Renderer* GetRenderer() const { return mRenderer; }
    bool IsRunning() const { return mIsRunning; }
    const std::vector<std::string>& GetObtainedWeaponNames()const;
    CameraComponent* GetCamera() const { return mCamera; }
    void SetCamera(CameraComponent* camera) { mCamera = camera; }
    Player* GetPlayer()const { return mPlayer; }
    void SetPlayer(Player* player) { mPlayer = player; }
    void SetCurrentMissionID(int missionID) { mCurrentMissionID = missionID; }
    int GetLastMissionID() const { return mCurrentMissionID; }
    const MissionData& GetMissionData() const { return mMissionData; }
    const Vector2& GetCameraPos() const { return mCameraPos; }
    SDL_Texture* GetTexture(const std::string& fileName);
    std::vector<UpgradeOption> GetRandomUpgrades(int count);

    std::vector<std::function<void(SDL_Renderer*, const Vector2&)>> mImmediateDraws;
    std::vector<class Actor*> mDeadActors;

    WeaponData mPendingWeapon;
    bool mHasPendingWeapon = false;
    int enemyCount = 0;
    std::vector<std::string> mObtainedWeaponNames;
    std::vector<std::string> mObtainedWeapons;

    bool mMissionCompleted = false;
    Uint32 mMissionCompleteTime = 0;
    bool mSceneChangeRequested = false;
    std::unique_ptr<Scene> mNextScene;

    std::unique_ptr<class UIButton> menuButton;
    Vector2 mCameraPos;
    HUDActor* mHUDActor = nullptr;

private:
    void ResetGameSceneState();
    void UpdateActors(float deltaTime);
    void DestroyStoppedActors();
    void DestroyAllActors();

    std::vector<class LineComponent*> mLineComponents;
    std::vector<class CollisionComponent*> mCollisionComponents;
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mWaitingActors;
    std::vector<class SpriteComponent*> mSprites;
    std::vector<class Enemy*> mEnemies;
    std::vector<class AllyUnit*> mAllies;
    std::vector<class Actor*> mTargetActors;

    int mCurrentMissionID = -1;
    MissionData mMissionData;
    float mDefenseTimer = 0.0f;
    float mGameTimer = 0.0f;

    std::unique_ptr<Scene> mCurrentScene;
    std::unordered_map<std::string, SDL_Texture*> mTextures;
    Mix_Music* mCurrentBGM = nullptr;

    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    TTF_Font* mFont = nullptr;
    Uint32 mTicksCount = 0;
    bool mIsRunning = true;
    bool mUpdatingActors = false;
    class Player* mPlayer = nullptr;
    CameraComponent* mCamera = nullptr;

    HUDSpriteComponent* mHPBar = nullptr;
    HUDSpriteComponent* mHPBack = nullptr;
    HUDSpriteComponent* mStaminaBar = nullptr;
    HUDSpriteComponent* mStaminaBack = nullptr;
    HUDSpriteComponent* mLevelBar = nullptr;
    HUDSpriteComponent* mLevelBack = nullptr;
    TargetIndicatorComponent* mTargetIndicator = nullptr;
    HPComponent* mHPComponent = nullptr;

    DataManager dataManager;
    std::unique_ptr<EnemyFactory> mEnemyFactory;

    SDL_Surface* mMissionCompleteSurface = nullptr;
    SDL_Texture* mMissionCompleteTexture = nullptr;
    std::unique_ptr<BlinkingTextComponent> mMissionTextComponent;

    float mOceanTime = 0.0f;
    enum EFadeState
    {
        ENone,
        EFadeOut,
        EFadeIn
    };
    EFadeState mFadeState = ENone;
    float mFadeTimer = 0.0f;
    float mFadeDuration = 0.5f;

    float mMissionStartTime = 0.0f;
    int mEnemiesKilledCount = 0;
    int mCreditsEarned = 0;

    bool mIsPaused = false;
    bool mSDLInitialized = false;
    bool mImageInitialized = false;
    bool mMixerInitialized = false;
    bool mAudioInitialized = false;
    bool mTTFInitialized = false;
    bool mShutdown = false;
};
