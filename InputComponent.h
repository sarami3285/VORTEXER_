#pragma once
#include "MoveComponent.h"
#include "ActiveSkill.h"

class Game;
class Player;

class InputComponent : public MoveComponent
{
public:
    InputComponent(class Actor* owner);

    void ProcessInput(const uint8_t* keyState) override;
    void Update(float deltaTime) override;

    void SetForwardKey(uint8_t key) { mForwardKey = key; }
    void SetBackKey(uint8_t key) { mBackKey = key; }
    void SetRightKey(uint8_t key) { mRightKey = key; }
    void SetLeftKey(uint8_t key) { mLeftKey = key; }
    void SetBoostKey(uint8_t key) { mBoostKey = key; }
    void SetSkillKey(uint8_t key) { mSkillKey = key; }

    void SetSkill(ActiveSkill* s) { skill = s; }

    void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
    void SetMaxRightSpeed(float speed) { mMaxRightSpeed = speed; }

    void SetAcceleration(float accel) { mAcceleration = accel; }
    void SetDeceleration(float decel) { mDeceleration = decel; }

    float mBoostDuration;
    float mBoostTimer;

private:
    void InputClear();

    uint8_t mForwardKey;
    uint8_t mBackKey;
    uint8_t mRightKey;
    uint8_t mLeftKey;
    uint8_t mBoostKey;
    uint8_t mSkillKey;

    bool IsForward;
    bool IsBack;
    bool IsRight;
    bool IsLeft;

    float mTargetForwardSpeed;
    float mTargetRightSpeed;
    float mCurrentForwardSpeed;
    float mCurrentRightSpeed;

    float mAcceleration;
    float mDeceleration;


    bool mIsBoosting;
    float mBoostMultiplier;   
    float mBoostRecoveryRate;

    float mNormalAcceleration;
    float mBoostAcceleration; 

    ActiveSkill* skill;
    Game* game;
    Player* player;
};
