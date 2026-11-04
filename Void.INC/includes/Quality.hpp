#pragma once

/**
 * @file Quality.hpp
 * @brief Void.INC — Quality Assurance: Data and Information carry a Quality %.
 *
 * Each of Data and Information has a live Quality percentage. 50% is the base
 * value: everything that is produced is multiplied by quality / 50, so 50% =
 * x1.0, 100% = x2.0, 25% = x0.5.
 *
 * Quality drifts: it wanders around a MEAN (50% at level 0, +5% per QA level,
 * max level 10 => 100%) with random swings. You also set a FLOOR for each
 * currency. While the live quality is BELOW the floor, production of that
 * currency is not banked at all — it is turned straight into Entropy instead
 * (lossy on purpose: far less Entropy than the Data / Information lost).
 * A floor of 0 means "never convert".
 *
 * Levels are bought with Credits, are permanent, and Fragmentation keeps them.
 * QA unlocks after the first Fragmentation (mid-game feature).
 *
 * State: GameState::qa* fields. Every number below is a balance knob.
 */

#include <algorithm>
#include <cmath>
#include <random>
#include "dependancies/ErisNum.hpp"

namespace QA {
    inline constexpr int    MAX_LEVEL        = 10;
    inline constexpr double BASE_MEAN        = 50.0;     // %
    inline constexpr double MEAN_PER_LEVEL   = 5.0;      // % per level
    inline constexpr double REVERSION        = 0.08;     // pull toward the mean per second
    inline constexpr double NOISE            = 5.0;      // % swing per sqrt(second)
    inline constexpr double MAX_QUALITY      = 150.0;    // hard clamp
    inline constexpr int    FLOOR_STEP       = 5;
    inline constexpr int    FLOOR_MAX        = 95;
    inline constexpr double UPGRADE_BASE     = 2000.0;   // Credits for level 1
    inline constexpr double UPGRADE_GROWTH   = 3.0;
    // Entropy gained per unit of production sent to Entropy. Chosen so that, at
    // the Fragment weights, converting is always a net LOSS of Fragment value.
    inline constexpr double ENTROPY_PER_DATA = 0.004;
    inline constexpr double ENTROPY_PER_INFO = 0.01;
}

[[nodiscard]] inline double qaMean(int level) {
    return QA::BASE_MEAN + QA::MEAN_PER_LEVEL * std::clamp(level, 0, QA::MAX_LEVEL);
}

[[nodiscard]] inline double qaUpgradeCost(int level) {
    return QA::UPGRADE_BASE * std::pow(QA::UPGRADE_GROWTH, static_cast<double>(level));
}

template <typename GS>
inline void ensureQA(GS& gs) {
    gs.qaLevelData = std::clamp(gs.qaLevelData, 0, QA::MAX_LEVEL);
    gs.qaLevelInfo = std::clamp(gs.qaLevelInfo, 0, QA::MAX_LEVEL);
    gs.qaFloorData = std::clamp(gs.qaFloorData, 0, QA::FLOOR_MAX);
    gs.qaFloorInfo = std::clamp(gs.qaFloorInfo, 0, QA::FLOOR_MAX);
    if (!std::isfinite(gs.qaQualData)) gs.qaQualData = QA::BASE_MEAN;
    if (!std::isfinite(gs.qaQualInfo)) gs.qaQualInfo = QA::BASE_MEAN;
    gs.qaQualData = std::clamp(gs.qaQualData, 0.0, QA::MAX_QUALITY);
    gs.qaQualInfo = std::clamp(gs.qaQualInfo, 0.0, QA::MAX_QUALITY);
}

/// Advance both qualities (Ornstein-Uhlenbeck around their means).
template <typename GS>
inline void tickQuality(GS& gs, double dt, std::mt19937& rng) {
    if (dt <= 0.0 || !gs.qaUnlocked) return;
    std::normal_distribution<double> nd(0.0, 1.0);
    auto step = [&](double q, int level) {
        q += (qaMean(level) - q) * std::min(1.0, QA::REVERSION * dt) + QA::NOISE * std::sqrt(dt) * nd(rng);
        return std::clamp(q, 0.0, QA::MAX_QUALITY);
    };
    gs.qaQualData = step(gs.qaQualData, gs.qaLevelData);
    gs.qaQualInfo = step(gs.qaQualInfo, gs.qaLevelInfo);
}

/// True when production of this currency is currently being turned into Entropy.
template <typename GS>
[[nodiscard]] inline bool qaConverting(const GS& gs, bool info) {
    if (!gs.qaUnlocked) return false;
    return (info ? gs.qaQualInfo : gs.qaQualData) < static_cast<double>(info ? gs.qaFloorInfo : gs.qaFloorData);
}

/// Multiplier applied to banked production right now (0 while converting).
template <typename GS>
[[nodiscard]] inline double qaMultiplier(const GS& gs, bool info) {
    if (!gs.qaUnlocked) return 1.0;
    if (qaConverting(gs, info)) return 0.0;
    return (info ? gs.qaQualInfo : gs.qaQualData) / QA::BASE_MEAN;
}

template <typename GS>
[[nodiscard]] inline bool canBuyQA(const GS& gs, bool info) {
    const int lv = info ? gs.qaLevelInfo : gs.qaLevelData;
    return gs.qaUnlocked && lv < QA::MAX_LEVEL && gs.credits >= ErisNum::ErisNum(qaUpgradeCost(lv));
}

template <typename GS>
inline bool buyQA(GS& gs, bool info) {
    if (!canBuyQA(gs, info)) return false;
    int& lv = info ? gs.qaLevelInfo : gs.qaLevelData;
    gs.credits -= ErisNum::ErisNum(qaUpgradeCost(lv));
    ++lv;
    return true;
}

template <typename GS>
inline void stepQAFloor(GS& gs, bool info, int dir) {
    int& f = info ? gs.qaFloorInfo : gs.qaFloorData;
    f = std::clamp(f + dir * QA::FLOOR_STEP, 0, QA::FLOOR_MAX);
}
