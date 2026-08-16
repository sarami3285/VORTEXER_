#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

struct MissionRewards {
    int BaseCredits = 0;
    float BonusMultiplier = 1.0f;
};

struct MapTileConfig {
    std::string tilesetTexture;
    int tileSize = 0;
    int tileSetRows = 0;
    std::string tileType;
};

struct GuardDetailConfig {
    std::string guardShipType;
    int guardCount = 0;
    float radiusRatio = 1.0f;
};

struct EnemyConfig
{
    std::string type;
    int count = 0;
    float min_dist = 0.0f;
    float max_dist = 0.0f;
    std::vector<Vector2> fixedPositions;

    std::string formationType;
    std::string mainShipType;

    Vector2 centerPosition = Vector2::Zero;

    float radiusX = 0.0f;
    float radiusY = 0.0f;
    float formationRotation = 0.0f;

    std::vector<GuardDetailConfig> guardDetails;
};

struct AllyConfig {
    std::string type;
    int count = 1;
    std::vector<Vector2> fixedPositions;
    std::string initialMode = "Attack";
};

struct TargetConfig {
    std::string type;
    Vector2 position;
    int hp = 100;
    std::string texture;
};

struct ReinforcementConfig {
    float time = 0.0f;
    std::string type;
    int count = 0;
    bool spawned = false;
};

struct MissionData {
    int id = 0;
    std::string name;
    float mapWidth = 0.0f;
    float mapHeight = 0.0f;
    Vector2 playerPos;
    std::string targetType = "Elimination";
    std::vector<EnemyConfig> enemyConfigs;
    std::vector<TargetConfig> targetConfigs;
    std::vector<AllyConfig> allyConfigs;
    std::vector<ReinforcementConfig> reinforcements;
    std::string description;
    std::string iconPath;

    MapTileConfig mapTileConfig;

    TargetConfig defenseTarget;
    float defenseDuration = 0.0f;

    MissionRewards Rewards;
};

class MissionDataLoader {
public:
    static bool LoadMission(int missionID, MissionData& outData);

private:
    static bool LoadFromJson(int id, MissionData& outData);
    static void GenerateRandomMission(int id, MissionData& outData);
};
