#include "WhiteFadeIn.h"
#include "SpriteComponent.h"
#include "Game.h"

WhiteFadeIn::WhiteFadeIn(Game* game, bool useZoom)
    : Actor(game)
    , mTimer(0.0f)
    , mUseZoom(useZoom)
{
    if (mUseZoom) {
        // 出撃時：真っ白な画面からフェードアウト
        mSprite = new SpriteComponent(this, 10001, "Assets/WhitePixel.png");
        SetPosition(Vector2(SC_W * 0.5f, SC_H * 0.5f));
        SetScale(2000.0f);
    }
    else {
        // 通常時：扉が閉まった状態からスタート
        mSprite = nullptr;
        mLeftGate = new Actor(game);
        new SpriteComponent(mLeftGate, 10000, "Assets/GateLeft.png");
        mLeftGate->SetPosition(Vector2(SC_W * 0.25f, SC_H * 0.5f));

        mRightGate = new Actor(game);
        new SpriteComponent(mRightGate, 10000, "Assets/GateLeft.png");
        mRightGate->SetPosition(Vector2(SC_W * 0.75f, SC_H * 0.5f));
        mRightGate->SetRotation(Math::Pi);
    }
}

void WhiteFadeIn::UpdateActor(float deltaTime) {
    Actor::UpdateActor(deltaTime);
    mTimer += deltaTime;
    const float DURATION = 0.5f;
    float t = Math::Min(1.0f, mTimer / DURATION);

    if (mUseZoom) {
        mSprite->SetAlpha(1.0f - t);
    }
    else {
        float ease = t * t;
        mLeftGate->SetPosition(Vector2(Math::Lerp1(SC_W * 0.25f, -GATE_W * 0.5f, ease), SC_H * 0.5f));
        mRightGate->SetPosition(Vector2(Math::Lerp1(SC_W * 0.75f, SC_W + GATE_W * 0.5f, ease), SC_H * 0.5f));
    }

    if (t >= 1.0f) {
        if (mLeftGate) mLeftGate->SetState(EStop);
        if (mRightGate) mRightGate->SetState(EStop);
        SetState(EStop);
    }
}