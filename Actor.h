#pragma once
#include <vector>
#include "Math.h"
#include <cstdint>
#include <string>
#include "Component.h" 

class CollisionComponent;
class Vector2;

class Actor
{
public:
	enum State {
		EAlive,
		EPaused,
		EStoping,
		EStop
	};

	Actor(class Game* game);
	virtual ~Actor();
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);

	virtual void TakeDamage(float amount, const Vector2& bulletDir) {}
	virtual class CollisionComponent* GetCircle() const{ return nullptr; }

	const std::vector<class Component*>& GetComponents() const { return mComponents; }
	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	void SetTag(const std::string& tag) { mTag = tag; }
	const std::string& GetTag() const { return mTag; }

	virtual void OnCollision(Actor* other) {}

	class Game* GetGame() { return mGame; }

	template <typename T>
	T* GetComponent() const
	{
		for (class Component* comp : mComponents)
		{
			T* specificComp = dynamic_cast<T*>(comp);
			if (specificComp)
			{
				return specificComp;
			}
		}
		return nullptr;
	}

	Vector2 GetForward() const {
		return Vector2(Math::Cos(mRotation), Math::Sin(mRotation));
	}
	Vector2 GetRight() const {
		return Vector2(-Math::Sin(mRotation), Math::Cos(mRotation));
	}

	void ProcessInput(const uint8_t* keyState);
	virtual void ActorInput(const uint8_t* keyState);

protected:
	std::vector<class Component*> mComponents;

private:
	std::string mTag;

	State mState;
	Vector2 mPosition;
	float mScale;
	float mRotation;

	class Game* mGame;
};
