#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <random>
#include "vector"
#include "memory"
#include "algorithm"
#include "Actor.h"
#include "SpriteComponent.h"
#include "LowLevelEnemy.h"
#include "Random.h"
#include "Player.h"
#include "TileMapComponent.h"
#include "CameraComponent.h"
#include "Scenes.h"
#include "HUDSpriteComponent.h"
#include "DroneGuard.h"
#include "SentryGun.h"
#include "AllyAIComponent.h" 
#include "AllyUnit.h"
#include "MissionDataLoader.h"
#include "LineComponent.h"
#include "Enemy.h"
#include "TargetActor.h"
#include "BattleShip.h"
#include "PatrolComponent.h"
#include "RangeAttackComponent.h"
#include "AllyBuilding.h"
#include "EnemyBase.h"
#include "Interceptor.h"
#include "AllyTank.h"
#include "RocketTurret.h"
#include "AllyTurret.h"
#include "Boss_Tank.h"
#include "Boss_Heri.h"

#include <cfloat>
Game::Game()
	: mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
	, mIsPaused(false)
{
	mEnemyFactory = std::make_unique<EnemyFactory>(this);
}

Game::~Game() {
	Shutdown();
}

bool Game::Initialize() {
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("初期化エラー：SDLが初期化できませんでした _ %s", SDL_GetError());
		return false;
	}
	mSDLInitialized = true;

	const int imageFlags = IMG_INIT_PNG;
	if ((IMG_Init(imageFlags) & imageFlags) != imageFlags) {
		SDL_Log("初期化エラー：画像システムの初期化ができませんでした _ %s", IMG_GetError());
		return false;
	}
	mImageInitialized = true;

	const int mixerFlags = MIX_INIT_MP3;
	if ((Mix_Init(mixerFlags) & mixerFlags) != mixerFlags) {
		SDL_Log("初期化エラー：MP3デコーダーの初期化ができませんでした _ %s", Mix_GetError());
		return false;
	}
	mMixerInitialized = true;

	if (TTF_Init() != 0) {
		SDL_Log("初期化エラー：フォントシステムの初期化ができませんでした _ %s", TTF_GetError());
		return false;
	}
	mTTFInitialized = true;

	mWindow = SDL_CreateWindow("VORTEXER", 100, 100, 1024, 768, SDL_WINDOW_ALLOW_HIGHDPI);
	if (!mWindow) {
		SDL_Log("初期化エラー：ウィンドウが作成できませんでした _ %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		SDL_Log("初期化エラー：レンダラーが作成できませんでした _ %s", SDL_GetError());
		return false;
	}
	SDL_RenderSetLogicalSize(mRenderer, 1024, 768);

	mFont = TTF_OpenFont("Assets/NSJP.ttf", 32);
	if (!mFont) {
		SDL_Log("初期化エラー：共通フォントのロードに失敗しました _ %s", TTF_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("初期化エラー：音声システムの初期化ができませんでした _ %s", Mix_GetError());
		return false;
	}
	mAudioInitialized = true;

	if (!dataManager.ValidateGameData()) {
		SDL_Log("初期化エラー：JSONファイルの読み込みエラー、あるいは記述ミスがある可能性");
		return false;
	}

	std::string equippedWeaponName;
	if (dataManager.LoadGameData("Assets/Data/SaveData.json", equippedWeaponName)) {
		if (!equippedWeaponName.empty()) {
			WeaponData weapon;
			if (weapon.LoadFromJSON("Assets/Data/WeaponData.json", equippedWeaponName)) {
				mPendingWeapon = weapon;
				mHasPendingWeapon = true;
			}
		}
	}

	Random::Init();
	mTicksCount = SDL_GetTicks();
	ChangeScene(std::make_unique<TitleScene>(this, mRenderer));

	return true;
}

void Game::RunLoop() {
	Uint32 lastTicks = SDL_GetTicks();

	while (mIsRunning) {
		Uint32 currentTicks = SDL_GetTicks();
		float deltaTime = (currentTicks - lastTicks) / 1000.0f;
		if (deltaTime > 0.05f) deltaTime = 0.05f;
		lastTicks = currentTicks;

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				mIsRunning = false;
			}

			if (mCurrentScene) {
				mCurrentScene->ProcessEvent(e);
			}

			if (mIsPaused && menuButton) {
				menuButton->HandleEvent(e);
			}
		}

		const Uint8* keyState = SDL_GetKeyboardState(NULL);

		if (mCurrentScene) {
			mCurrentScene->ProcessInput(keyState);
			mCurrentScene->Update(deltaTime);

			SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
			SDL_RenderClear(mRenderer);

			mCurrentScene->Draw(mRenderer);
			SDL_RenderPresent(mRenderer);

			if (mCurrentScene->IsFinished()) {
				ChangeScene(mCurrentScene->NextScene());
			}
		}
	}
}

