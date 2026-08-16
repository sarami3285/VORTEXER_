#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
    CameraComponent(class Actor* owner);

    void Update(float deltaTime) override;
    void Shake(float intensity, float duration);

    Vector2 GetCameraPos() const { return mCameraPos; }
    void SetMapSize(float width, float height) {
        int w = static_cast<int>(width);
        int h = static_cast<int>(height);

        mMapWidth = (w / static_cast<float>(32)) * 32;
        mMapHeight = (h / static_cast<float>(32)) * 32;

    }

    static float mMapWidth;
    static float mMapHeight;
    static const float MapOffset;

private:
    Vector2 mCameraPos;
    float mScreenWidth = 1024.0f;
    float mScreenHeight = 768.0f;
    float mShakeTimer = 0.0f;
    float mShakeIntensity = 0.0f;
};

