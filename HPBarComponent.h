#pragma once
#include "Component.h"
#include "SDL_image.h"

class HPComponent;
class SpriteComponent;

class HPBarComponent : public Component {
public:
    HPBarComponent(Actor* owner, HPComponent* hpComp);
    ~HPBarComponent();

    void Update(float deltaTime) override;
    void DrawHPBar(SDL_Renderer* renderer);

private:
    HPComponent* mHP;
    Actor* mHPBarActor;
    SpriteComponent* mBarSprite;
    SpriteComponent* mBackgroundSprite;
};