void Game::GameUpdate() {
	if (!mPlayer || !mPlayer->mHPComponent || !mHPBar || !mStaminaBar) {
		return;
	}

	float hpPercent = static_cast<float>(mPlayer->mHPComponent->GetHP()) / mPlayer->mHPComponent->GetMaxHP();
	mHPBar->SetHPPercent(hpPercent);

	float staminaPercent = (mPlayer->ic->mBoostTimer / mPlayer->ic->mBoostDuration);
	mStaminaBar->SetHPPercent(staminaPercent);

	if (mPlayer && mLevelBar) {
		int currentExp = mPlayer->GetCurrentEXP();
		int expToNextLevel = mPlayer->GetEXPToNextLevel();

		float expPercent = (expToNextLevel > 0) ? (static_cast<float>(currentExp) / expToNextLevel) : 0.0f;
		mLevelBar->SetHPPercent(expPercent);
	}

	const Uint32 currentTicks = SDL_GetTicks();
	float deltaTime = (currentTicks - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	mTicksCount = currentTicks;

	if (mIsPaused) {
		return;
	}
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓　ここからポーズ時に停止する処理　↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	if (mPlayer && mPlayer->GetState() != Actor::EAlive && !mSceneChangeRequested) {
		SDL_Log("GameOver: Player is dead. Requesting Result Scene.");

		const bool isWin = false;
		const int earnedCurrency = 0;

		mSceneChangeRequested = true;
		mNextScene = std::make_unique<ResultScene>(this, mRenderer, isWin, earnedCurrency);
		return;
	}

	UpdateActors(deltaTime);

	bool missionObjectiveAchieved = false;

	if (mMissionData.targetType == "Elimination") {
		bool allEnemiesDead = mEnemies.empty();

		if (!allEnemiesDead) {
			allEnemiesDead = true;
			for (auto enemy : mEnemies) {
				if (enemy->GetState() == Actor::EAlive) {
					allEnemiesDead = false;
					break;
				}
			}
		}
		missionObjectiveAchieved = allEnemiesDead;
	}
	else if (mMissionData.targetType == "Destruction") {
		bool foundAliveTarget = false;
		int aliveCount = 0;

		for (auto target : mTargetActors) {
			if (target && target->GetState() == Actor::EAlive) {
				foundAliveTarget = true;
				aliveCount++;
			}
		}

		missionObjectiveAchieved = !foundAliveTarget;
	}
	else if (mMissionData.targetType == "Defense") {
		bool aliveDefenseTarget = false;

		// 1. まず現在のターゲットリストが有効かチェック
		for (auto it = mTargetActors.begin(); it != mTargetActors.end(); ) {
			Actor* target = *it;

			// ポインタ自体がnullでないか、および削除待ち(EStop)でないか確認
			if (target != nullptr && target->GetState() == Actor::EAlive) {
				// dynamic_castは計算負荷が高く、無効なポインタだとクラッシュするため慎重に行う
				if (dynamic_cast<AllyBuilding*>(target)) {
					aliveDefenseTarget = true;
					break;
				}
				++it;
			}
			else {
				// 死んでいる、または無効なアクターはリストから除外（念のための安全策）
				it = mTargetActors.erase(it);
			}
		}

		// 2. 防衛対象が全滅した瞬間にシーン遷移フラグを立てる
		if (!aliveDefenseTarget && !mSceneChangeRequested) {
			mSceneChangeRequested = true;
			mNextScene = std::make_unique<ResultScene>(this, mRenderer, false, 0);
			return; // 遷移が決まったら即座に抜ける
		}

		mDefenseTimer += deltaTime;
		if (mDefenseTimer >= mMissionData.defenseDuration) {
			missionObjectiveAchieved = true;
		}
	}

	mGameTimer += deltaTime;

	// Game.cpp の増援出現処理セクションを修正

	for (auto& wave : mMissionData.reinforcements) {
		// 未スポーンかつ時間が経過している場合のみ実行
		if (!wave.spawned && mGameTimer >= wave.time) {
			// 処理の最初にフラグを立てて、重複実行を物理的に遮断する
			wave.spawned = true;

			float mapW = CameraComponent::mMapWidth;
			float mapH = CameraComponent::mMapHeight;
			Vector2 pPos = mPlayer->GetPosition();

			// 四隅から最も遠い場所を特定する計算
			std::vector<Vector2> corners = {
				Vector2(0.0f, 0.0f),
				Vector2(mapW, 0.0f),
				Vector2(0.0f, mapH),
				Vector2(mapW, mapH)
			};

			Vector2 farCorner = corners[0];
			float maxDistSq = -1.0f;
			for (const auto& c : corners) {
				float d = (pPos - c).LengthSq();
				if (d > maxDistSq) {
					maxDistSq = d;
					farCorner = c;
				}
			}

			// wave.countで指定された数だけ正確に生成
			for (int i = 0; i < wave.count; i++) {
				Enemy* enemy = nullptr;
				EnemyConfig cfg;
				cfg.type = wave.type;

				// 出現位置が重ならないように微調整
				Vector2 spawnPos = farCorner;
				spawnPos.x += (spawnPos.x < 100.0f) ? 100.0f : -100.0f;
				spawnPos.y += (spawnPos.y < 100.0f) ? 100.0f : -100.0f;
				spawnPos += Vector2(i * 40.0f, (i % 2) * 40.0f);

				InstanceEnemy(cfg, enemy, spawnPos);

				if (enemy && !mEnemies.empty()) {
					if (auto stateComp = enemy->GetComponent<EnemyStateComponent>()) {
						stateComp->SetIsHunter(true);
						stateComp->SetState(EnemyStateComponent::EState::Attack);

						if (auto attackComp = enemy->GetComponent<RangedAttackComponent>()) {
							attackComp->mChaseRange = 99999.0f;
							attackComp->SetIsActive(true);
						}
					}
				}
			}
		}
	}

	if (missionObjectiveAchieved && !mMissionCompleted) {
		mMissionCompleted = true;
		mMissionCompleteTime = SDL_GetTicks();
	}

	if (mMissionCompleted) {
		if (mMissionTextComponent) {
			mMissionTextComponent->Update(deltaTime);
		}

		Uint32 currentTime = SDL_GetTicks();
		if (currentTime - mMissionCompleteTime >= 5000) {
			if (!mSceneChangeRequested) {
				mSceneChangeRequested = true;

				const bool isWin = true;
				const int earnedCurrency = mCreditsEarned;
				mNextScene = std::make_unique<ResultScene>(this, mRenderer, isWin, earnedCurrency);
			}
		}
	}

	if (mHUDActor && mHUDActor->GetMiniMapLogic()) {
		std::vector<Vector2> enemyPositions;
		for (auto enemy : mEnemies) {
			if (enemy && enemy->GetState() == Actor::EAlive) {
				enemyPositions.push_back(enemy->GetPosition());
			}
		}
		mHUDActor->GetMiniMapLogic()->SetTargets(enemyPositions);
		mHUDActor->GetMiniMapLogic()->SetPlayerPosition(mPlayer->GetPosition());
	}

	if (mSceneChangeRequested) {
		ChangeScene(std::move(mNextScene));
		mSceneChangeRequested = false;
	}

	mOceanTime += deltaTime;

	if (mCurrentScene && dynamic_cast<GameScene*>(mCurrentScene.get())) {
		TileMapComponent* tileMap = nullptr;
		for (auto actor : mActors) {
			tileMap = actor->GetComponent<TileMapComponent>();
			if (tileMap) break;
		}

		if (tileMap) {
			tileMap->UpdateFade(deltaTime);
		}

		if (tileMap && tileMap->IsAnimated()) {
			const float TIME_PER_FRAME = 1.50f;
			const int TOTAL_FRAMES = 2;

			if (TOTAL_FRAMES > 0) {
				int currentFrame = (int)(mOceanTime / TIME_PER_FRAME);
				currentFrame = currentFrame % TOTAL_FRAMES;
				tileMap->SetAnimationFrame(currentFrame);
			}
		}
	}

	for (auto& drawFunc : mImmediateDraws) {
		drawFunc(mRenderer, mCameraPos);
	}
	mImmediateDraws.clear();
}

void Game::UpdateMenu(float deltaTime) {
	UpdateActors(deltaTime);
}

void Game::UpdateActors(float deltaTime) {
	mUpdatingActors = true;
	for (auto actor : mActors) {
		if (actor) {
			actor->Update(deltaTime);
		}
	}
	mUpdatingActors = false;

	for (auto pending : mWaitingActors) {
		mActors.emplace_back(pending);
	}
	mWaitingActors.clear();
	DestroyStoppedActors();
}

void Game::DestroyStoppedActors() {
	std::vector<Actor*> stoppedActors;
	stoppedActors.reserve(mActors.size());
	for (auto actor : mActors) {
		if (actor && actor != mPlayer && actor->GetState() == Actor::EStop) {
			stoppedActors.emplace_back(actor);
		}
	}

	for (auto actor : stoppedActors) {
		RemoveActor(actor);
	}
}

void Game::DrawSpritesSimple(SDL_Renderer* renderer) {
	for (auto sprite : mSprites) {
		if (sprite && sprite->GetOwner()->GetState() != Actor::EStop) {
			//メニュー用スプライトは描画順10000以上。ゲーム用は10000未満
			if (sprite->GetDrawOrder() < 10000) {
				continue;
			}
			sprite->Draw(renderer, Vector2(0, 0));
		}
	}
}

void Game::GameGenerate() {
	SDL_SetRenderDrawColor(mRenderer, 100, 200, 100, 255);
	SDL_RenderClear(mRenderer);

	if (mPlayer) {
		mCameraPos = mPlayer->GetCameraComponent()->GetCameraPos();
	}

	for (auto sprite : mSprites) {
		//メニュー用スプライトは描画順10000以上。ゲーム用は10000未満
		if (sprite->GetDrawOrder() >= 10000) {
			continue;
		}
		if (sprite && sprite->GetOwner()->GetState() != Actor::EStop) {
			sprite->Draw(mRenderer, mCameraPos);
		}
	}

	for (auto line : mLineComponents) {
		if (line && line->GetOwner()->GetState() != Actor::EStop) {
			line->Draw(mRenderer, mCameraPos);
		}
	}

	if (mMissionCompleted && mMissionTextComponent) {
		mMissionTextComponent->Draw();
	}

	if (mTargetIndicator) {
		mTargetIndicator->Draw(mRenderer);
	}

	if (mIsPaused) {
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 150);
		SDL_Rect fullScreen = { 0, 0, 1024, 768 };
		SDL_RenderFillRect(mRenderer, &fullScreen);
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);

		if (menuButton) menuButton->Draw(mRenderer);
		this->DrawSpritesSimple(mRenderer);

		if (mFont) {
			SDL_Color white = { 255, 255, 255, 255 };
			SDL_Surface* surf = TTF_RenderUTF8_Blended(mFont, "PAUSE", white);
			if (surf) {
				SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
				SDL_Rect dst = { (1024 - surf->w) / 2, (768 - surf->h) / 2, surf->w, surf->h };
				SDL_RenderCopy(mRenderer, tex, nullptr, &dst);
				SDL_FreeSurface(surf);
				SDL_DestroyTexture(tex);
			}
		}
	}

	SDL_RenderPresent(mRenderer);
}

