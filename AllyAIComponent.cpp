#include "AllyAIComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Enemy.h"
#include "BulletComponent.h"
#include "MoveComponent.h"
#include "Player.h"
#include "Random.h"

AllyAIComponent::AllyAIComponent(Actor* owner)
	: Component(owner)
	, mMode(EStay)
	, mAttackRange(400.0f)
	, mOrbitAngle(0.0f)
{
	mOrbitAngle = Random::GetFloatRange(0.0f, Math::TwoPi);
}

void AllyAIComponent::Update(float deltaTime)
{
	// 共通：敵がいれば攻撃（向き調整含む）
	AttackProcess(deltaTime);

	// 兵種ごとの移動ロジック
	switch (mUnitType)
	{
	case EDrone:   OrbitProcess(deltaTime);   break;
	case ETank:    TankProcess(deltaTime);    break;
	case EFighter: FighterProcess(deltaTime); break;
	case ETurret: TurretProcess(deltaTime); break;
	}
}

void AllyAIComponent::OrbitProcess(float deltaTime)
{
	Actor* target = mGuardTarget ? mGuardTarget : mOwner->GetGame()->GetPlayer();
	if (!target) return;

	// MoveComponentによる自動前進は干渉しないよう停止
	auto mc = mOwner->GetComponent<MoveComponent>();
	if (mc) { mc->SetForwardSpeed(0.0f); mc->SetAngularSpeed(0.0f); }

	const float orbitRadius = 170.0f;
	const float orbitSpeed = 0.9f;
	mOrbitAngle += orbitSpeed * deltaTime;
	mOrbitAngle = Math::WrapAngle(mOrbitAngle);

	Vector2 centerPos = target->GetPosition();
	Vector2 targetPos;
	targetPos.x = centerPos.x + orbitRadius * Math::Cos(mOrbitAngle);
	targetPos.y = centerPos.y + orbitRadius * Math::Sin(mOrbitAngle);

	// ステージ端クランプ
	targetPos.x = Math::Clamp(targetPos.x, 50.0f, 3190.0f);
	targetPos.y = Math::Clamp(targetPos.y, 50.0f, 3190.0f);

	Vector2 currentPos = mOwner->GetPosition();
	Vector2 toTarget = (targetPos + mOffset) - currentPos;
	float distToTarget = toTarget.Length();

	if (distToTarget > 1.0f)
	{
		toTarget.Normalize();
		float baseSpeed = (orbitRadius * orbitSpeed) * 1.5f;
		if (distToTarget > 100.0f) baseSpeed *= 2.0f;

		// ベクトルによる位置更新
		Vector2 nextPos = currentPos + toTarget * baseSpeed * deltaTime;
		mOwner->SetPosition(Vector2::Lerp(currentPos, nextPos, 0.4f));

		// 敵がいないときのみ移動方向を向く
		if (!SearchNearestEnemy())
		{
			float targetAngle = Math::Atan2(toTarget.y, toTarget.x);
			float currentAngle = mOwner->GetRotation();
			float angleDiff = Math::WrapAngle(targetAngle - currentAngle);
			mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.1f));
		}
	}
}

void AllyAIComponent::TankProcess(float deltaTime)
{
	Actor* center = mGuardTarget ? mGuardTarget : mOwner->GetGame()->GetPlayer();
	Actor* enemy = SearchNearestEnemy();
	if (!center) return;

	// 1. 射程内に敵がいるかチェック
	bool isEnemyInRange = false;
	if (enemy)
	{
		float distSq = (enemy->GetPosition() - mOwner->GetPosition()).LengthSq();
		if (distSq <= mAttackRange * mAttackRange) isEnemyInRange = true;
	}

	// 2. 敵がいたら静止（向きはAttackProcessで制御）
	if (isEnemyInRange) { return; }

	// 3. 敵がいないなら目標座標への移動（Orbitのベクトル移動ロジックを流用）
	mTankTimer -= deltaTime;
	if (mTankTimer <= 0.0f)
	{
		mTankTimer = 5.0f;
		float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
		float dist = Random::GetFloatRange(200.0f, 400.0f);
		mTankDest = center->GetPosition() + Vector2(Math::Cos(angle) * dist, Math::Sin(angle) * dist);
	}

	Vector2 currentPos = mOwner->GetPosition();
	Vector2 toTarget = mTankDest - currentPos;
	float distToTarget = toTarget.Length();

	if (distToTarget > 100.0f)
	{
		toTarget.Normalize();
		float speed = 150.0f;
		Vector2 nextPos = currentPos + toTarget * speed * deltaTime;
		mOwner->SetPosition(nextPos);

		float targetAngle = Math::Atan2(toTarget.y, toTarget.x);
		float currentAngle = mOwner->GetRotation();
		float angleDiff = Math::WrapAngle(targetAngle - currentAngle);
		mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.1f));
	}
}

