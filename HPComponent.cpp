#include "HPComponent.h"
#include "Actor.h"

HPComponent::HPComponent(class Actor*owner,int maxHP)
	:Component(owner)
	, mMaxHP(maxHP)
	, mCurrentHP(maxHP)
{

}

void HPComponent::TakeDamage(int amount) {
	mCurrentHP -= amount;
	if (mCurrentHP < 0)
	{
		mCurrentHP = 0;
	}
}

void HPComponent::Heal(int amount) {
	mCurrentHP += amount;
	if (mCurrentHP > mMaxHP)
	{
		mCurrentHP = mMaxHP;
	}
}

void HPComponent::SetHP(int amount) {
	mCurrentHP = amount;
}

void HPComponent::SetMaxHP(int amount) {
	mMaxHP = amount;
}