void Game::GameInput() {
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	static bool wasEscPressed = false;
	bool isEscPressed = keyState[SDL_SCANCODE_ESCAPE];

	if (isEscPressed && !wasEscPressed) {
		mIsPaused = !mIsPaused;
	}
	wasEscPressed = isEscPressed;

	if (mIsPaused) {
		if (menuButton && menuButton->IsClicked()) {
			mIsPaused = false;
			mSceneChangeRequested = true;
			mNextScene = std::make_unique<MenuScene>(this, mRenderer);
		}
		return;
	}

	mUpdatingActors = true;
	for (auto actor : mActors) {
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::LoadData(int missionID) {
	SetCurrentMissionID(missionID);

	mPlayer = new Player(this);

	if (mHasPendingWeapon) {
		WeaponData baseData;
		if (baseData.LoadFromJSON("Assets/Data/WeaponData.json", mPendingWeapon.name)) {
			mPlayer->SetWeapon(baseData);
		}
		else {
			mPlayer->SetWeapon(mPendingWeapon);
		}
	}

	mMissionStartTime = SDL_GetTicks() / 1000.0f;
	mEnemiesKilledCount = 0;

	if (!MissionDataLoader::LoadMission(missionID, mMissionData)) {
		SDL_Log("ERROR: Failed to load mission data for ID %d. Using fallback.", missionID);
		mMissionData = MissionData{};
		mMissionData.id = missionID;
		mMissionData.name = "Fallback Mission";
		mMissionData.mapWidth = 2000.0f;
		mMissionData.mapHeight = 2000.0f;
		mMissionData.playerPos = Vector2(1000.0f, 1000.0f);
		mMissionData.mapTileConfig.tileType = "City";
		mMissionData.mapTileConfig.tileSize = 540;
		mMissionData.mapTileConfig.tileSetRows = 3;
		mMissionData.mapTileConfig.tilesetTexture = "Assets/MapTile.png";
	}

	PlayBGM("Assets/Audio/Drumnbass_03.mp3", -1);
	mPlayer->SetPosition(mMissionData.playerPos);
	mPlayer->SetRotation(Math::PiOver2);

	std::string equippedWeaponName;
	if (dataManager.LoadGameData("Assets/Data/SaveData.json", equippedWeaponName)) {
		if (!equippedWeaponName.empty()) {
			WeaponData baseWeapon;
			if (baseWeapon.LoadFromJSON("Assets/Data/WeaponData.json", equippedWeaponName)) {
				mPlayer->SetWeapon(baseWeapon);
			}
		}
	}

	if (mHasPendingWeapon) {
		mPlayer->SetWeapon(mPendingWeapon);
		mHasPendingWeapon = true;
	}

	CameraComponent::mMapWidth = mMissionData.mapWidth;
	CameraComponent::mMapHeight = mMissionData.mapHeight;

	int tileSize = mMissionData.mapTileConfig.tileSize;
	int tileNumWidth = static_cast<int>((CameraComponent::mMapWidth + tileSize - 1.0f) / tileSize);
	int tileNumHeight = static_cast<int>((CameraComponent::mMapHeight + tileSize - 1.0f) / tileSize);

	SDL_Texture* tileSetTex = GetTexture(mMissionData.mapTileConfig.tilesetTexture);

	Actor* mapActor = new Actor(this);
	TileMapComponent* tileMap = new TileMapComponent(mapActor);
	tileMap->SetTileSet(tileSetTex, tileSize, mMissionData.mapTileConfig.tileSetRows);

	std::vector<std::vector<int>> mapData(tileNumHeight, std::vector<int>(tileNumWidth, 0));
	srand(static_cast<unsigned int>(time(nullptr)));

	if (mMissionData.mapTileConfig.tileType == "City") {
		for (int y = 0; y < tileNumHeight; ++y) {
			for (int x = 0; x < tileNumWidth; ++x) {
				int r = rand() % 100;
				if (y < tileNumHeight / 3 || y > tileNumHeight * 2 / 3 || x < tileNumWidth / 3 || x > tileNumWidth * 2 / 3) {
					if (r < 60) mapData[y][x] = 0;
					else mapData[y][x] = 3;
				}
				else {
					if (r < 25) mapData[y][x] = 1;
					else if (r < 50) mapData[y][x] = 2;
					else if (r < 75) mapData[y][x] = 4;
					else mapData[y][x] = 5;
				}
			}
		}
	}
	else if (mMissionData.mapTileConfig.tileType == "Desert") {
		for (int y = 0; y < tileNumHeight; ++y) {
			for (int x = 0; x < tileNumWidth; ++x) {
				int r = rand() % 100;
				if (r < 20) mapData[y][x] = 1;
				else if (r < 40) mapData[y][x] = 2;
				else if (r < 60) mapData[y][x] = 3;
				else if (r < 80) mapData[y][x] = 4;
				else mapData[y][x] = 5;
			}
		}
	}
	else if (mMissionData.mapTileConfig.tileType == "Ocean") {
		for (int y = 0; y < tileNumHeight; ++y) {
			for (int x = 0; x < tileNumWidth; ++x) {
				mapData[y][x] = 0;
			}
		}
		tileMap->SetIsAnimated(true);
		tileMap->SetAnimationRows(mMissionData.mapTileConfig.tileSetRows);
	}

	tileMap->SetMapData(mapData);
	mapActor->SetPosition(Vector2(0, 0));

	mHUDActor = new HUDActor(this);
	mTargetIndicator = new TargetIndicatorComponent(mHUDActor);
	mHPBack = new HUDSpriteComponent(mHUDActor);
	mHPBar = new HUDSpriteComponent(mHUDActor);
	mHPBack->SetTexture(GetTexture("Assets/HPBack.png"));
	mHPBar->SetTexture(GetTexture("Assets/HP.png"));
	mHPBack->SetOffset(Vector2(250.0f, 50.0f));
	mHPBar->SetOffset(Vector2(250.0f, 50.0f));
	mHPBar->SetHPPercent(1.0f);

	mStaminaBack = new HUDSpriteComponent(mHUDActor);
	mStaminaBar = new HUDSpriteComponent(mHUDActor);
	mStaminaBack->SetTexture(GetTexture("Assets/StaminaBack.png"));
	mStaminaBar->SetTexture(GetTexture("Assets/Stamina1.png"));
	mStaminaBack->SetOffset(Vector2(250.0f, 88.0f));
	mStaminaBar->SetOffset(Vector2(250.0f, 84.0f));
	mStaminaBar->SetHPPercent(1.0f);

	mLevelBack = new HUDSpriteComponent(mHUDActor);
	mLevelBar = new HUDSpriteComponent(mHUDActor);
	mLevelBack->SetTexture(GetTexture("Assets/HPBack.png"));
	mLevelBar->SetTexture(GetTexture("Assets/EXPBar.png"));
	const Vector2 LevelBarOffset(250.0f, 126.0f);
	mLevelBack->SetOffset(LevelBarOffset);
	mLevelBar->SetOffset(LevelBarOffset);
	mLevelBar->SetHPPercent(0.5f);

	auto weaponBack = new HUDSpriteComponent(mHUDActor);
	weaponBack->SetTexture(GetTexture("Assets/weaponBackGround.png"));
	weaponBack->SetOffset(Vector2(200.0f, 630.0f));

	auto weaponIcon = new HUDSpriteComponent(mHUDActor);
	if (mPlayer && mPlayer->GetWeaponComponent()) {
		std::string iconPath = mPlayer->GetWeaponComponent()->GetWeaponIconPath();
		weaponIcon->SetTexture(GetTexture(iconPath));
	}
	else {
		weaponIcon->SetTexture(this->GetTexture("Assets/Weapon1.png"));
	}

	weaponIcon->SetOffset(Vector2(200.0f, 630.0f));
	mHUDActor->SetPosition(Vector2(0.0f, 0.0f));

	if (!mMissionData.targetConfigs.empty()) {
		for (const auto& config : mMissionData.targetConfigs) {
			TargetActor* target = new TargetActor(this);
			target->SetPosition(config.position);

			if (target->GetHPComponent() && config.hp > 0) {
				target->GetHPComponent()->SetMaxHP(config.hp);
				target->GetHPComponent()->SetHP(config.hp);
			}

			if (auto sc = target->GetComponent<SpriteComponent>()) {
				if (!config.texture.empty()) {
					sc->SetTexture(GetTexture(config.texture));
				}
			}
			mTargetActors.push_back(target);
		}
	}
	if (mMissionData.targetType == "Defense") {
		AllyBuilding* building = new AllyBuilding(
			this,
			mMissionData.defenseTarget.position,
			mMissionData.defenseTarget.hp,
			mMissionData.defenseTarget.texture
		);

		mTargetActors.push_back(building);
		mDefenseTimer = 0.0f;
	}

	for (const auto& config : mMissionData.enemyConfigs) {
		float mapW = CameraComponent::mMapWidth;
		float mapH = CameraComponent::mMapHeight;
		Vector2 pos;
		Enemy* enemy = nullptr;

		// 距離判定用の閾値 (500^2)
		const float safeRadiusSq = 800.0f * 800.0f;
		Vector2 pPos = mPlayer->GetPosition();

		if (config.type == "Formation") {
			pos = config.centerPosition;
			InstanceEnemy(config, enemy, pos);
		}
		else {
			for (int i = 0; i < config.count; i++) {
				if (!config.fixedPositions.empty() && i < config.fixedPositions.size()) {
					pos = config.fixedPositions[i];
				}
				else {
					bool validPos = false;
					int tries = 0;
					while (!validPos && tries < 100) {
						float randX = Random::GetFloatRange(100.0f, mapW - 100.0f);
						float randY = Random::GetFloatRange(100.0f, mapH - 100.0f);
						pos = Vector2(randX, randY);

						if ((pos - pPos).LengthSq() > safeRadiusSq) {
							validPos = true;
						}
						tries++;
					}
				}

				InstanceEnemy(config, enemy, pos);

				if (enemy) {
					HUDSpriteComponent* hpBar = new HUDSpriteComponent(mHUDActor);
					hpBar->SetTexture(GetTexture("Assets/HP1.png"));
					hpBar->SetOffset(Vector2(0, -50.0f));
					hpBar->SetTargetActor(enemy);
					enemy = nullptr;
				}
			}
		}
	}

	for (const auto& config : mMissionData.allyConfigs) {
		for (int i = 0; i < config.count; i++) {
			Vector2 spawnPos;
			if (i < config.fixedPositions.size()) {
				spawnPos = config.fixedPositions[i];
			}
			else {
				// 固定位置がない場合はプレイヤーの近くにランダム配置
				spawnPos = mPlayer->GetPosition() + Vector2(Random::GetFloatRange(-300.0f, 300.0f), Random::GetFloatRange(-300.0f, 300.0f));
			}

			AllyAIComponent::EAllyMode initialMode = AllyAIComponent::EAttack;
			if (config.initialMode == "Stay") initialMode = AllyAIComponent::EStay;

			AllyUnit* ally = nullptr;
			if (config.type == "AllyTank") {
				ally = new AllyTank(this, initialMode, spawnPos);
			}
			else if (config.type == "AllyTurret") {
				ally = new AllyTurret(this, initialMode, spawnPos);
			}
			else if (config.type == "AllyFighter") {
				// AllyFighterクラスがない場合はAllyUnitを生成してタイプをセット
				ally = new AllyUnit(this, initialMode, spawnPos);
				ally->GetAI()->SetUnitType(AllyAIComponent::EFighter);
			}
			else {
				// デフォルトはドローン
				ally = new AllyUnit(this, initialMode, spawnPos);
				ally->GetAI()->SetUnitType(AllyAIComponent::EDrone);
			}

			if (ally) {
				// 友軍にもHPバーを表示する場合
				HUDSpriteComponent* hpBar = new HUDSpriteComponent(mHUDActor);
				hpBar->SetTexture(GetTexture("Assets/HP1.png"));
				hpBar->SetOffset(Vector2(0, -40.0f));
				hpBar->SetTargetActor(ally);
			}
		}
	}

	const int MissionTextY = 500;
	mMissionTextComponent = std::make_unique<BlinkingTextComponent>(
		mRenderer,
		mFont,
		"Mission Completed!  Return to Base",
		0,
		MissionTextY,
		0.8f
	);

	menuButton = std::make_unique<UIButton>(412, 450, 200, 60, "Back to Menu", mRenderer, mFont);

	SDL_Log("DEBUG: Actors size after LoadData: %zu", mActors.size());
}

void Game::UnloadData() {
	DestroyAllActors();
	mSprites.clear();
	mEnemies.clear();
	mTargetActors.clear();
	mAllies.clear();
	mLineComponents.clear();

	for (auto& tex : mTextures) {
		SDL_DestroyTexture(tex.second);
	}
	mTextures.clear();

	menuButton.reset();
	mMissionTextComponent.reset();

	mPlayer = nullptr;
	mCamera = nullptr;
	mHUDActor = nullptr;
	mHPBack = nullptr;
	mHPBar = nullptr;
	mStaminaBack = nullptr;
	mStaminaBar = nullptr;
	mLevelBack = nullptr;
	mLevelBar = nullptr;
	mTargetIndicator = nullptr;
	mHPComponent = nullptr;
}

void Game::ChangeScene(std::unique_ptr<Scene> newScene) {
	if (mCurrentScene && dynamic_cast<GameScene*>(mCurrentScene.get()) != nullptr) {
		if (mPlayer && mPlayer->GetWeaponComponent()) {
			mPendingWeapon = mPlayer->GetWeaponComponent()->GetWeaponData();
			mHasPendingWeapon = true;
		}

		UnloadData();
		ResetGameSceneState();
	}

	if (mCurrentBGM) {
		StopBGM();
	}

	mCurrentScene = std::move(newScene);
	if (mCurrentScene) {
		mCurrentScene->LoadContent();
		mCurrentScene->LoadData();
	}
}

void Game::ResetGameSceneState() {
	mMissionCompleted = false;
	mMissionCompleteTime = 0;
	mSceneChangeRequested = false;
	mNextScene = nullptr;
	mGameTimer = 0.0f;
	mDefenseTimer = 0.0f;
	mCreditsEarned = 0;
}

void Game::Shutdown() {
	if (mShutdown) {
		return;
	}
	mShutdown = true;

	if (mPlayer) {
		dataManager.SaveGameData("Assets/Data/SaveData.json", mPlayer->GetCurrentWeaponName());
	}

	StopBGM();
	mNextScene.reset();
	mCurrentScene.reset();
	UnloadData();
	mEnemyFactory.reset();

	if (mFont) {
		TTF_CloseFont(mFont);
		mFont = nullptr;
	}
	if (mRenderer) {
		SDL_DestroyRenderer(mRenderer);
		mRenderer = nullptr;
	}
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
	if (mAudioInitialized) {
		Mix_CloseAudio();
		mAudioInitialized = false;
	}
	if (mMixerInitialized) {
		Mix_Quit();
		mMixerInitialized = false;
	}
	if (mTTFInitialized) {
		TTF_Quit();
		mTTFInitialized = false;
	}
	if (mImageInitialized) {
		IMG_Quit();
		mImageInitialized = false;
	}
	if (mSDLInitialized) {
		SDL_Quit();
		mSDLInitialized = false;
	}
}

void Game::AddActor(Actor* actor) {
	if (std::find(mActors.begin(), mActors.end(), actor) != mActors.end() ||
		std::find(mWaitingActors.begin(), mWaitingActors.end(), actor) != mWaitingActors.end()) {
		SDL_Log("Warning: Actor %p is already added!", actor);
		return;
	}

	if (mUpdatingActors) {
		mWaitingActors.emplace_back(actor);
	}
	else {
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor) {
	if (!actor) {
		return;
	}

	bool wasOwned = false;
	auto iter_waiting = std::find(mWaitingActors.begin(), mWaitingActors.end(), actor);
	if (iter_waiting != mWaitingActors.end()) {
		std::iter_swap(iter_waiting, mWaitingActors.end() - 1);
		mWaitingActors.pop_back();
		wasOwned = true;
	}

	auto iter_actors = std::find(mActors.begin(), mActors.end(), actor);
	if (iter_actors != mActors.end()) {
		std::iter_swap(iter_actors, mActors.end() - 1);
		mActors.pop_back();
		wasOwned = true;
	}

	if (!wasOwned) {
		SDL_Log("Warning: Attempted to remove unmanaged Actor %p", actor);
		return;
	}

	if (auto enemy = dynamic_cast<Enemy*>(actor)) {
		RemoveEnemy(enemy);
	}

	auto iter_target = std::find(mTargetActors.begin(), mTargetActors.end(), actor);
	if (iter_target != mTargetActors.end()) {
		std::iter_swap(iter_target, mTargetActors.end() - 1);
		mTargetActors.pop_back();
	}

	if (auto ally = dynamic_cast<AllyUnit*>(actor)) {
		RemoveAlly(ally);
	}

	delete actor;
}

void Game::DestroyAllActors() {
	mUpdatingActors = false;
	while (!mWaitingActors.empty()) {
		RemoveActor(mWaitingActors.back());
	}
	while (!mActors.empty()) {
		RemoveActor(mActors.back());
	}
}

void Game::AddSprite(SpriteComponent* sprite) {
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); iter++) {
		if (myDrawOrder < (*iter)->GetDrawOrder()) {
			break;
		}
	}
	mSprites.insert(iter, sprite);
}

SDL_Texture* Game::GetTexture(const std::string& fileName) {
	SDL_Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);

	if (iter != mTextures.end()) {
		tex = iter->second;
	}
	else {
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf) {
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex) {
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName, tex);
	}
	return tex;
}

