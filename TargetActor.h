#pragma once
#include "Actor.h"
#include "Enemy.h"
#include "HPComponent.h"
class CollisionComponent;

class TargetActor : public Enemy {
public:
    TargetActor(class Game* game);
    ~TargetActor();
    void UpdateActor(float deltaTime) override;
    void TakeDamage(float amount, const Vector2& bulletDir) override;
    class CollisionComponent* GetCircle() const override;
    class HPComponent* GetHPComponent() const;

private:
};
