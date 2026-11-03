#pragma once

/**
 * @file Merchant.hpp
 * @brief Void.INC — the Merchant: a rare visitor selling minor permanent upgrades and cosmetics.
 *
 * The Merchant arrives at a random moment (MERCHANT_WAIT_MIN..MAX seconds of
 * play after the last visit), stays MERCHANT_STAY seconds, then leaves. Each
 * visit offers up to MERCHANT_OFFERS items picked at random from what you
 * don't have maxed. Everything is paid in Credits and is PERMANENT:
 * Fragmentation does not touch it.
 *
 *  - Upgrades: small multipliers with a level cap. Price grows per level and
 *    with a progress scale (sqrt of lifetime Credits), so it never becomes free.
 *  - Cosmetics: spiral colours. One is equipped at a time.
 *
 * Offer ids: 0..upgrades-1 = upgrade, kCosmeticBase + j = cosmetic j. Append
 * new upgrades / cosmetics at the END of their list.
 *
 * Every number below is a balance knob.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>
#include "Flags.hpp"
#include "dependancies/ErisNum.hpp"

enum class MerchantEffect { DATA_PROD, INFO_PROD, ENTROPY_PROD, SELL_VALUE, RESEARCH_SPEED };

struct MerchantUpgrade {
    std::string    name;
    std::string    desc;
    double         baseCost;
    int            maxLevel;
    MerchantEffect effect;
    double         perLevel;   ///< each level multiplies by (1 + perLevel)
};

struct Cosmetic {
    std::string name;
    double      cost;
    uint8_t     r, g, b;
};

inline const std::vector<MerchantUpgrade> merchantUpgrades = {
    { "Polished Cache",    "Data production +4% per level.",         400, 5, MerchantEffect::DATA_PROD,      0.04 },
    { "Spare Indexes",     "Information gained +4% per level.",      400, 5, MerchantEffect::INFO_PROD,      0.04 },
    { "Void Lenses",       "Entropy recovered +4% per level.",       600, 5, MerchantEffect::ENTROPY_PROD,   0.04 },
    { "Broker's Ledger",   "Credits from selling +3% per level.",    800, 5, MerchantEffect::SELL_VALUE,     0.03 },
    { "Overclocked Fans",  "Research speed +4% per level.",          700, 5, MerchantEffect::RESEARCH_SPEED, 0.04 },
};

inline const std::vector<Cosmetic> cosmetics = {
    { "Cyan Spiral",    1500,  90, 230, 255 },
    { "Violet Spiral",  1500, 190, 130, 255 },
    { "Gold Spiral",    1500, 255, 205,  90 },
    { "Crimson Spiral", 1500, 255,  90,  90 },
    { "Green Spiral",   1500, 120, 255, 150 },
};

namespace MerchantRules {
    inline constexpr double MERCHANT_WAIT_MIN = 180.0;
    inline constexpr double MERCHANT_WAIT_MAX = 480.0;
    inline constexpr double MERCHANT_STAY     = 75.0;
    inline constexpr size_t MERCHANT_OFFERS   = 3;
    inline constexpr double LEVEL_COST_GROWTH = 2.2;
    inline constexpr uint8_t kCosmeticBase    = 64;
}

[[nodiscard]] inline std::mt19937& merchantRng() {
    static std::mt19937 rng{ std::random_device{}() };
    return rng;
}

template <typename GS>
inline void ensureMerchantState(GS& gs) {
    gs.merchantLevels.resize(merchantUpgrades.size(), 0);
    for (size_t i = 0; i < merchantUpgrades.size(); ++i)
        gs.merchantLevels[i] = static_cast<uint8_t>(std::min<int>(gs.merchantLevels[i], merchantUpgrades[i].maxLevel));
    if (gs.cosmeticEquipped < -1 || gs.cosmeticEquipped >= static_cast<int32_t>(cosmetics.size())) gs.cosmeticEquipped = -1;
    if (!std::isfinite(gs.merchantTimer) || gs.merchantTimer < 0.0) gs.merchantTimer = 0.0;
    std::erase_if(gs.merchantOffers, [](uint8_t id) {
        return id < MerchantRules::kCosmeticBase ? id >= merchantUpgrades.size()
                                                 : static_cast<size_t>(id - MerchantRules::kCosmeticBase) >= cosmetics.size();
    });
    if (gs.merchantOffers.size() > MerchantRules::MERCHANT_OFFERS) gs.merchantOffers.resize(MerchantRules::MERCHANT_OFFERS);
    if (gs.merchantPresent && gs.merchantOffers.empty()) { gs.merchantPresent = false; gs.merchantTimer = 0.0; }
}

template <typename GS>
[[nodiscard]] inline double merchantMult(const GS& gs, MerchantEffect e) {
    double m = 1.0;
    for (size_t i = 0; i < merchantUpgrades.size() && i < gs.merchantLevels.size(); ++i)
        if (merchantUpgrades[i].effect == e)
            m *= std::pow(1.0 + merchantUpgrades[i].perLevel, static_cast<double>(gs.merchantLevels[i]));
    return m;
}

template <typename GS>
[[nodiscard]] inline double merchantScale(const GS& gs) {
    return std::max(1.0, std::sqrt(std::max(0.0, gs.allCreditProduced.toDouble()) / 1000.0));
}

template <typename GS>
[[nodiscard]] inline double offerCost(const GS& gs, uint8_t id) {
    if (id >= MerchantRules::kCosmeticBase) {
        const size_t j = id - MerchantRules::kCosmeticBase;
        return j < cosmetics.size() ? cosmetics[j].cost * merchantScale(gs) : 0.0;
    }
    if (id >= merchantUpgrades.size()) return 0.0;
    const size_t lv = id < gs.merchantLevels.size() ? gs.merchantLevels[id] : 0;
    return merchantUpgrades[id].baseCost * std::pow(MerchantRules::LEVEL_COST_GROWTH, static_cast<double>(lv)) * merchantScale(gs);
}

template <typename GS>
[[nodiscard]] inline bool offerAvailable(const GS& gs, uint8_t id) {
    if (id >= MerchantRules::kCosmeticBase) {
        const size_t j = id - MerchantRules::kCosmeticBase;
        return j < cosmetics.size() && !Flags::get(gs.cosmeticFlags, j);
    }
    return id < merchantUpgrades.size() && (id >= gs.merchantLevels.size() || gs.merchantLevels[id] < merchantUpgrades[id].maxLevel);
}

template <typename GS>
inline void rollOffers(GS& gs, std::mt19937& rng) {
    std::vector<uint8_t> pool;
    for (size_t i = 0; i < merchantUpgrades.size(); ++i) if (offerAvailable(gs, static_cast<uint8_t>(i))) pool.push_back(static_cast<uint8_t>(i));
    for (size_t j = 0; j < cosmetics.size(); ++j)
        if (offerAvailable(gs, static_cast<uint8_t>(MerchantRules::kCosmeticBase + j))) pool.push_back(static_cast<uint8_t>(MerchantRules::kCosmeticBase + j));
    std::shuffle(pool.begin(), pool.end(), rng);
    if (pool.size() > MerchantRules::MERCHANT_OFFERS) pool.resize(MerchantRules::MERCHANT_OFFERS);
    gs.merchantOffers = pool;
}

template <typename GS>
inline void tickMerchant(GS& gs, double dt, std::mt19937& rng = merchantRng()) {
    if (dt <= 0.0 || !gs.marketUnlocked) return;     // the Merchant starts visiting once you trade
    auto wait = [&] { return std::uniform_real_distribution<double>(MerchantRules::MERCHANT_WAIT_MIN, MerchantRules::MERCHANT_WAIT_MAX)(rng); };
    if (!gs.merchantPresent) {
        if (gs.merchantTimer <= 0.0) gs.merchantTimer = wait();          // first schedule
        gs.merchantTimer -= dt;
        if (gs.merchantTimer <= 0.0) {
            rollOffers(gs, rng);
            if (gs.merchantOffers.empty()) { gs.merchantTimer = wait(); return; }   // nothing left to sell
            gs.merchantPresent = true;
            gs.merchantTimer   = MerchantRules::MERCHANT_STAY;
        }
    } else {
        gs.merchantTimer -= dt;
        if (gs.merchantTimer <= 0.0 || gs.merchantOffers.empty()) {
            gs.merchantPresent = false;
            gs.merchantOffers.clear();
            gs.merchantTimer = wait();
        }
    }
}

template <typename GS>
[[nodiscard]] inline bool canBuyOffer(const GS& gs, size_t slot) {
    if (!gs.merchantPresent || slot >= gs.merchantOffers.size()) return false;
    const uint8_t id = gs.merchantOffers[slot];
    return offerAvailable(gs, id) && gs.credits >= ErisNum::ErisNum(offerCost(gs, id));
}

template <typename GS>
inline bool buyOffer(GS& gs, size_t slot) {
    if (!canBuyOffer(gs, slot)) return false;
    const uint8_t id = gs.merchantOffers[slot];
    gs.credits -= ErisNum::ErisNum(offerCost(gs, id));
    if (id >= MerchantRules::kCosmeticBase) {
        const size_t j = id - MerchantRules::kCosmeticBase;
        Flags::set(gs.cosmeticFlags, j, true);
        gs.cosmeticEquipped = static_cast<int32_t>(j);
    } else {
        gs.merchantLevels.resize(merchantUpgrades.size(), 0);
        ++gs.merchantLevels[id];
    }
    gs.merchantOffers.erase(gs.merchantOffers.begin() + static_cast<std::ptrdiff_t>(slot));
    return true;
}