void Game::RemoveSprite(SpriteComponent* sprite) {
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end()) {
		mSprites.erase(iter);
	}
}

void Game::UpdateObtainedWeapons() {
	std::vector<std::string> allWeaponNames = {
		"MusinGun",
		"ShotGun",
		"RailGun"
	};

	mObtainedWeaponNames.clear();

	for (const std::string& name : allWeaponNames) {
		WeaponData data;
		if (data.LoadFromJSON("Assets/Data/WeaponData.json", name) && data.gotWeapon) {
			mObtainedWeaponNames.push_back(name);
		}
	}
}

const std::vector<std::string>& Game::GetObtainedWeaponNames() const {
	return mObtainedWeaponNames;
}

void Game::AddObtainedWeapon(const std::string& weaponName) {
	if (!dataManager.IsWeaponObtained(weaponName)) {
		dataManager.SetWeaponObtained(weaponName, true);
		SDL_Log("INFO: Weapon obtained: %s", weaponName.c_str());
	}
}

void Game::AddEnemy(Enemy* enemy) {
	auto it = std::find(mEnemies.begin(), mEnemies.end(), enemy); 
	if (it == mEnemies.end()) {
		mEnemies.emplace_back(enemy); 
	} 
}

void Game::RemoveEnemy(Enemy* enemy) {
	auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
	if (iter != mEnemies.end()) {
		std::iter_swap(iter, mEnemies.end() - 1);
		mEnemies.pop_back();
	}
}

