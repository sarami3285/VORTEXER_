#include "Player.h"
#include "LockOnComponent.h"
#include "DeathEffect.h"
#include "Scenes.h"
#include "MetalFragment.h"
#include "AllyDrone.h"
#include "AllyAIComponent.h"

#include "MissileBarrageSkill.h"
#include "DroneDeploySkill.h"
#include "PulseBarrier.h"

#include <cfloat>
Player::Player(Game* game)
	: Actor(game)
	, mAimMode(AimMode::Auto)
	, mCurrentLockTarget(nullptr)
	, mLockOnEffect(nullptr)
{
	game->SetPlayer(this);

	mSpriteComponent = new SpriteComponent(this, 10, "Assets/PlayerBack.png", true, Vector2{ 12,12 });
	mSpriteComponent->SetColor(1.0f, 1.0f, 1.0f);

	mCollision = new CollisionComponent(this, 15.0f);
	mCameraComponent = new CameraComponent(this);

	mWeaponComponent = new WeaponComponent(this);
	mWeaponComponent->SetDamageRate(1.0f);

	mCameraComponent->SetMapSize(3240.0f, 3240.0f);
	game->SetCamera(mCameraComponent);

	mHPComponent = new HPComponent(this, 100);

	ic = new InputComponent(this);

	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetRightKey(SDL_SCANCODE_A);
	ic->SetLeftKey(SDL_SCANCODE_D);
	ic->SetBoostKey(SDL_SCANCODE_LSHIFT);
	ic->SetSkillKey(SDL_SCANCODE_E);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxRightSpeed(300.0f);

	ic->SetSkill(new PulseBarrierSkill(this));

	DeffenceRate = 1.0f;
}

void Player::UpdateActor(float deltaTime)
{
	if (mAimMode == AimMode::Auto)
	{
		class Enemy* nearest = GetNearestEnemy(GetGame()->GetActors());
		if (nearest)
		{
			float distSq = (nearest->GetPosition() - GetPosition()).LengthSq();
			if (distSq > lockOnRange * lockOnRange)
			{
				nearest = nullptr;
			}
		}

		if (nearest != mCurrentLockTarget)
		{
			if (mLockOnEffect)
			{
				mLockOnEffect->SetState(Actor::State::EStop);
				mLockOnEffect = nullptr;
			}
			if (nearest)
			{
				mLockOnEffect = new LockOnEffect(GetGame(), nearest);
			}
			mCurrentLockTarget = nearest;
		}
		if (mCurrentLockTarget)
		{
			UpdateSpriteBasedOnDirection(mCurrentLockTarget->GetPosition());
		}
	}
	else
	{
		mCurrentLockTarget = nullptr;

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		Vector2 worldMousePos = ScreenToWorld(mouseX, mouseY);
		UpdateSpriteBasedOnDirection(worldMousePos);

		if (mLockOnEffect)
		{
			mLockOnEffect->SetPosition(worldMousePos);
			mLockOnEffect->SetTarget(nullptr);
		}
	}
	CollisionComponent* playerCC = mCollision;

	if (playerCC)
	{
		std::vector<MetalFragment*> fragmentsToCollect;

		for (Actor* actor : GetGame()->GetActors())
		{
			if (actor->GetState() == EAlive)
			{
				if (MetalFragment* fragment = dynamic_cast<MetalFragment*>(actor))
				{
					CollisionComponent* fragmentCC = fragment->GetCircle();

					if (fragmentCC && Intersect(*playerCC, *fragmentCC))
					{
						fragmentsToCollect.push_back(fragment);
					}
				}
			}
		}

		for (MetalFragment* fragment : fragmentsToCollect)
		{
			AddExperience(fragment->GetExperienceValue());
			fragment->SetState(Actor::State::EStop);
		}
	}
}

