#include "MetalFragment.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "MoveComponent.h"
#include "Random.h"
#include "CameraComponent.h" 

constexpr float FRAGMENT_LIFETIME = 15.0f;
constexpr float INITIAL_SPEED_MIN = 100.0f;
constexpr float INITIAL_SPEED_MAX = 300.0f;
constexpr float COLLISION_RADIUS = 20.0f;
constexpr float DRAG_FACTOR = 0.9f;
constexpr int GRID_SIZE = 2; // スプライトシートのグリッド (2x2)

MetalFragment::MetalFragment(Game* game, const Vector2& startPos)
    : Actor(game)
    , mTimeToLive(FRAGMENT_LIFETIME)
    , mExperienceValue(Random::GetIntRange(5, 10))
{
    SetPosition(startPos);

    mSprite = new SpriteComponent(this, 1 , "Assets/Fragment.png" , false , Vector2{0,0});

    mCollision = new CollisionComponent(this,COLLISION_RADIUS);

    mMove = new MoveComponent(this);
    mMove->SetForwardSpeed(0.0f);
    mMove->SetMaxSpeed(INITIAL_SPEED_MAX);

    float texW = static_cast<float>(mSprite->GetTexWidth());
    float texH = static_cast<float>(mSprite->GetTexHeight());

    float fragW = texW / static_cast<float>(GRID_SIZE);
    float fragH = texH / static_cast<float>(GRID_SIZE);

    // ランダムなグリッド座標 (0, 1) を取得
    int randCol = Random::GetIntRange(0, GRID_SIZE - 1); // 0 or 1
    int randRow = Random::GetIntRange(0, GRID_SIZE - 1); // 0 or 1

    float u_start = randCol * fragW;
    float v_start = randRow * fragH;

    mSprite->SetSpriteUV(u_start, v_start, fragW, fragH);
   
    float randomRotation = Random::GetFloatRange(0.0f, Math::TwoPi);
    SetRotation(randomRotation);

    float initialSpeed = Random::GetFloatRange(INITIAL_SPEED_MIN, INITIAL_SPEED_MAX);
    mMove->SetForwardSpeed(initialSpeed);

    SetScale(0.5f);
}

MetalFragment::~MetalFragment()
{
}

void MetalFragment::UpdateActor(float deltaTime)
{
    mTimeToLive -= deltaTime;
    if (mTimeToLive <= 0.0f)
    {
        SetState(EStop);
        return;
    }

    float currentSpeed = mMove->GetForwardSpeed();
    if (currentSpeed > 0.0f)
    {
        float newSpeed = currentSpeed - DRAG_FACTOR * currentSpeed * deltaTime;
        mMove->SetForwardSpeed(Math::Max(0.0f, newSpeed));
    }

    Player* player = GetGame()->GetPlayer();
    Vector2 playerPos = player->GetPosition();
    Vector2 fragmentPos = GetPosition();

    Vector2 diff = playerPos - fragmentPos;
    float distSq = diff.LengthSq();

    if (distSq < SUCTION_RADIUS_SQ)
    {
        Vector2 direction = Vector2::Normalize(diff);
        float speed = mMove->GetForwardSpeed();
        mMove->SetForwardSpeed(speed + SUCTION_ACCELERATION * deltaTime);
        float targetAngle = Math::Atan2(direction.y, direction.x);
        SetRotation(targetAngle);
    }

    Vector2 pos = GetPosition();
    float margin = 20.0f;
    if (pos.x < -margin || pos.x > CameraComponent::mMapWidth + margin ||
        pos.y < -margin || pos.y > CameraComponent::mMapHeight + margin)
    {
        SetState(EStop);
    }
}
