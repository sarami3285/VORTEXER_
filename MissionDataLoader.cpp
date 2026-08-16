#include "MissionDataLoader.h"
#include "Random.h"
#include <SDL_log.h>

bool MissionDataLoader::LoadMission(int id, MissionData& outData) { if (id < 100) { return LoadFromJson(id, outData); } else { GenerateRandomMission(id, outData); return true; } }

bool MissionDataLoader::LoadFromJson(int id, MissionData& outData) {
    std::ifstream file("Assets/Data/MissionData.json"); if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
    }
    catch (const json::exception& e) {
        SDL_Log("Mission data read error: %s", e.what());
        return false;
    }

    if (!j.contains("missions") || !j["missions"].is_array()) return false;

    bool found = false;
    try {
    for (auto& m : j["missions"]) {
        if (m.contains("id") && m["id"].get<int>() == id) {
            outData = MissionData();
            outData.id = id;
            outData.name = m.value("name", "Unknown Mission");
            outData.description = m.value("description", "");
            outData.iconPath = m.value("icon_path", "Assets/DefaultMission.png");

            outData.targetType = m.value("target_type", "Elimination");

            auto& r = m["rewards"];
            outData.Rewards.BaseCredits = r.value("BaseCredits", 0);
            outData.Rewards.BonusMultiplier = r.value("BonusMultiplier", 1.0f);

            outData.mapWidth = m.value("map_width", 2000.0f);
            outData.mapHeight = m.value("map_height", 2000.0f);

            auto& tc = m["map_tile_config"];
            outData.mapTileConfig.tileType = tc.value("tile_type", "City");
            outData.mapTileConfig.tileSize = tc.value("tile_size", 540);
            outData.mapTileConfig.tilesetTexture = tc.value("tileset_texture", "Assets/MapTile.png");
            outData.mapTileConfig.tileSetRows = tc.value("tile_set_rows", 3);

            if (m.contains("enemy_configs") && m["enemy_configs"].is_array()) {
                for (auto& e : m["enemy_configs"]) {
                    EnemyConfig config;
                    config.type = e.value("type", "Standard");
                    config.count = e.value("count", 0);
                    config.min_dist = static_cast<float>(e.value("min_dist", 100.0));
                    config.max_dist = static_cast<float>(e.value("max_dist", 1000.0));

                    if (e.contains("positions") && e["positions"].is_array()) {
                        for (auto& posArr : e["positions"]) {
                            if (posArr.is_array() && posArr.size() >= 2) {
                                config.fixedPositions.push_back(Vector2(posArr[0].get<float>(), posArr[1].get<float>()));
                            }
                        }
                    }

                    if (config.type == "Formation") {
                        config.formationType = e.value("formation_type", "");
                        config.mainShipType = e.value("main_ship_type", "");
                        config.radiusX = e.value("radius_x", 0.0f);
                        config.radiusY = e.value("radius_y", 0.0f);
                        config.formationRotation = e.value("formation_rotation", 0.0f);

                        if (e.contains("center_position") && e["center_position"].is_array()) {
                            config.centerPosition = Vector2(
                                e["center_position"][0].get<float>(),
                                e["center_position"][1].get<float>()
                            );
                        }

                        if (e.contains("guard_details") && e["guard_details"].is_array()) {
                            for (auto& g : e["guard_details"]) {
                                GuardDetailConfig gd;
                                gd.guardShipType = g.value("guard_ship_type", "");
                                gd.guardCount = g.value("guard_count", 0);
                                gd.radiusRatio = g.value("radius_ratio", 1.0f);
                                config.guardDetails.push_back(gd);
                            }
                        }
                    }

                    outData.enemyConfigs.push_back(config);
                }
            }

            if (m.contains("playerPos") && m["playerPos"].is_object()) {
                auto& p = m["playerPos"];
                outData.playerPos.x = p.value("x", 0.0f);
                outData.playerPos.y = p.value("y", 0.0f);
            }
            else {
                outData.playerPos = Vector2(512.0f, 384.0f);
            }

            if (m.contains("reinforcements") && m["reinforcements"].is_array()) {
                for (auto& re : m["reinforcements"]) {
                    ReinforcementConfig rConfig;
                    rConfig.time = re.value("time", 0.0f);
                    rConfig.type = re.value("type", "LowLevelEnemy");
                    rConfig.count = re.value("count", 0);
                    rConfig.spawned = false;
                    outData.reinforcements.push_back(rConfig);
                }
            }

            if (m.contains("target_configs") && m["target_configs"].is_array()) {
                for (auto& t : m["target_configs"]) {
                    TargetConfig tConfig;
                    tConfig.type = t.value("type", "Target");
                    tConfig.hp = t.value("hp", 100);
                    tConfig.texture = t.value("texture", "");
                    if (t.contains("position") && t["position"].is_array()) {
                        tConfig.position = Vector2(t["position"][0].get<float>(), t["position"][1].get<float>());
                    }
                    outData.targetConfigs.push_back(tConfig);
                }
            }

            if (m.contains("defense_target")) {
                auto& dt = m["defense_target"];
                outData.defenseTarget.type = dt.value("type", "Core");
                outData.defenseTarget.hp = dt.value("hp", 1000);
                if (dt.contains("position") && dt["position"].is_array()) {
                    outData.defenseTarget.position = Vector2(
                        dt["position"][0].get<float>(),
                        dt["position"][1].get<float>()
                    );
                }
                outData.defenseTarget.texture = dt.value("texture", "Assets/AllyBase.png");
            }

            if (m.contains("defense_duration")) {
                outData.defenseDuration = m["defense_duration"].get<float>();
            }

            if (m.contains("ally_configs") && m["ally_configs"].is_array()) {
                for (auto& a : m["ally_configs"]) {
                    AllyConfig aConfig;
                    aConfig.type = a.value("type", "AllyDrone");
                    aConfig.count = a.value("count", 1);
                    aConfig.initialMode = a.value("initial_mode", "Attack");

                    if (a.contains("fixed_positions") && a["fixed_positions"].is_array()) {
                        for (auto& posArr : a["fixed_positions"]) {
                            if (posArr.is_array() && posArr.size() >= 2) {
                                aConfig.fixedPositions.push_back(Vector2(posArr[0].get<float>(), posArr[1].get<float>()));
                            }
                        }
                    }
                    outData.allyConfigs.push_back(aConfig);
                }
            }

            found = true;
            break;
        }
    }
    }
    catch (const json::exception& e) {
        SDL_Log("Mission data type error for ID %d: %s", id, e.what());
        return false;
    }
    return found;
}

