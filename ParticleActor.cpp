#include "ParticleActor.h"
#include "Game.h"
#include "SDL.h"

ParticleActor::ParticleActor(Game* game, const Vector2& pos, const Vector2& vel, const Vector3& color)
    : Actor(game)
    , mVelocity(vel)
    , mBaseColor(color)
    , mLifeTime(1.0f)
{
    SetPosition(pos);
}

void ParticleActor::UpdateActor(float dt)
{
    // 1. 位置の更新
    Vector2 pos = GetPosition();
    pos += mVelocity * dt;
    SetPosition(pos);

    // 2. 寿命の減少
    mLifeTime -= dt;
    if (mLifeTime <= 0.0f)
    {
        SetState(EStop); 
        return;
    }

    // 3. 描画予約
    GetGame()->AddImmediateDraw([this](SDL_Renderer* renderer, const Vector2& cameraPos) {
        this->Draw(renderer, cameraPos);
        });
}

void ParticleActor::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    Uint8 r = static_cast<Uint8>(mBaseColor.x * 255);
    Uint8 g = static_cast<Uint8>(mBaseColor.y * 255);
    Uint8 b = static_cast<Uint8>(mBaseColor.z * 255);
    Uint8 a = static_cast<Uint8>(mLifeTime * 255);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    Vector2 pos = GetPosition() - cameraPos;
    int radius = static_cast<int>(6 * mLifeTime);

    for (int w = -radius; w <= radius; w++)
    {
        for (int h = -radius; h <= radius; h++)
        {
            if (w * w + h * h <= radius * radius)
            {
                SDL_RenderDrawPoint(renderer,
                    static_cast<int>(pos.x + w),
                    static_cast<int>(pos.y + h));
            }
        }
    }
}
