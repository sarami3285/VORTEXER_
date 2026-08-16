#pragma once
#include "Component.h"
#include "Actor.h"

class EnemyStateComponent;

// 敵の振る舞いコンポーネントの基底クラス
class EnemyBehaviorComponent : public Component
{
public:
    EnemyBehaviorComponent(Actor* owner, int updateOrder = 10);
    ~EnemyBehaviorComponent();
    void Update(float dt) override {
        if (mIsEnabled) { 
            Execute(dt);
        }
    }
    virtual void Execute(float dt) = 0;
};
