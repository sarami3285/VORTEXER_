#pragma once
#include "json.hpp"
#include <fstream>
#include <string>
#include <stdexcept>

using json = nlohmann::json;

struct WeaponData
{
    std::string name;
    std::string bulletTexturePath;
    int price = 0;
    std::string description;
    float fireRate = 0.0f;
    int baseDamage = 0;
    float bulletSpeed = 0.0f;
    float decayRate = 0.0f;
    int burstCount = 1;
    float burstInterval = 0.0f;
    int pelletCount = 1;
    float spreadAngle = 0.0f;
    float randomSpread = 0.0f;
    std::string weaponIcon;
    std::string shotSound;
    bool gotWeapon = false;

    bool LoadFromJSON(const std::string& filepath, const std::string& weaponName)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        json j;
        try {
            file >> j;
        }
        catch (const json::parse_error& e) {
            SDL_Log("JSON PARSE ERROR: %s at byte %d", e.what(), static_cast<int>(e.byte));
            return false;
        }
        catch (const std::exception& e) {
            SDL_Log("JSON LOADING ERROR: %s", e.what());
            return false;
        }

        if (!j.contains("weapons") || !j["weapons"].contains(weaponName)) return false;

        const auto& w = j["weapons"][weaponName];

        try {
            name = weaponName;
            bulletTexturePath = w["bulletTexturePath"].get<std::string>();
            fireRate = w["fireRate"].get<float>();
            price = w["price"].get<int>();
            description = w.value("description", "説明文がありません");
            baseDamage = w["baseDamage"].get<int>();
            bulletSpeed = w["bulletSpeed"].get<float>();
            randomSpread = w.value("randomSpread", 0.0f);
            decayRate = w["decayRate"].get<float>();
            burstCount = w["burstCount"].get<int>();
            burstInterval = w["burstInterval"].get<float>();
            pelletCount = w["pelletCount"].get<int>();
            spreadAngle = w["spreadAngle"].get<float>();
            weaponIcon = w.value("weaponIcon", "");
            shotSound = w.value("shotSound", "");
            gotWeapon = w.value("GetWeapon", false);
        }
        catch (const std::exception& e) {
            SDL_Log("JSON parse error: %s", e.what());
            return false;
        }

        return true;
    }

    WeaponData() = default;
    WeaponData(const WeaponData&) = default;
    WeaponData& operator=(const WeaponData&) = default;

    WeaponData(WeaponData&&) noexcept = default;
    WeaponData& operator=(WeaponData&&) noexcept = default;
};
