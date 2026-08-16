#pragma once
#include "Actor.h"
#include "AudioComponent.h"
#include "string"

class EnemyBullet : public Actor {
public:
	EnemyBullet(class Game* game, const Vector2& startPos, float rotation, float speed, float lifeTime, int damage, const std::string& texturePath = "Assets/Bullet.png");
	void UpdateActor(float deltaTime) override;

	int Damage;
	class CollisionComponent* mCollision;

private:
	AudioComponent* mAudioComponent;
	float mDeathTimer;
	std::string mBulletPath = "Assets/Bullet.png";
};
