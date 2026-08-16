// SkillComponent.cpp
#include "SkillComponent.h"
#include "ActiveSkill.h" 
#include "PassiveSkill.h"
#include "SDL.h" 
#include "Actor.h"

SkillComponent::SkillComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
{
}

SkillComponent::~SkillComponent()
{
    
}

void SkillComponent::ProcessInput(const uint8_t* keyState)
{
    
}

void SkillComponent::AddActiveSkill(ActiveSkill* skill)
{
    mActiveSkills.push_back(skill);
}

void SkillComponent::AddPassiveSkill(PassiveSkill* skill)
{
    skill->ApplyInitialEffect();
    mPassiveSkills.push_back(skill);
}
