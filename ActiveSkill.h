#pragma once
#include "Component.h"
#include <string>
#include "Actor.h"

class ActiveSkill : public Component
{
public:
    ActiveSkill(Actor* owner, const std::string& skillName, float cooldown, int updateOrder = 50)
        : Component(owner, updateOrder)
        , mSkillName(skillName)
        , mCooldownDuration(cooldown)
        , mCooldownTimer(0.0f)
    {
    }

    virtual ~ActiveSkill() {}

    void Update(float deltaTime) override
    {
        if (mCooldownTimer > 0.0f) {
            mCooldownTimer -= deltaTime;
        }
    }

    virtual void ExecuteSkill() = 0;

    bool TryActivate()
    {
        if (mCooldownTimer <= 0.0f)
        {
            ExecuteSkill();
            mCooldownTimer = mCooldownDuration;
            return true;
        }
        return false;
    }

    float GetCooldownRemaining() const { return mCooldownTimer; }
    const std::string& GetSkillName() const { return mSkillName; }

protected:
    std::string mSkillName;
    float mCooldownDuration;
    float mCooldownTimer;
};
