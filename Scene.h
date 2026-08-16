#pragma once
#include <SDL_stdinc.h>
#include <SDL.h>
#include <memory>

class Scene {
public:
    virtual ~Scene() = default;

    virtual void ProcessEvent(const SDL_Event& e) {}
    virtual void ProcessInput(const Uint8* keyState) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(SDL_Renderer* renderer) = 0;
    virtual bool IsFinished() const { return false; }
    virtual std::unique_ptr<Scene> NextScene() { return nullptr; }

    virtual void LoadContent() {}
    virtual void LoadData() {}
};