void Player::ActorInput(const uint8_t* keyState)
{
	if (mIsWaitingForUpgrade)
	{
		int selectedIndex = -1;
		if (keyState[SDL_SCANCODE_1]) selectedIndex = 0;
		else if (keyState[SDL_SCANCODE_2]) selectedIndex = 1;
		else if (keyState[SDL_SCANCODE_3]) selectedIndex = 2;

		if (selectedIndex != -1 && selectedIndex < (int)mCurrentOptions.size())
		{
			ApplyUpgrade(mCurrentOptions[selectedIndex]);
			for (auto ui : mUpgradeUIActors)
			{
				ui->SetState(Actor::EStop);
			}
			mUpgradeUIActors.clear();
			mCurrentOptions.clear();
			mIsWaitingForUpgrade = false;

			// 決定時の演出予定地
			SDL_Log("Upgrade Selected: %d", selectedIndex + 1);
		}
	}

	// --- 2. 照準モードの切り替え (TABキー) ---
	bool isTabPressed = keyState[SDL_SCANCODE_TAB];
	if (isTabPressed && !prevTabState)
	{
		mAimMode = (mAimMode == AimMode::Auto) ? AimMode::Manual : AimMode::Auto;
		if (mLockOnEffect)
		{
			mLockOnEffect->SetState(Actor::State::EStop);
			mLockOnEffect = nullptr;
		}
		SDL_Log("AimMode is changed");
		if (mAimMode == AimMode::Manual)
		{
			mLockOnEffect = new LockOnEffect(GetGame(), nullptr);
		}
	}
	prevTabState = isTabPressed;

	// --- 3. 攻撃処理 ---
	if (keyState[SDL_SCANCODE_SPACE])
	{
		Vector2 fireDir;

		if (mAimMode == AimMode::Auto)
		{
			class Enemy* nearest = GetNearestEnemy(GetGame()->GetActors());
			if (nearest)
			{
				float distSq = (nearest->GetPosition() - GetPosition()).LengthSq();
				if (distSq <= lockOnRange * lockOnRange)
				{
					fireDir = nearest->GetPosition() - GetPosition();
				}
				else
				{
					goto skip_fire;
				}
			}
			else
			{
				goto skip_fire;
			}
		}
		else
		{
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			Vector2 worldMousePos = ScreenToWorld(mouseX, mouseY);
			fireDir = worldMousePos - GetPosition();
		}

		if (fireDir.LengthSq() > 0.0f)
		{
			fireDir.Normalize();
			mWeaponComponent->Fire(fireDir);
		}
	}

skip_fire:
	// --- 4. 移動コンポーネントへの入力反映 ---
	if (ic)
	{
		ic->ProcessInput(keyState);
	}
}

class Enemy* Player::GetNearestEnemy(const std::vector<Actor*>& actors)
{
	class Enemy* nearest = nullptr;
	float bestDistSq = FLT_MAX;
	Vector2 myPos = GetPosition();

	for (auto actor : actors)
	{
		if (auto enemy = dynamic_cast<class Enemy*>(actor))
		{
			if (enemy->GetState() == Actor::State::EAlive)
			{
				float distSq = (enemy->GetPosition() - myPos).LengthSq();
				if (distSq < bestDistSq)
				{
					bestDistSq = distSq;
					nearest = enemy;
				}
			}
		}
	}
	return nearest;
}

void Player::UpdateSpriteBasedOnDirection(const Vector2& targetPos)
{
	Vector2 direction = targetPos - GetPosition();
	if (direction.LengthSq() > 0.0f)
	{
		direction.Normalize();
		float angle = Math::Atan2(-direction.y, direction.x);

		if (angle >= -Math::Pi / 4 && angle < Math::Pi / 4)
		{
			mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerRight.png"));
		}
		else if (angle >= Math::Pi / 4 && angle < 3 * Math::Pi / 4)
		{
			mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerForward.png"));
		}
		else if (angle >= -3 * Math::Pi / 4 && angle < -Math::Pi / 4)
		{
			mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerBack.png"));
		}
		else
		{
			mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerLeft.png"));
		}
	}
}

Vector2 Player::ScreenToWorld(int screenX, int screenY)
{
	Vector2 camPos = GetGame()->GetCamera()->GetCameraPos();
	Vector2 worldPos = Vector2(float(screenX), float(screenY)) + camPos;

	return worldPos;
}

void Player::TakeDamage(int damage) {
	const int reducedDamage = static_cast<int>(damage * DeffenceRate * BarrierRate);
	mHPComponent->TakeDamage(reducedDamage);
	mCameraComponent->Shake(static_cast<float>(damage), 0.5f);
	mSpriteComponent->FlashRed(0.1f);
	if (mHPComponent->IsDead())
	{
		//SetState(Actor::State::EStop);
		DeathEffect* effect = new DeathEffect(GetGame(), 10);
		effect->SetPosition(GetPosition());
		GetGame()->AddActor(effect);
		SetState(Actor::EStop);
	}
}
void Player::TakeRepair(int amount) {
	mHPComponent->Heal(amount);
}

