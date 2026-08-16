#include "DeathEffect.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "AudioComponent.h"
#include "Random.h"

// Transient effect actors are reclaimed by Game after entering EStop.
DeathEffect::DeathEffect(Game* game, int explosionCount)
    : Actor(game)
    , mRemainingCount(explosionCount)
    , mSpawnTimer(0.0f)
{
}

void DeathEffect::UpdateActor(float deltaTime)
{
    if (mRemainingCount <= 0) {
        mSpawnTimer += deltaTime;
        if (mSpawnTimer > 1.0f) SetState(EStop);
        return;
    }

    mSpawnTimer -= deltaTime;
    if (mSpawnTimer <= 0.0f) {
        mSpawnTimer = 0.12f + Random::GetFloatRange(0.0f, 0.05f);
        Vector2 offset(Random::GetFloatRange(-60.0f, 60.0f), Random::GetFloatRange(-60.0f, 60.0f));
        ExplosionParticle* p = new ExplosionParticle(GetGame());
        p->SetPosition(GetPosition() + offset);
        p->SetScale(GetScale() * Random::GetFloatRange(0.7f, 1.3f));

        mRemainingCount--;
    }
}

ExplosionParticle::ExplosionParticle(Game* game)
    : Actor(game)
    , mFrameTimer(0.0f)
    , mCurrentFrame(0)
{
    mSprite = new SpriteComponent(this, 150, "Assets/explosion.png");

    auto audio = new AudioComponent(this);
    audio->LoadSE("death", "Assets/Audio/game_explosion7.mp3");
    audio->PlaySE("death");

    float frameWidth = mSprite->GetTexWidth() / 4.0f;
    float frameHeight = static_cast<float>(mSprite->GetTexHeight());
    mSprite->SetSpriteUV(0, 0, frameWidth, frameHeight);
}

void ExplosionParticle::UpdateActor(float deltaTime)
{
    mFrameTimer += deltaTime;
    if (mFrameTimer >= 0.08f) {
        mFrameTimer -= 0.08f;
        mCurrentFrame++;

        if (mCurrentFrame >= 4) {
            SetState(EStop);
            return;
        }

        float frameWidth = mSprite->GetTexWidth() / 4.0f;
        float frameHeight = static_cast<float>(mSprite->GetTexHeight());
        mSprite->SetSpriteUV(frameWidth * mCurrentFrame, 0, frameWidth, frameHeight);
    }
}
