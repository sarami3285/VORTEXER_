#include "DataManager.h"
#include <fstream>
#include <string>
#include <algorithm>
#include "json.hpp"
#include "SDL.h"

using json = nlohmann::json;
int DataManager::mCurrencyAmount = 0;

int DataManager::GetCurrency() const {
    return mCurrencyAmount;
}

void DataManager::AddCurrency(int amount) {
    if (amount < 0) return;
    mCurrencyAmount += amount;
}

bool DataManager::UseCurrency(int amount) {
    if (amount < 0) return false;
    if (mCurrencyAmount < amount) return false;
    mCurrencyAmount -= amount;
    return true;
}

void DataManager::SetCurrency(int amount) {
    if (amount >= 0) {
        mCurrencyAmount = amount;
    }
}

bool DataManager::IsWeaponObtained(const std::string& weaponName) const {
    std::ifstream file("Assets/Data/WeaponData.json");
    if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
        if (j.contains("weapons") && j["weapons"].is_object() && j["weapons"].contains(weaponName)) {
            return j["weapons"][weaponName].value("GetWeapon", false);
        }
    }
    catch (const json::exception& e) {
        SDL_Log("Weapon data read error: %s", e.what());
    }
    return false;
}

void DataManager::SetWeaponObtained(const std::string& weaponName, bool obtained) {
    std::ifstream inFile("Assets/Data/WeaponData.json");
    if (!inFile.is_open()) {
        SDL_Log("Error: Could not open WeaponData.json for reading.");
        return;
    }

    json data;
    try {
        inFile >> data;
        inFile.close();
    }
    catch (json::exception& e) {
        SDL_Log("JSON parse error on read: %s", e.what());
        return;
    }

    if (data.contains("weapons") && data["weapons"].contains(weaponName)) {
        data["weapons"][weaponName]["GetWeapon"] = obtained;
        std::ofstream outFile("Assets/Data/WeaponData.json");
        if (outFile.is_open()) {
            outFile << data.dump(4);
            outFile.close();
        }
        else {
            SDL_Log("Error: Could not open WeaponData.json for writing.");
        }
    }
}

bool DataManager::SaveGameData(const std::string& path, const std::string& equippedWeaponName) {
    json j;
    j["currency"] = mCurrencyAmount;
    j["equippedWeapon"] = equippedWeaponName;

    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        SDL_Log("Error: Could not open save file for writing.");
        return false;
    }

    j["clearedMissions"] = mClearedMissions;

    ofs << j.dump(4);
    return true;
}

bool DataManager::LoadGameData(const std::string& path, std::string& outEquippedWeaponName) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        SDL_Log("Save file not found, starting a new game.");
        return false;
    }

    json j;
    try {
        ifs >> j;
    }
    catch (json::exception& e) {
        SDL_Log("JSON parse error on load: %s", e.what());
        return false;
    }

    try {
        if (j.contains("currency")) {
            mCurrencyAmount = std::max(0, j["currency"].get<int>());
        }
        if (j.contains("equippedWeapon")) {
            outEquippedWeaponName = j["equippedWeapon"].get<std::string>();
        }

        mClearedMissions.clear();
        if (j.contains("clearedMissions") && j["clearedMissions"].is_array()) {
            for (const auto& id : j["clearedMissions"]) {
                mClearedMissions.push_back(id.get<int>());
            }
        }
    }
    catch (const json::exception& e) {
        SDL_Log("Save data type error: %s", e.what());
        mClearedMissions.clear();
        return false;
    }

    return true;
}

void DataManager::AddClearedMission(int id, const std::string& path, const std::string& weaponName) {
    auto it = std::find(mClearedMissions.begin(), mClearedMissions.end(), id);
    if (it == mClearedMissions.end()) {
        mClearedMissions.push_back(id);
        SaveGameData(path, weaponName);
    }
}


bool DataManager::ValidateGameData() {
    bool isValid = true;

    std::ifstream missionFile("Assets/Data/MissionData.json");
    if (!missionFile.is_open()) {
        SDL_Log("Validation Error: Missing MissionData.json");
        isValid = false;
    }
    else {
        json j;
        try {
            missionFile >> j;
            if (j.contains("missions") && j["missions"].is_array()) {
                for (const auto& m : j["missions"]) {
                    if (!m.contains("id") || !m.contains("name") || !m.contains("map_x") || !m.contains("map_y") || !m.contains("prev_mission_ids")) {
                        SDL_Log("Validation Error: Mission data missing essential keys.");
                        isValid = false;
                    }
                }
            }
            else {
                SDL_Log("Validation Error: Mission data must contain a missions array.");
                isValid = false;
            }
        }
        catch (json::exception& e) {
            SDL_Log("Validation Error: MissionData.json parse error: %s", e.what());
            isValid = false;
        }
    }

    std::ifstream weaponFile("Assets/Data/WeaponData.json");
    if (!weaponFile.is_open()) {
        SDL_Log("Validation Error: Missing WeaponData.json");
        isValid = false;
    }
    else {
        json j;
        try {
            weaponFile >> j;
            if (j.contains("weapons") && j["weapons"].is_object()) {
                for (auto& [key, val] : j["weapons"].items()) {
                    if (!val.contains("price") || !val.contains("weaponIcon")) {
                        SDL_Log("Validation Error: Weapon data missing essential keys.");
                        isValid = false;
                    }
                }
            }
            else {
                SDL_Log("Validation Error: Weapon data must contain a weapons object.");
                isValid = false;
            }
        }
        catch (json::exception& e) {
            SDL_Log("Validation Error: WeaponData.json parse error: %s", e.what());
            isValid = false;
        }
    }

    return isValid;
}