Enemy* Game::GetNearestEnemy(const Vector2& pos) {
	Enemy* nearest = nullptr;
	float minSqDist = FLT_MAX;

	for (Enemy* enemy : mEnemies) {
		if (enemy->GetState() != Actor::EAlive) {
			continue;
		}

		Vector2 diff = enemy->GetPosition() - pos;
		float sqDist = diff.LengthSq();

		if (sqDist < minSqDist) {
			minSqDist = sqDist;
			nearest = enemy;
		}
	}
	return nearest;
}

void Game::AddAlly(AllyUnit* ally) {
	mAllies.emplace_back(ally);
}

void Game::RemoveAlly(AllyUnit* ally) {
	auto iter = std::find(mAllies.begin(), mAllies.end(), ally);
	if (iter != mAllies.end()) {
		std::iter_swap(iter, mAllies.end() - 1);
		mAllies.pop_back();
	}
}

Actor* Game::GetNearestTarget(const Vector2& pos) {
	Actor* nearest = nullptr;
	float minDist = std::numeric_limits<float>::max();

	// 1. プレイヤー（生存確認）
	if (mPlayer && mPlayer->GetState() == Actor::EAlive) {
		float dist = (mPlayer->GetPosition() - pos).LengthSq();
		minDist = dist;
		nearest = mPlayer;
	}

	// 2. 味方ユニット
	for (auto ally : mAllies) {
		if (ally && ally->GetState() == Actor::EAlive) {
			float dist = (ally->GetPosition() - pos).LengthSq();
			if (dist < minDist) {
				minDist = dist;
				nearest = ally;
			}
		}
	}

	// 3. 破壊対象（TargetActor）
	for (auto target : mTargetActors) {
		if (target && target->GetState() == Actor::EAlive) {
			float dist = (target->GetPosition() - pos).LengthSq();
			if (dist < minDist) {
				minDist = dist;
				nearest = target;
			}
		}
	}

	return nearest;
}

