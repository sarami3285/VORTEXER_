#include "ArtilleryAttack.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Player.h"
#include "Math.h"

// 視覚的な弾丸（当たり判定なし）
class VisualShell : public Actor {
public:
    VisualShell(Game* game, const Vector2& start, const Vector2& end, float duration, ArtilleryAttack* parent)
        : Actor(game), mStart(start), mEnd(end), mDuration(duration), mTime(0.0f), mParent(parent) {
        new SpriteComponent(this, 150, "Assets/StrongBullet.png");
        SetPosition(start);
        Vector2 diff = end - start;
        SetRotation(Math::Atan2(diff.y, diff.x));
    }
    void UpdateActor(float dt) override {
        mTime += dt;
        float t = mTime / mDuration;
        if (t >= 1.0f) {
            if (mParent) mParent->OnImpact();
            SetState(EStop);
        }
        SetPosition(Vector2::Lerp(mStart, mEnd, t));
    }
private:
    Vector2 mStart, mEnd;
    float mDuration, mTime;
    ArtilleryAttack* mParent;
};

ArtilleryAttack::ArtilleryAttack(Game* game, const Vector2& startPos, const Vector2& targetPos)
    : Actor(game), mStartPos(startPos), mTargetPos(targetPos), mLifeTime(0.0f)
    , mMaxPreAttackTime(0.9f), mIsExploding(false), mExplosionTimer(0.0f), mExplosionFrame(0) {

    SetPosition(targetPos);
    // 予告円（画像左上を使用するため SourceRect を設定）
    mMarkerSprite = new SpriteComponent(this, 1, "Assets/MissileExplosion.png");
    mMarkerSprite->SetAlpha(0.0f);
    // 仮にスプライトシートが 2x2 と想定し、左上の予告マークを切り出す
    int w, h;
    SDL_QueryTexture(game->GetTexture("Assets/MissileExplosion.png"), nullptr, nullptr, &w, &h);
    mMarkerSprite->SetSpriteUV(0, 0, w / 2.0f, h / 2.0f);

    // 視覚的な弾を発射
    new VisualShell(game, startPos, targetPos, mMaxPreAttackTime, this);
}

void ArtilleryAttack::UpdateActor(float dt) {
    if (mIsExploding) {
        mExplosionTimer += dt;
        if (mExplosionTimer > 0.1f) {
            mExplosionTimer = 0.0f;
            mExplosionFrame++;
            // アニメーション切り替え（2x2シートの想定）
            int w = mMarkerSprite->GetTexWidth() / 2;
            int h = mMarkerSprite->GetTexHeight() / 2;
            int col = mExplosionFrame % 2;
            int row = mExplosionFrame / 2;
            mMarkerSprite->SetSpriteUV(
                static_cast<float>(col * w),
                static_cast<float>(row * h),
                static_cast<float>(w),
                static_cast<float>(h));

            if (mExplosionFrame > 3) SetState(EStop);
        }
    }
    else {
        mLifeTime += dt;
        // フェードイン
        float alpha = Math::Clamp(mLifeTime / mMaxPreAttackTime, 0.0f, 1.0f);
        mMarkerSprite->SetAlpha(alpha);

        // 赤線の即時描画
        GetGame()->AddImmediateDraw([this](SDL_Renderer* renderer, const Vector2& cam) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 180);
            Vector2 s = mStartPos - cam;
            Vector2 t = mTargetPos - cam;
            SDL_RenderDrawLine(renderer, (int)s.x, (int)s.y, (int)t.x, (int)t.y);
            });
    }
}

void ArtilleryAttack::OnImpact() {
    mIsExploding = true;
    mMarkerSprite->SetAlpha(1.0f);
    // 範囲ダメージ判定
    Player* p = GetGame()->GetPlayer();
    if (p) {
        float distSq = (p->GetPosition() - GetPosition()).LengthSq();
        if (distSq < 100.0f * 100.0f) { // 半径100ピクセル
            p->TakeDamage(50); // 砲撃は大ダメージ
        }
    }
}
