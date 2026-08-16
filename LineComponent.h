#pragma once
#include "Component.h"
#include "Math.h"
#include <SDL.h>

class LineComponent : public Component {
public:
    LineComponent(class Actor* owner, int drawOrder = 90);
    ~LineComponent();

    void Draw(SDL_Renderer* renderer, const Vector2& cameraPos);

    void SetTargetPos(const Vector2& pos) { mTargetPos = pos; }
    void SetColor(Uint8 r, Uint8 g, Uint8 b) { mColorR = r; mColorG = g; mColorB = b; }
    void SetVisibility(bool visible) { mIsVisible = visible; }
    bool GetVisibility() const { return mIsVisible; }

private:
    Vector2 mTargetPos;
    Uint8 mColorR, mColorG, mColorB;
    int mDrawOrder;
    bool mIsVisible = false;
};
