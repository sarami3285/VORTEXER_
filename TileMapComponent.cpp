#include "TileMapComponent.h"
#include "Actor.h"
#include "SDL.h"
#include "Game.h" 
#include "Math.h"

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
    : SpriteComponent(owner, drawOrder)
    , mTileSet(nullptr)
    , mTileSize(32)
    , mTilesPerRow(4)
    , mIsAnimated(false)
    , mAnimationRows(1)
    , mCurrentFrame(0)
    , mPrevFrame(0)            
    , mTimeSinceLastSwitch(0.0f) 
{
}

TileMapComponent::~TileMapComponent()
{
}

void TileMapComponent::SetTileSet(SDL_Texture* tileSet, int tileSize, int tilesPerRow)
{
    mTileSet = tileSet;
    mTileSize = tileSize;
    mTilesPerRow = tilesPerRow;
}

void TileMapComponent::SetMapData(const std::vector<std::vector<int>>& mapData)
{
    mMapData = mapData;
}

void TileMapComponent::UpdateFade(float deltaTime)
{
    if (mIsAnimated)
    {
        mTimeSinceLastSwitch += deltaTime;
        mTimeSinceLastSwitch = Math::Min(mTimeSinceLastSwitch, FADE_DURATION);
    }
}

void TileMapComponent::SetAnimationFrame(int frameIndex)
{
    if (mCurrentFrame != frameIndex)
    {
        mPrevFrame = mCurrentFrame;
        mCurrentFrame = frameIndex;
        mTimeSinceLastSwitch = 0.0f;
    }
}


void TileMapComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    if (!mTileSet) return;
    if (mMapData.empty()) return;
    Uint8 currentAlphaMod = 255;

    if (mIsAnimated && mAnimationRows > 1)
    {
        float progress = mTimeSinceLastSwitch / FADE_DURATION;
        float alpha = Math::Clamp(progress, 0.0f, 1.0f);
        currentAlphaMod = static_cast<Uint8>(alpha * 255.0f); 
    }

    Uint8 originalAlpha;
    SDL_GetTextureAlphaMod(mTileSet, &originalAlpha);

    int rows = static_cast<int>(mMapData.size());
    int cols = static_cast<int>(mMapData[0].size());

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            int tileIndex = mMapData[y][x];
            if (tileIndex < 0) continue;

            int tu = tileIndex % mTilesPerRow;
            int tv_base = tileIndex / mTilesPerRow;

            SDL_Rect dstRect = {
                static_cast<int>(x * mTileSize - cameraPos.x),
                static_cast<int>(y * mTileSize - cameraPos.y),
                mTileSize,
                mTileSize
            };

            if (mIsAnimated && tileIndex == 0 && mAnimationRows > 1)
            {
                // --- A. 背景フレームの描画---
                int tv_prev = mPrevFrame;
                SDL_Rect srcRect_prev = { tu * mTileSize, tv_prev * mTileSize, mTileSize, mTileSize };
                SDL_RenderCopy(renderer, mTileSet, &srcRect_prev, &dstRect);

                // --- B. 前景フレームの描画  ---
                int tv_current = mCurrentFrame;
                SDL_Rect srcRect_current = { tu * mTileSize, tv_current * mTileSize, mTileSize, mTileSize };

                SDL_SetTextureAlphaMod(mTileSet, currentAlphaMod);
                SDL_RenderCopy(renderer, mTileSet, &srcRect_current, &dstRect);
                SDL_SetTextureAlphaMod(mTileSet, originalAlpha);

            }
            else
            {
                int tv = tv_base;
                SDL_Rect srcRect = { tu * mTileSize, tv * mTileSize, mTileSize, mTileSize };
                SDL_RenderCopy(renderer, mTileSet, &srcRect, &dstRect);
            }
        }
    }
}
