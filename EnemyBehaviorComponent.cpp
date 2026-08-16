#include "EnemyBehaviorComponent.h"
#include "Actor.h"

EnemyBehaviorComponent::EnemyBehaviorComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
{
    // いまんとこ空
}

EnemyBehaviorComponent::~EnemyBehaviorComponent() {
    
}
