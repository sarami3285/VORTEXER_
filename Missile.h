#pragma once
#include "Bullet.h"
#include "SpriteComponent.h"

class Missile : public Bullet
{
public:
    Missile(class Game* game);
    void UpdateActor(float deltaTime) override;
    void SetTarget(Actor* target);

    void* operator new(size_t size) { return ::operator new(size); }
    void operator delete(void* p) { ::operator delete(p); }


private:
    Actor* mTarget = nullptr;
};