void AllyAIComponent::TurretProcess(float deltaTime) {
	// ターゲット（プレイヤーや防衛対象）がいない場合は何もしない
	Actor* center = mGuardTarget ? mGuardTarget : mOwner->GetGame()->GetPlayer();
	if (!center) return;

	// 索敵
	Actor* enemy = SearchNearestEnemy();

	if (enemy)
	{
		Vector2 diff = enemy->GetPosition() - mOwner->GetPosition();
		float distSq = diff.LengthSq();

		// 射程内の場合
		if (distSq <= 900 *900)
		{
			// 向きの調整（AttackProcessでも行っているが、Turretは回転速度を個別に制御したい場合に有効）
			float targetAngle = Math::Atan2(diff.y, diff.x);
			float currentAngle = mOwner->GetRotation();
			float angleDiff = Math::WrapAngle(targetAngle - currentAngle);

			// 砲塔の旋回速度（0.15fで滑らかに追従）
			mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.15f));
		}
	}
	else
	{
		// 敵がいないときはプレイヤーと同じ向き、あるいはデフォルトの向きで待機
		float currentAngle = mOwner->GetRotation();
		float targetAngle = center->GetRotation();
		float angleDiff = Math::WrapAngle(targetAngle - currentAngle);
		mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.05f));
	}

	// 移動コンポーネントがある場合は速度を強制的にゼロにする（ハチの巣モードの鉄則）
	if (auto mc = mOwner->GetComponent<MoveComponent>()) {
		mc->SetForwardSpeed(0.0f);
		mc->SetAngularSpeed(0.0f);
	}
}

void AllyAIComponent::FighterProcess(float deltaTime)
{
	Actor* enemy = SearchNearestEnemy();
	Actor* center = mGuardTarget ? mGuardTarget : mOwner->GetGame()->GetPlayer();
	Actor* target = enemy ? enemy : center;
	if (!target) return;

	Vector2 currentPos = mOwner->GetPosition();
	Vector2 diff = target->GetPosition() - currentPos;
	float distSq = diff.LengthSq();

	Vector2 moveDir;
	float speed = 800.0f;

	if (mIsPassing)
	{
		// 突撃：ターゲットへ向かうベクトル
		moveDir = diff;
		moveDir.Normalize();
		if (distSq < 120.0f * 120.0f) mIsPassing = false;
	}
	else
	{
		// 離脱：現在の向きを維持して突き抜ける
		float curRot = mOwner->GetRotation();
		moveDir = Vector2(Math::Cos(curRot), Math::Sin(curRot));
		speed = 700.0f;
		if (distSq > 700.0f * 700.0f) mIsPassing = true;
	}

	// 位置更新
	mOwner->SetPosition(currentPos + moveDir * speed * deltaTime);

	// 向き更新（突撃時のみターゲットを向く、離脱時は維持）
	if (mIsPassing)
	{
		float targetAngle = Math::Atan2(moveDir.y, moveDir.x);
		float currentAngle = mOwner->GetRotation();
		float angleDiff = Math::WrapAngle(targetAngle - currentAngle);
		mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.05f));
	}
}

void AllyAIComponent::AttackProcess(float deltaTime)
{
	Actor* target = SearchNearestEnemy();
	if (!target) return;

	Vector2 diff = target->GetPosition() - mOwner->GetPosition();
	if (diff.LengthSq() <= mAttackRange * mAttackRange)
	{
		float targetAngle = Math::Atan2(diff.y, diff.x);
		float currentAngle = mOwner->GetRotation();
		float angleDiff = Math::WrapAngle(targetAngle - currentAngle);
		mOwner->SetRotation(Math::Lerp1(currentAngle, currentAngle + angleDiff, 0.2f));

		if (auto bc = mOwner->GetComponent<BulletComponent>())
		{
			bc->Fire(deltaTime);
		}
	}
}

Actor* AllyAIComponent::SearchNearestEnemy()
{
	Actor* nearest = nullptr;
	float nearestDistSq = 1e38f;
	for (auto actor : mOwner->GetGame()->GetActors())
	{
		if (dynamic_cast<Enemy*>(actor) && actor->GetState() == Actor::EAlive)
		{
			float distSq = (actor->GetPosition() - mOwner->GetPosition()).LengthSq();
			if (distSq < nearestDistSq) { nearestDistSq = distSq; nearest = actor; }
		}
	}
	return nearest;
}
