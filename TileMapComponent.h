#pragma once
#include "SpriteComponent.h"
#include <vector>
#include "SDL.h"
#include "Math.h"

class TileMapComponent : public SpriteComponent
{
public:
    TileMapComponent(class Actor* owner, int drawOrder = 0);
    ~TileMapComponent();

    void SetTileSet(SDL_Texture* tileSet, int tileSize, int tilesPerRow);
    void SetMapData(const std::vector<std::vector<int>>& mapData);

    virtual void Draw(SDL_Renderer* renderer, const Vector2& cameraPos) override;

    void SetIsAnimated(bool isAnim) { mIsAnimated = isAnim; }
    bool IsAnimated() const { return mIsAnimated; }

    void SetAnimationRows(int rows) { mAnimationRows = rows; }
    int GetAnimationRows() const { return mAnimationRows; }

    void SetAnimationFrame(int frameIndex);

    int mPrevFrame;
    float mTimeSinceLastSwitch;
    const float FADE_DURATION = 1.0f;
    void UpdateFade(float deltaTime);

private:
    bool mIsAnimated;
    int mAnimationRows;
    int mCurrentFrame;
    SDL_Texture* mTileSet;
    int mTileSize;
    int mTilesPerRow;
    std::vector<std::vector<int>> mMapData;
};





