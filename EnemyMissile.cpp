#include "EnemyMissile.h"
#include "Game.h"
#include "Player.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "Math.h"
#include "DeathEffect.h"

EnemyMissile::EnemyMissile(Game* game, const Vector2& startPos, float rotation, float speed, float lifeTime, int damage)
    : EnemyBullet(game, startPos, rotation, speed, lifeTime, damage)
{
    mTarget = game->GetPlayer();

    this->mTurnSpeed = Math::TwoPi * 0.15f;

    MoveComponent* mc = GetComponent<MoveComponent>();
    if (mc) {
        mc->SetForwardSpeed(speed);
    }

    SpriteComponent* sc = GetComponent<SpriteComponent>();

    if (sc)
    {
        sc->SetTexture(game->GetTexture("Assets/Missile.png"));
        sc->SetSpriteUV(0, 0, mFrameWidth, static_cast<float>(sc->GetTexHeight()));
        sc->SetColor(1.0f, 1.0f, 1.0f);
        sc->SetCastShadow(true);
    }
}

void EnemyMissile::UpdateActor(float deltaTime)
{
    EnemyBullet::UpdateActor(deltaTime);

    mAnimTimer += deltaTime;
    if (mAnimTimer >= (1.0f / mAnimFPS))
    {
        mAnimTimer -= (1.0f / mAnimFPS);

        mCurrentFrame++;
        if (mCurrentFrame >= mNumFrames)
        {
            mCurrentFrame = 0;
        }

        SpriteComponent* sc = GetComponent<SpriteComponent>();
        if (sc)
        {
            sc->SetSpriteUV(static_cast<float>(mCurrentFrame * mFrameWidth),
                0,
                static_cast<float>(mFrameWidth),
                static_cast<float>(sc->GetTexHeight()));
        }
    }

    if (GetState() != EStop && mTarget)
    {
        Vector2 toTarget = mTarget->GetPosition() - GetPosition();
        float targetAngle = Math::Atan2(toTarget.y, toTarget.x);

        float currentAngle = GetRotation();
        float diff = Math::WrapAngle(targetAngle - currentAngle);

        float turnAmount = mTurnSpeed * deltaTime;
        if (Math::Abs(diff) < turnAmount)
        {
            SetRotation(targetAngle);
        }
        else
        {
            SetRotation(currentAngle + Math::Sign(diff) * turnAmount);
        }

        if (Intersect(*mCollision, *(mTarget->GetCircle())))
        {
            Explode();
            SetState(EStop);
        }
    }
    else {
        DeathEffect* effect = new DeathEffect(GetGame(), 1);
        effect->SetPosition(GetPosition());
        effect->SetScale(0.7f);
    }
}

void EnemyMissile::Explode()
{
    Player* player = GetGame()->GetPlayer();

    if (!player) return;

    Vector2 toPlayer = player->GetPosition() - GetPosition();

    if (toPlayer.LengthSq() <= mExplosionRadius * mExplosionRadius)
    {
        player->TakeDamage(Damage);
    }
}
