#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>
#include <list>
class LowLevelEnemy;

class MiniMapComponent : public Component {
public:
    MiniMapComponent(class Actor* owner);

    void Update(float deltaTime) override;

    void SetPlayerPosition(const Vector2& pos) { mPlayerPos = pos; }
    void SetTargets(const std::vector<Vector2>& enemyPositions) { mEnemyPositions = enemyPositions; }
    void SetMapSize(const Vector2& mapSize) { mMapSize = mapSize; }
    void SetRadius(float radius) { mRadius = radius; }
    void SetVisibleRange(float range) { mVisibleRange = range; }

    const std::vector<Vector2>& GetEnemyPositions() const { return mEnemyPositions; }
    const Vector2& GetPlayerPosition() const { return mPlayerPos; }
    float GetRadius() const { return mRadius; }
    float GetVisibleRange() const { return mVisibleRange; }
    const Vector2& GetMapSize() const { return mMapSize; }
    float GetScanAngle() const { return mScanAngle; }


    struct DetectedEnemy {
        Vector2 position;
        float remainingTime;
    };

    const std::list<DetectedEnemy>& GetDetectedEnemies() const { return mDetectedEnemies; }

private:
    Vector2 mPlayerPos;
    std::vector<Vector2> mEnemyPositions;
    std::list<DetectedEnemy> mDetectedEnemies;
    const float FADE_OUT_DURATION = 1.0f;
    Vector2 mMapSize;

    float mScanAngle;
    float mRadius = 90.0f;
    float mVisibleRange = 5000.0f;
};
