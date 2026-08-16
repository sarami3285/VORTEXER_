#pragma once
#include "Actor.h"
#include "MiniMapComponent.h"
#include "MiniMapSpriteComponent.h"

class HUDActor : public Actor {
public:
    HUDActor(Game* game)
        : Actor(game)
    {
        SetState(Actor::EAlive);
        mMiniMapLogic = new MiniMapComponent(this);
        mMiniMapDraw = new MiniMapSpriteComponent(this, mMiniMapLogic);
        mMiniMapLogic->SetRadius(94.0f);
        mMiniMapLogic->SetVisibleRange(1500.0f);
        mMiniMapDraw->SetOffset(Vector2(880.0f, 658.0f));
    }



    MiniMapComponent* GetMiniMapLogic() const { return mMiniMapLogic; }
    MiniMapSpriteComponent* GetMiniMapDraw() const { return mMiniMapDraw; }

private:
    MiniMapComponent* mMiniMapLogic;
    MiniMapSpriteComponent* mMiniMapDraw;
};
