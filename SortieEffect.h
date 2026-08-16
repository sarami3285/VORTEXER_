#pragma once
#include "Actor.h"
#include <functional>

class SortieEffect : public Actor {
public:
    // useZoom が true なら出撃演出、false なら通常遷移
    SortieEffect(class Game* game, std::function<void()> onFinished, bool useZoom = false);
    void UpdateActor(float deltaTime) override;

private:
    class Actor* mLeftGate;
    class Actor* mRightGate;
    class SpriteComponent* mWhiteOverlay;

    float mTimer;
    std::function<void()> mCallback;
    bool mUseZoom;

    const float SC_W = 1024.0f;
    const float SC_H = 768.0f;
    const float GATE_W = 512.0f;
};