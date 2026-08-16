#pragma once
#include "Actor.h"

class WhiteFadeIn : public Actor {
public:
    WhiteFadeIn(class Game* game, bool useZoom = false);
    void UpdateActor(float deltaTime) override;
private:
    class SpriteComponent* mSprite;
    class Actor* mLeftGate = nullptr;
    class Actor* mRightGate = nullptr;
    float mTimer;
    bool mUseZoom;

    const float SC_W = 1024.0f;
    const float SC_H = 768.0f;
    const float GATE_W = 512.0f;
};