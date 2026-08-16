#include "TargetIndicatorComponent.h"
#include "Actor.h"
#include "Game.h"
#include "TargetActor.h"
#include <SDL.h>

TargetIndicatorComponent::TargetIndicatorComponent(Actor* owner)
    : Component(owner) {
}

void TargetIndicatorComponent::Draw(SDL_Renderer* renderer) {
    Game* game = mOwner->GetGame();
    if (game->GetMissionData().targetType != "Destruction") return;

    Vector2 camPos = game->GetCameraPos();
    const float margin = 30.0f;
    const float screenW = 1024.0f;
    const float screenH = 768.0f;

    for (auto target : game->GetTargetActors()) {
        if (!target || target->GetState() != Actor::EAlive) continue;

        Vector2 targetScreenPos = target->GetPosition() - camPos;
        bool isOffScreen = (targetScreenPos.x < 0 || targetScreenPos.x > screenW ||
            targetScreenPos.y < 0 || targetScreenPos.y > screenH);

        if (isOffScreen) {
            Vector2 edgePos;
            edgePos.x = Math::Clamp(targetScreenPos.x, margin, screenW - margin);
            edgePos.y = Math::Clamp(targetScreenPos.y, margin, screenH - margin);

            Vector2 dir = edgePos - Vector2(screenW / 2.0f, screenH / 2.0f);
            dir.Normalize();

            Vector2 v1 = edgePos;
            Vector2 sideDir(-dir.y, dir.x);
            Vector2 v2 = edgePos - dir * 25.0f + sideDir * 12.0f;
            Vector2 v3 = edgePos - dir * 25.0f - sideDir * 12.0f;

            SDL_Vertex vertices[3];
            SDL_Color color = { 255, 0, 0, 255 };
            vertices[0] = { { v1.x, v1.y }, color, { 0.0f, 0.0f } };
            vertices[1] = { { v2.x, v2.y }, color, { 0.0f, 0.0f } };
            vertices[2] = { { v3.x, v3.y }, color, { 0.0f, 0.0f } };

            SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
        }
    }
}
