#pragma once
#include <string>
#include <vector>
#include <map>

class DataManager {
public:
    int GetCurrency() const;
    void AddCurrency(int amount);
    bool UseCurrency(int amount);
    void SetCurrency(int amount);

    bool SaveGameData(const std::string& path, const std::string& equippedWeaponName);
    bool LoadGameData(const std::string& path, std::string& outEquippedWeaponName);

    bool IsWeaponObtained(const std::string& weaponName) const;
    void SetWeaponObtained(const std::string& weaponName, bool obtained);

    const std::vector<int>& GetClearedMissions() const { return mClearedMissions; }
    void AddClearedMission(int id, const std::string& path, const std::string& weaponName);


    bool ValidateGameData();

private:
    static int mCurrencyAmount;
    std::vector<int> mClearedMissions;
};