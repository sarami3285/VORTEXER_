#pragma once
#include "Component.h"
#include <vector>

class ActiveSkill;
class PassiveSkill;

class SkillComponent : public Component
{
public:
    SkillComponent(class Actor* owner, int updateOrder = 1);
    ~SkillComponent();
    void ProcessInput(const uint8_t* keyState) override;
    void AddActiveSkill(ActiveSkill* skill);
    void AddPassiveSkill(PassiveSkill* skill);

private:
    std::vector<ActiveSkill*> mActiveSkills;
    std::vector<PassiveSkill*> mPassiveSkills;
};