void Player::SetWeapon(const WeaponData& weapon)
{
	SDL_Log("SetWeapon start: %s", weapon.name.c_str());

	CurrentWeapon = weapon;
	SDL_Log("Weapon copied");

	if (mWeaponComponent)
	{
		SDL_Log("WeaponComponent exists");
		mWeaponComponent->SetWeaponData(weapon);
		mWeaponComponent->SetCurrentWeaponName(weapon.name);
		SDL_Log("Weapon set to: %s", weapon.name.c_str());
	}
	else
	{
		SDL_Log("WeaponComponent is nullptr!");
	}
}


const WeaponData& Player::GetWeapon() const
{
	return CurrentWeapon;
}

const std::string& Player::GetCurrentWeaponName() const
{
	return CurrentWeapon.name;
}


int Player::CalculateEXPForLevel(int level)
{
	if (level < 1) return 100;
	return 100 + (level - 1) * 50;
}

void Player::LevelUp() {
	mCurrentLevel++;
	mEXPToNextLevel = CalculateEXPForLevel(mCurrentLevel);

	mCurrentOptions = GetGame()->GetRandomUpgrades(3);
	mIsWaitingForUpgrade = true;
	for (auto actor : mUpgradeUIActors) {
		actor->SetState(Actor::EStop);
	}
	mUpgradeUIActors.clear();

	for (int i = 0; i < (int)mCurrentOptions.size(); ++i) {
		Actor* uiActor = new Actor(GetGame());
		HUDSpriteComponent* bg = new HUDSpriteComponent(uiActor);

		switch (mCurrentOptions[i].type)
		{
		case UpgradeType::DamageBoost:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_DamageUp.png"));
			break;
		case UpgradeType::FireRateBoost:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_FireRateUp.png"));
			break;
		case UpgradeType::DefenseBoost:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_DefenseUp.png"));
			break;
		case UpgradeType::SpeedBoost:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_SpeedUp.png"));
			break;
		case UpgradeType::Repair:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_Repair.png"));
			break;
		case UpgradeType::SummonAlly:
			bg->SetTexture(GetGame()->GetTexture("Assets/Skill_SummonInterceptor.png"));
			break;
		default:
			//bg->SetTexture(GetGame()->GetTexture("Assets/Skill_Generic.png"));
			break;
		}

		float startX = 640.0f; // 左端のカードのX座標
		float spacing = 130.0f; // カード同士の間隔
		float yPos = 100.0f;    // 画面中央のY座標

		Vector2 pos(startX + i * spacing, yPos);
		bg->SetOffset(pos);
		bg->SetOffset(pos);
		mUpgradeUIActors.push_back(uiActor);
	}

	SDL_Log("LEVEL UP! Level: %d. Press 1, 2, or 3 to upgrade.", mCurrentLevel);
}

void Player::AddExperience(int amount)
{
	if (amount <= 0) return;

	mCurrentEXP += amount;

	while (mCurrentEXP >= mEXPToNextLevel)
	{
		mCurrentEXP -= mEXPToNextLevel;
		LevelUp();
	}
}

void Player::ApplyUpgrade(const UpgradeOption& opt) {
	switch (opt.type) {
	case UpgradeType::DamageBoost:
		mWeaponComponent->AddDamageRate(0.1f);
		break;

	case UpgradeType::FireRateBoost:
		mWeaponComponent->AddFireRate(0.05f);
		break;

	case UpgradeType::DefenseBoost:
		DeffenceRate *= 0.9f;
		break;

	case UpgradeType::Repair:
		TakeRepair(static_cast<int>(mHPComponent->GetMaxHP() * (2.0f / 5.0f)));
		break;

	case UpgradeType::SpeedBoost:
	{
		float currentForward = ic->GetMaxForwardSpeed();
		float currentRight = ic->GetMaxRightSpeed();

		ic->SetMaxForwardSpeed(currentForward * 1.1f);
		ic->SetMaxRightSpeed(currentRight * 1.1f);

		SDL_Log("Speed Up: New Max Forward = %.1f", currentForward * 1.1f);
	}
	break;
	case UpgradeType::SummonAlly:
	{
		AllyDrone* ally = new AllyDrone(GetGame(), AllyAIComponent::EFollow, GetPosition());
	}
	break;
	}
	SDL_Log("Applied: %s", opt.name.c_str());
}

bool Player::UseSkill() {
	if (mCurrentEXP < mEXPToNextLevel / 2) {
		return false;
	}
	mCurrentEXP -= mEXPToNextLevel / 2;
	return true;
}
