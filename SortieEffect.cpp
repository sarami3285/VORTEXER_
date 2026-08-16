#include "SortieEffect.h"
#include "SpriteComponent.h"
#include "Game.h"

SortieEffect::SortieEffect(Game* game, std::function<void()> onFinished, bool useZoom)
    : Actor(game)
    , mTimer(0.0f)
    , mCallback(onFinished)
    , mUseZoom(useZoom)
{
    mLeftGate = new Actor(game);
    new SpriteComponent(mLeftGate, 10000, "Assets/GateLeft.png");
    mLeftGate->SetPosition(Vector2(-GATE_W * 0.5f, SC_H * 0.5f));

    mRightGate = new Actor(game);
    new SpriteComponent(mRightGate, 10000, "Assets/GateLeft.png");
    mRightGate->SetPosition(Vector2(SC_W + GATE_W * 0.5f, SC_H * 0.5f));
    mRightGate->SetRotation(Math::Pi);

    // ホワイトアウト用（ズーム時のみ使用）
    mWhiteOverlay = new SpriteComponent(this, 10001, "Assets/WhitePixel.png");
    mWhiteOverlay->SetAlpha(0.0f);
    SetPosition(Vector2(SC_W * 0.5f, SC_H * 0.5f));
    SetScale(2000.0f);
}

void SortieEffect::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);
    mTimer += deltaTime;

    const float CLOSE_TIME = 0.5f;
    const float ZOOM_TIME = 0.5f;

    if (mTimer < CLOSE_TIME) {
        // --- 共通：扉が閉まる ---
        float t = mTimer / CLOSE_TIME;
        float ease = 1.0f - Math::Pow(1.0f - t, 2.0f);

        mLeftGate->SetPosition(Vector2(Math::Lerp1(-GATE_W * 0.5f, SC_W * 0.25f, ease), SC_H * 0.5f));
        mRightGate->SetPosition(Vector2(Math::Lerp1(SC_W + GATE_W * 0.5f, SC_W * 0.75f, ease), SC_H * 0.5f));
    }
    else {
        if (mUseZoom) {
            float t = (mTimer - CLOSE_TIME) / ZOOM_TIME;
            if (t > 1.0f) t = 1.0f;

            float scale = Math::Lerp1(1.0f, 20.0f, t * t * t);
            mLeftGate->SetScale(scale);
            mRightGate->SetScale(scale);

            float centerX = SC_W * 0.5f;
            float baseDist = SC_W * 0.25f;
            mLeftGate->SetPosition(Vector2(centerX - baseDist * scale, SC_H * 0.5f));
            mRightGate->SetPosition(Vector2(centerX + baseDist * scale, SC_H * 0.5f));

            mWhiteOverlay->SetAlpha(t);

            if (t >= 1.0f && mCallback) {
                mLeftGate->SetState(EStop);
                mRightGate->SetState(EStop);
                SetState(EStop);

                auto tempCallback = mCallback;
                mCallback = nullptr;
                tempCallback();
            }
        }
        else {
            // --- 通常時：即遷移 ---
            if (mCallback) {
                mCallback();
                mCallback = nullptr;
                mLeftGate->SetPosition(Vector2(-GATE_W * 0.5f, SC_H * 0.5f));
                mRightGate->SetPosition(Vector2(SC_W + GATE_W * 0.5f, SC_H * 0.5f));
                SetState(EStop);
            }
        }
    }
}