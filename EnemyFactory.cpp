#include "EnemyFactory.h"
#include "Game.h"
#include "Enemy.h"
#include "LowLevelEnemy.h"
#include "DroneGuard.h"
#include "SentryGun.h"
#include "EnemyBase.h"
#include "RocketTurret.h"
#include "BattleShip.h"
#include "Interceptor.h"
#include "Boss_Tank.h"
#include "Boss_Heri.h"
#include "HUDSpriteComponent.h"
#include "PatrolComponent.h"

EnemyFactory::EnemyFactory(Game* game)
    : mGame(game) {
}

Enemy* EnemyFactory::CreateEnemy(const EnemyConfig& config, Vector2 pos) {
    Enemy* enemy = nullptr;

    if (config.type == "LowLevelEnemy") {
        enemy = new LowLevelEnemy(mGame);
        enemy->SetPosition(pos);
    }
    else if (config.type == "DroneGuard") {
        enemy = new DroneGuard(mGame);
        enemy->SetPosition(pos);
    }
    else if (config.type == "SentryGun") {
        enemy = new SentryGun(mGame);
        enemy->SetPosition(pos);
    }
    else if (config.type == "EnemyBase") {
        for (const auto& p : config.fixedPositions) {
            EnemyBase* eb = new EnemyBase(mGame);
            eb->SetPosition(p);
        }
    }
    else if (config.type == "RocketTurret") {
        for (const auto& p : config.fixedPositions) {
            RocketTurret* eb = new RocketTurret(mGame);
            eb->SetPosition(p);
        }
    }
    else if (config.type == "BattleShip") {
        enemy = new BattleShip(mGame, pos);
    }
    else if (config.type == "Interceptor") {
        enemy = new Interceptor(mGame);
    }
    else if (config.type == "Boss_Tank") {
        enemy = new Boss_Tank(mGame);
        enemy->SetPosition(pos);
    }
    else if (config.type == "Boss_Heri") {
        enemy = new Boss_Heri(mGame);
        enemy->SetPosition(pos);
    }
    else if (config.type == "Formation") {
        Enemy* flagship = nullptr;
        Vector2 centerPos = config.centerPosition;

        if (config.mainShipType == "LowLevelEnemy") {
            flagship = new LowLevelEnemy(mGame);
        }
        else if (config.mainShipType == "DroneGuard") {
            flagship = new DroneGuard(mGame);
        }
        else if (config.mainShipType == "SentryGun") {
            flagship = new SentryGun(mGame);
        }
        else if (config.mainShipType == "BattleShip") {
            flagship = new BattleShip(mGame, centerPos);
        }

        if (flagship) {
            if (config.mainShipType != "BattleShip") {
                flagship->SetPosition(centerPos);
            }
            if (mGame->mHUDActor != nullptr) {
                HUDSpriteComponent* hpBar = new HUDSpriteComponent(mGame->mHUDActor);
                hpBar->SetTexture(mGame->GetTexture("Assets/HP1.png"));
                hpBar->SetOffset(Vector2(0, -50.0f));
                hpBar->SetTargetActor(flagship);
            }
        }

        if (!flagship) {
            SDL_Log("ERROR: Flagship type '%s' is unknown or failed to spawn.", config.mainShipType.c_str());
            return nullptr;
        }

        float rx = config.radiusX;
        float ry = config.radiusY;
        float formationRot = config.formationRotation;

        for (const auto& detail : config.guardDetails) {
            int guardCount = detail.guardCount;
            const std::string& guardShipType = detail.guardShipType;
            float radiusRatio = detail.radiusRatio;

            float currentRx = rx * radiusRatio;
            float currentRy = ry * radiusRatio;

            for (int i = 0; i < guardCount; i++) {
                float angle = Math::TwoPi * (float)i / guardCount;

                float x = currentRx * Math::Cos(angle);
                float y = currentRy * Math::Sin(angle);

                float cosRot = Math::Cos(formationRot);
                float sinRot = Math::Sin(formationRot);
                float rotatedX = x * cosRot - y * sinRot;
                float rotatedY = x * sinRot + y * cosRot;

                Vector2 offset(rotatedX, rotatedY);
                Vector2 guardPos(centerPos.x + rotatedX, centerPos.y + rotatedY);

                Enemy* guard = nullptr;

                if (guardShipType == "LowLevelEnemy") {
                    guard = new LowLevelEnemy(mGame);
                }
                else if (guardShipType == "DroneGuard") {
                    guard = new DroneGuard(mGame);
                }
                else if (guardShipType == "SentryGun") {
                    guard = new SentryGun(mGame);
                }
                else if (guardShipType == "BattleShip") {
                    guard = new BattleShip(mGame, guardPos);
                }

                if (guard) {
                    if (guardShipType != "BattleShip") {
                        guard->SetPosition(guardPos);
                    }

                    if (PatrolComponent* pc = guard->GetComponent<PatrolComponent>()) {
                        pc->SetFormationTarget(flagship, offset);
                    }

                    if (mGame->mHUDActor != nullptr) {
                        HUDSpriteComponent* hpBar = new HUDSpriteComponent(mGame->mHUDActor);
                        hpBar->SetTexture(mGame->GetTexture("Assets/HP1.png"));
                        hpBar->SetOffset(Vector2(0, -50.0f));
                        hpBar->SetTargetActor(guard);
                    }
                }
            }
        }
    }

    return enemy;
}