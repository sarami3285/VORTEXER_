#pragma once
#include "HUDSpriteComponent.h"

class MiniMapComponent;

class MiniMapSpriteComponent : public HUDSpriteComponent {
public:
    MiniMapSpriteComponent(Actor* owner, MiniMapComponent* logic, int drawOrder = 9000);
    void Draw(SDL_Renderer* renderer, const Vector2& cameraPos) override;

private:
    MiniMapComponent* mLogic;
};

