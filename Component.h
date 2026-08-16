#pragma once
#include <cstdint>
#include "SDL.h" 
#include "Math.h"

class Actor;

class Component
{
public:
    Component(class Actor* owner, int updateOrder = 100);
    virtual ~Component();
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    virtual void Update(float deltaTime);
    virtual void ProcessInput(const uint8_t* keyState) {}
    virtual void OnUpdateWorldTransform() {}
    virtual void OnDamageTaken(float& damageAmount, const class Actor* damageSource, const Vector2& bulletDir) {}
    class Actor* GetOwner() { return mOwner; }
    int GetUpdateOrder() const { return mUpdateOrder; }

    virtual void SetIsActive(bool active) { mIsEnabled = active; }
    bool IsActive() const { return mIsEnabled; }

    virtual void Draw(SDL_Renderer* renderer, const Vector2& cameraPos) {}
    virtual void ProcessEvent(const SDL_Event& event) {}

protected:
    class Actor* mOwner;
    int mUpdateOrder;
    bool mIsEnabled = true;
};
