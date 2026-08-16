#pragma once
#include "Actor.h"
#include "Math.h"

class ParticleActor : public Actor {
public:
    ParticleActor(class Game* game, const Vector2& pos, const Vector2& vel, const Vector3& color);
    void UpdateActor(float dt) override;
    void Draw(struct SDL_Renderer* renderer, const Vector2& cameraPos);

private:
    Vector2 mVelocity;
    Vector3 mBaseColor;
    float mLifeTime;
};
