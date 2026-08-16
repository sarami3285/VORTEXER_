#pragma once
#include "Component.h"
#include "Math.h"

class TargetIndicatorComponent : public Component {
public:
    TargetIndicatorComponent(class Actor* owner);
    // Gameクラスの描画ループ（ImmediateDrawなど）から呼ばれることを想定
    void Draw(struct SDL_Renderer* renderer);
};
