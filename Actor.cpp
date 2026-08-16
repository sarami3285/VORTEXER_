#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include "LineComponent.h" 
#include "SpriteComponent.h" 
#include <algorithm>
#include "Math.h"

Actor::Actor(Game* game)
	:mState(EAlive)
	, mPosition(Vector2::Zero)
	, mScale(1.0f)
	, mRotation(0.0f)
	, mGame(game)
{
	mGame->AddActor(this);
}

// Components are exclusively owned by their actor.
Actor::~Actor() {
	while (!mComponents.empty()) {
		Component* c = mComponents.back();
		mComponents.pop_back();
		delete c;
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == EAlive || mState == State::EStoping)
	{
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		if (comp) {
			comp->Update(deltaTime);
		}
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::AddComponent(Component* component)
{
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (;
		iter != mComponents.end();
		++iter)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}

	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

void Actor::ProcessInput(const uint8_t* keyState) {
	if (mState == EAlive) {
		for (auto comp : mComponents) {
			if (comp) {
				comp->ProcessInput(keyState);
			}
		}
		ActorInput(keyState);
	}
}

void Actor::ActorInput(const uint8_t* keyState)
{
}