void Game::AddLineComponent(LineComponent* line) {
	mLineComponents.emplace_back(line);
}

void Game::RemoveLineComponent(LineComponent* line) {
	auto iter = std::find(mLineComponents.begin(), mLineComponents.end(), line);
	if (iter != mLineComponents.end()) {
		std::iter_swap(iter, mLineComponents.end() - 1);
		mLineComponents.pop_back();
	}
}

void Game::AddTargetActor(Actor* actor) {
	auto it = std::find(mTargetActors.begin(), mTargetActors.end(), actor);
	if (it == mTargetActors.end()) {
		mTargetActors.emplace_back(actor);
	}
}

void Game::PlayBGM(const std::string& fileName, int loop) {
	if (mCurrentBGM) {
		Mix_HaltMusic();
		Mix_FreeMusic(mCurrentBGM);
		mCurrentBGM = nullptr;
	}

	mCurrentBGM = Mix_LoadMUS(fileName.c_str());
	if (!mCurrentBGM) {
		SDL_Log("Failed to load BGM: %s", Mix_GetError());
		return;
	}

	Mix_VolumeMusic(24);
	Mix_PlayMusic(mCurrentBGM, loop);
}

void Game::StopBGM() {
	if (mCurrentBGM) {
		Mix_HaltMusic();
		Mix_FreeMusic(mCurrentBGM);
		mCurrentBGM = nullptr;
	}
}

