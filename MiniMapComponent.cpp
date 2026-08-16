#include "MiniMapComponent.h"
#include "Actor.h"
#include <cmath>
#include <algorithm>

const float SCAN_SPEED = Math::TwoPi / 3.0f;
const float DETECT_ANGLE_RANGE = 0.05f;

MiniMapComponent::MiniMapComponent(Actor* owner)
    : Component(owner)
    , mScanAngle(0.0f)
{
    
}


void MiniMapComponent::Update(float deltaTime) {
	mScanAngle += SCAN_SPEED * deltaTime;
	if (mScanAngle > Math::TwoPi) {
		mScanAngle -= Math::TwoPi;
	}

	for (auto it = mDetectedEnemies.begin(); it != mDetectedEnemies.end(); ) {
		it->remainingTime -= deltaTime;

		if (it->remainingTime <= 0.0f) {
			it = mDetectedEnemies.erase(it);
		}
		else {
			++it;
		}
	}

	const float currentScanAngle = mScanAngle;

	for (const auto& enemyPos : mEnemyPositions) {
		Vector2 diff = enemyPos - mPlayerPos;
		float dist = diff.Length();

		if (dist > mVisibleRange) {
			continue;
		}

		bool isDetected = false;
		const float CLOSE_RANGE = mVisibleRange * 0.2f; // 視認範囲の20%以内
		const float CLOSE_RANGE_SQ = CLOSE_RANGE * CLOSE_RANGE;

		if (dist * dist <= CLOSE_RANGE_SQ) {
			isDetected = true;
		}

		if (!isDetected) {
			float enemyAngle = std::atan2(diff.y, diff.x);

			if (enemyAngle < 0.0f) {
				enemyAngle += Math::TwoPi;
			}

			float angleDiff = std::fabs(enemyAngle - currentScanAngle);

			if (angleDiff > Math::Pi) {
				angleDiff = Math::TwoPi - angleDiff;
			}

			if (angleDiff <= DETECT_ANGLE_RANGE) {
				isDetected = true;
			}
		}

		if (isDetected) {
			bool alreadyDetected = false;
			for (auto& detected : mDetectedEnemies) {
				if ((detected.position - enemyPos).LengthSq() < 100.0f) {
					detected.remainingTime = FADE_OUT_DURATION;
					alreadyDetected = true;
					break;
				}
			}

			if (!alreadyDetected) {
				DetectedEnemy newDetection;
				newDetection.position = enemyPos;
				newDetection.remainingTime = FADE_OUT_DURATION;
				mDetectedEnemies.push_back(newDetection);
			}
		}
	}
}
