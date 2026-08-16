#pragma once
#include "Actor.h"
#include <string>

class DeathEnemy : public Actor
{
public:
    DeathEnemy(class Game* game, const Vector2& pos,float rotation, const std::string& texturePath,int ExpFRAGMENTS,const Vector2& velocity , float Speed);

    void UpdateActor(float deltaTime) override;

private:
    void ExplodeIntoFragments();

    Vector2 mVelocity;
    float mFriction;
    std::string mTexPath;
    class SpriteComponent* mSprite;
    float red = 255;
    int Exp = 3;
    float sp = 0;
    float mLifeTime = 1.0f;
};
