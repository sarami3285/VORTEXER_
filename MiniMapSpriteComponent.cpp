#include "MiniMapSpriteComponent.h"
#include "MiniMapComponent.h"
#include "Game.h"
#include <cmath>

MiniMapSpriteComponent::MiniMapSpriteComponent(Actor* owner, MiniMapComponent* logic, int drawOrder)
	: HUDSpriteComponent(owner, drawOrder)
	, mLogic(logic)
{
}

void MiniMapSpriteComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
	if (!mLogic) return;

	Vector2 center = GetOwner()->GetPosition() + mOffset;
	float radius = mLogic->GetRadius();
	float visibleRange = mLogic->GetVisibleRange();
	const Vector2& playerPos = mLogic->GetPlayerPosition();
	const auto& enemies = mLogic->GetEnemyPositions();

	// 背景 
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect bg = { static_cast<int>(center.x - radius), static_cast<int>(center.y - radius),
					static_cast<int>(radius * 2), static_cast<int>(radius * 2) };
	SDL_RenderFillRect(renderer, &bg);

	// 枠線（白い円）
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int angle = 0; angle < 360; ++angle) {
		for (float size = 30; size <= radius; size += 30) {
			float rad = angle * Math::Pi / 180.0f;
			int x = static_cast<int>(center.x + size * std::cos(rad));
			int y = static_cast<int>(center.y + size * std::sin(rad));
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}

	// 🚩 スキャンラインの残像
	float currentAngle = mLogic->GetScanAngle();
	const int TRAIL_LENGTH_DEGREES = 5;
	const int TRAIL_STEPS = 3;

	float startAngle = currentAngle - (Math::TwoPi * TRAIL_LENGTH_DEGREES / 360.0f);
	float stepAngle = (Math::TwoPi * TRAIL_LENGTH_DEGREES / 360.0f) / TRAIL_STEPS;
	const float THICKNESS_OFFSET = Math::Pi / 180.0f * 0.5f;

	for (int i = 0; i < TRAIL_STEPS; ++i) {
		float angle = startAngle + stepAngle * (float)i;
		Uint8 alpha = static_cast<Uint8>(Math::Clamp(10 + (245 * (i + 1) / TRAIL_STEPS), 0, 255));
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, alpha);

		// 1. 線の中心
		int trailX = static_cast<int>(center.x + radius * std::cos(angle));
		int trailY = static_cast<int>(center.y + radius * std::sin(angle));
		SDL_RenderDrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y), trailX, trailY);

		// 2. 太さ
		float angleMinus = angle - THICKNESS_OFFSET;
		int minusX = static_cast<int>(center.x + radius * std::cos(angleMinus));
		int minusY = static_cast<int>(center.y + radius * std::sin(angleMinus));
		SDL_RenderDrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y), minusX, minusY);

		// 3. 太さ
		float anglePlus = angle + THICKNESS_OFFSET;
		int plusX = static_cast<int>(center.x + radius * std::cos(anglePlus));
		int plusY = static_cast<int>(center.y + radius * std::sin(anglePlus));
		SDL_RenderDrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y), plusX, plusY);
	}


	// プレイヤー位置（緑の中心点）
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_Rect playerDot = { static_cast<int>(center.x) - 3, static_cast<int>(center.y) - 3, 6, 6 };
	SDL_RenderFillRect(renderer, &playerDot);

	const auto& detectedEnemies = mLogic->GetDetectedEnemies();
	const float FADE_OUT_DURATION = 1.0f;

	// 検出された敵を描画
	for (const auto& detected : detectedEnemies) {
		Vector2 diff = detected.position - playerPos;
		float dist = diff.Length();
		if (dist > visibleRange) continue;
		float ratio = detected.remainingTime / FADE_OUT_DURATION;
		Uint8 alpha = static_cast<Uint8>(Math::Clamp(ratio, 0.0f, 1.0f) * 255);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);
		Vector2 scaled = diff * (radius / visibleRange);
		SDL_Rect dot = {
			static_cast<int>(center.x + scaled.x) - 2,
			static_cast<int>(center.y + scaled.y) - 2,
			4, 4
		};

		SDL_RenderFillRect(renderer, &dot);
	}
}
