#pragma once
#include "Component.h"

class HPComponent : public Component
{
public:
    HPComponent(class Actor* owner, int maxHP = 100);

    void TakeDamage(int amount);
    void Heal(int amount);

    int GetHP() const { return mCurrentHP; }
    int GetMaxHP() const { return mMaxHP; }

    void SetHP(int amount);
    void SetMaxHP(int amount);

    bool IsDead() const { return mCurrentHP <= 0; }

private:
    int mCurrentHP;
    int mMaxHP;
};