void MissionDataLoader::GenerateRandomMission(int id, MissionData& outData) {
    outData = MissionData(); outData.id = id;

    srand(static_cast<unsigned int>(id));
    int level = ((id - 100) % 5) + 1;

    std::vector<std::string> types = { "City", "Desert", "Ocean" };
    outData.mapTileConfig.tileType = types[rand() % types.size()];
    outData.mapTileConfig.tileSize = 540;
    outData.mapTileConfig.tilesetTexture = "Assets/MapTile.png";
    outData.mapTileConfig.tileSetRows = 3;

    outData.mapWidth = 3000.0f + (level * 1000.0f);
    outData.mapHeight = 3000.0f + (level * 1000.0f);

    outData.playerPos = Vector2(outData.mapWidth / 2.0f, outData.mapHeight / 2.0f);

    outData.targetType = "Elimination";
    outData.Rewards.BaseCredits = 1000 * level;
    outData.Rewards.BonusMultiplier = 1.0f + (level * 0.2f);

    outData.enemyConfigs.clear();

    float safeRadius = 500.0f;
    float safeRadiusSq = safeRadius * safeRadius;

    int groupCount = 3 + level;
    for (int i = 0; i < groupCount; ++i) {
        EnemyConfig group;
        group.type = (rand() % 2 == 0) ? "LowLevelEnemy" : "DroneGuard";
        group.count = 5 + level;
        group.min_dist = 100.0f;
        group.max_dist = 1500.0f;

        Vector2 groupPos;
        bool valid = false;
        int tries = 0;
        while (!valid && tries < 100) {
            groupPos = Vector2(
                static_cast<float>(200 + rand() % static_cast<int>(outData.mapWidth - 400)),
                static_cast<float>(200 + rand() % static_cast<int>(outData.mapHeight - 400))
            );
            if ((groupPos - outData.playerPos).LengthSq() > safeRadiusSq) {
                valid = true;
            }
            tries++;
        }
        group.centerPosition = groupPos;

        group.formationType = "Circle";
        group.radiusX = 400.0f;
        group.radiusY = 400.0f;

        outData.enemyConfigs.push_back(group);
    }

    outData.name = "Random Mission Lv." + std::to_string(level);
    outData.description = "【UNAUTHORIZED MISSION】\n難易度 Lv." + std::to_string(level) + "\n全敵機を殲滅せよ。";
    outData.iconPath = "Assets/DefaultMission.png";
}
