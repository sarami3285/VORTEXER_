#pragma once
#include "Component.h"
#include <string>
#include "Actor.h" 

class PassiveSkill : public Component
{
public:
    PassiveSkill(Actor* owner, const std::string& skillName, int updateOrder = 50)
        : Component(owner, updateOrder)
        , mSkillName(skillName)
    {
    }

    virtual ~PassiveSkill() {}
    virtual void ApplyInitialEffect() = 0;

    const std::string& GetSkillName() const { return mSkillName; }

protected:
    std::string mSkillName;
};
