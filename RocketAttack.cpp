#include "RocketAttack.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Player.h"
#include "Math.h"

class VisualMissile : public Actor {
public:
    VisualMissile(Game* game, const Vector2& start, const Vector2& end, float duration, RocketAttack* parent)
        : Actor(game), mStart(start), mEnd(end), mDuration(duration), mTime(0.0f), mParent(parent) {
        SpriteComponent* sc = new SpriteComponent(this, 150, "Assets/SkillMissile.png");
        SetPosition(start);
        Vector2 diff = end - start;
        SetRotation(Math::Atan2(diff.y, diff.x));
    }

    void UpdateActor(float dt) override {
        Actor::UpdateActor(dt);
        mTime += dt;
        float t = mTime / mDuration;
        if (t >= 1.0f) {
            t = 1.0f;
            if (mParent) mParent->OnMissileImpact();
            SetState(State::EStop); // 完了したら停止
        }
        SetPosition(Vector2::Lerp(mStart, mEnd, t));
    }
private:
    Vector2 mStart, mEnd;
    float mDuration, mTime;
    RocketAttack* mParent;
};

RocketAttack::RocketAttack(Game* game, const Vector2& startPos, const Vector2& targetPos)
    : Actor(game)
    , mStartPos(startPos)
    , mTargetPos(targetPos)
    , mTime(0.0f)
    , mDuration(2.0f)
    , mIsExploding(false)
    , mExplosionTimer(0.0f)
    , mExplosionFrame(0)
{
    SetPosition(startPos);
    mCircleSprite = new SpriteComponent(this, 1, "Assets/MissileExplosion.png");
    int w = mCircleSprite->GetTexWidth() / 2;
    int h = mCircleSprite->GetTexHeight() / 2;
    SDL_Rect initialRect = { 0, 0, w, h };
    mCircleSprite->SetSourceRect(initialRect);
    mCircleSprite->SetAlpha(0.0f);

    mCollision = new CollisionComponent(this, 80.0f);
    mMissileActor = new VisualMissile(game, startPos, targetPos, mDuration, this);
}

void RocketAttack::UpdateActor(float dt) {
    Actor::UpdateActor(dt);

    if (mIsExploding) {
        if (mExplosionFrame == 1) {
            GetGame()->GetCamera()->Shake(10.0f, 0.5f);
        }
        mCircleSprite->SetAlpha(1.0f);
        SetPosition(mTargetPos);

        mExplosionTimer += dt;
        if (mExplosionTimer >= 0.1f) {
            mExplosionTimer = 0.0f;
            mExplosionFrame++;

            // アニメーション更新（1～3フレーム）
            if (mExplosionFrame <= 3) {
                int w = mCircleSprite->GetTexWidth() / 2;
                int h = mCircleSprite->GetTexHeight() / 2;
                int col = mExplosionFrame % 2;
                int row = mExplosionFrame / 2;
                SDL_Rect rect = { col * w, row * h, w, h };
                mCircleSprite->SetSourceRect(rect);
            }

            // 全フレーム終了で消去
            if (mExplosionFrame > 3) {
                SetState(State::EStop);
            }
        }
    }
    else {
        mTime += dt;
        float tCircle = Math::Clamp(mTime / (mDuration * 0.5f), 0.0f, 1.0f);
        float alpha = Math::Clamp((tCircle - 0.2f) / 0.3f, 0.0f, 1.0f);
        mCircleSprite->SetAlpha(alpha);

        Vector2 currentCirclePos = Vector2::Lerp(mStartPos, mTargetPos, tCircle);
        SetPosition(currentCirclePos);
    }
}

void RocketAttack::OnMissileImpact() {
    mIsExploding = true;
    mExplosionFrame = 1; // 爆発の最初のコマを表示
    mExplosionTimer = 0.0f;
    SetPosition(mTargetPos);

    // スプライトを爆発の最初のコマに更新
    int w = mCircleSprite->GetTexWidth() / 2;
    int h = mCircleSprite->GetTexHeight() / 2;
    SDL_Rect rect = { 1 * w, 0 * h, w, h }; // 右上（爆発開始）
    mCircleSprite->SetSourceRect(rect);

    // 【重要】着弾した瞬間にダメージ判定を行う
    Player* p = GetGame()->GetPlayer();
    if (p) {
        // Intersectに渡す前に自身の座標を確実に更新
        if (Intersect(*mCollision, *(p->GetCircle()))) {
            p->TakeDamage(35);
            SDL_Log("Rocket HIT!"); // デバッグログ
        }
    }
}