void Game::RequestSceneChange(std::unique_ptr<Scene> newScene) {
	mSceneChangeRequested = true;
	mNextScene = std::move(newScene);
}

std::vector<UpgradeOption> Game::GetRandomUpgrades(int count) {
	std::vector<UpgradeOption> allOptions = {
		{ UpgradeType::DamageBoost, "火力強化", "攻撃力が15%上昇", 1.15f },
		{ UpgradeType::FireRateBoost, "連射強化", "発射間隔が短縮", 0.90f },
		{ UpgradeType::DefenseBoost, "装甲強化", "被ダメージが10%減少", 0.90f },
		{ UpgradeType::SpeedBoost, "機動力強化", "移動速度が10%上昇", 1.10f },
		{ UpgradeType::Repair , "回復" , "体力を40%回復する", 1.40f },
		{ UpgradeType::SummonAlly, "増援要請", "友軍攻撃ヘリを投入", 1.0f }
	};

	std::shuffle(allOptions.begin(), allOptions.end(), Math::generator);

	std::vector<UpgradeOption> selection;
	for (int i = 0; i < count && i < (int)allOptions.size(); ++i) {
		selection.push_back(allOptions[i]);
	}
	return selection;
}

void Game::InstanceEnemy(const EnemyConfig& config, Enemy*& enemy, Vector2 pos) {
	enemy = mEnemyFactory->CreateEnemy(config, pos);
}
