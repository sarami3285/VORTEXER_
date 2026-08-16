#pragma once

// 強化の種類
enum class UpgradeType {
    DamageBoost,    // 攻撃力アップ
    FireRateBoost,  // 連射速度アップ
    DefenseBoost,   // 防御力アップ
    SpeedBoost,     // 移動速度アップ
    PelletAdd,       // 散弾数アップ
    Repair,
    SummonAlly
};

// 強化カードのデータ
struct UpgradeOption {
    UpgradeType type;
    std::string name;
    std::string description;
    float value; // 強化する値（1.15f など）
};
