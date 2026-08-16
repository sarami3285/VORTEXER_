#include "DeathEnemy.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Random.h"
#include "EnemyFragment.h"
#include "Math.h"
#include "MetalFragment.h"
#include "ParticleActor.h"
#include "DeathEffect.h"

DeathEnemy::DeathEnemy(Game* game, const Vector2& pos,float rotation, const std::string& texturePath,int ExpFRAGMENTS, const Vector2& velocity , float Speed)
    : Actor(game)
    , mVelocity(velocity)
    , mFriction(0.96f)
    , mTexPath(texturePath)
{
    SetPosition(pos);
    SetRotation(rotation);
    mSprite = new SpriteComponent(this, 0, texturePath , true , Vector2{2,2});
    sp = Speed;
    Exp = ExpFRAGMENTS;
}

void DeathEnemy::UpdateActor(float deltaTime)
{
    Vector2 pos = GetPosition();
    pos += mVelocity * deltaTime;
    SetPosition(pos);
    mVelocity *= mFriction;

    if (mLifeTime > 0.0f) {
        mLifeTime -= deltaTime / 0.5f;
    }

    if (mSprite) {
        mSprite->SetColor(1.0f, mLifeTime, mLifeTime);
    }

    if (mVelocity.LengthSq() < 1000.0f || mLifeTime <= 0.0f)
    {
        ExplodeIntoFragments();
        SetState(EStop);
    }
}
void DeathEnemy::ExplodeIntoFragments()
{
    for (int i = 0; i < 20; ++i) {
        Vector2 pVel = Vector2(Random::GetFloatRange(-1.0f, 1.0f), Random::GetFloatRange(-1.0f, 1.0f));
        pVel.Normalize();
        float speed = Random::GetFloatRange(sp/12, sp);
        Vector3 pColor = Vector3(1.0f, (speed / 600.0f), 0.0f);
        new ParticleActor(GetGame(), GetPosition(), pVel * speed, pColor);
    }

    for (int i = 0; i < Exp; ++i)
    {
        MetalFragment* fragment = new MetalFragment(GetGame(), GetPosition());
        GetGame()->AddActor(fragment);
    }

    DeathEffect* deathEffect = new DeathEffect(GetGame(),3);
    deathEffect->SetPosition(GetPosition());


    const float frameW = 64.0f;
    const float frameH = 64.0f;
    const float chunkW = frameW / 2.0f;
    const float chunkH = frameH / 2.0f;

    for (int y = 0; y < 2; ++y)
    {
        for (int x = 0; x < 2; ++x)
        {
            Vector2 fragVel = Vector2(Random::GetFloatRange(-100.0f, 100.0f), Random::GetFloatRange(-100.0f, 100.0f));
            EnemyFragment* frag = new EnemyFragment(GetGame(), GetPosition(), fragVel);
            SpriteComponent* sc = frag->GetComponent<SpriteComponent>();
            if (sc)
            {
                sc->SetSpriteUV(x * chunkW, y * chunkH, chunkW, chunkH);
            }
        }
    }
}
