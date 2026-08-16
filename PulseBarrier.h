#pragma once
#include "ActiveSkill.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "SpriteComponent.h"
#include "Math.h"

class PulseBarrierSkill : public ActiveSkill
{
public:
    PulseBarrierSkill(Actor* owner)
        : ActiveSkill(owner, "PulseBarrier", 20.0f)
        , mIsActive(false)
        , mLifeTimer(0.0f)
        , mCurrentScale(0.0f)
    {
        // 描画用のActorを生成
        mBarrierVisual = new Actor(mOwner->GetGame());
        mBarrierVisual->SetScale(0.0f);
        mBarrierSprite = new SpriteComponent(mBarrierVisual, 150, "Assets/pulseBarrier.png");
    }

    void Update(float deltaTime) override
    {
        ActiveSkill::Update(deltaTime);

        Player* player = dynamic_cast<Player*>(mOwner);
        if (!player) return;

        if (mIsActive)
        {
            mLifeTimer -= deltaTime;
            // 展開：0から1へ補間
            mCurrentScale = Math::Lerp1(mCurrentScale, 1.0f, 5.0f * deltaTime);

            if (mLifeTimer <= 0.0f)
            {
                mIsActive = false;
                player->SetBarrierActive(false);
            }
        }
        else
        {
            // 収納：1から0へ補間
            mCurrentScale = Math::Lerp1(mCurrentScale, 0.0f, 5.0f * deltaTime);
        }

        // ビジュアル更新
        if (mBarrierVisual)
        {
            mBarrierVisual->SetPosition(mOwner->GetPosition());
            mBarrierVisual->SetScale(mCurrentScale);
        }
    }

    void ExecuteSkill() override
    {
        Player* player = dynamic_cast<Player*>(mOwner);
        if (!player) return;

        mIsActive = true;
        mLifeTimer = 10.0f; // 10秒間維持
        player->SetBarrierActive(true);
    }

private:
    Actor* mBarrierVisual;
    SpriteComponent* mBarrierSprite;
    bool mIsActive;
    float mLifeTimer;
    float mCurrentScale;
};