#pragma once

/**
 * @file Marketplace.hpp
 * @brief Void.INC — Marketplace perks, purchased with Credits.
 *
 * Marketplace items are one-time credit purchases that apply passive
 * multipliers to production or market behaviour. Multiple items targeting
 * the same thing stack multiplicatively (same rule as Supports).
 *
 * Perk types (MarketPerkType):
 *   DATA_PROD       — multiplies Data production rate
 *   INFO_PROD       — multiplies Information production rate
 *   ENTROPY_PROD    — multiplies Entropy gain from void interception
 *   MARKET_VOL_CAP  — raises the maximum market multiplier cap
 *   DATA_SELL_VALUE — multiplies the per-unit credit yield when selling Data
 *   INFO_SELL_VALUE — multiplies the per-unit credit yield when selling Info
 *
 * Saved via GameState::marketFlags (see Flags.hpp).
 */

#include <string>
#include <vector>
#include <cstdint>
#include "dependancies/ErisNum.hpp"
#include "Flags.hpp"

enum class MarketPerkType {
    DATA_PROD,        ///< boost Data production rate
    INFO_PROD,        ///< boost Information production rate
    ENTROPY_PROD,     ///< boost Entropy gain from void interception
    MARKET_VOL_CAP,   ///< raise the market multiplier ceiling
    DATA_SELL_VALUE,  ///< more credits per unit of Data sold
    INFO_SELL_VALUE,  ///< more credits per unit of Info sold
};

struct MarketPerk {
    std::string      name;
    ErisNum::ErisNum cost;              ///< Credits cost
    MarketPerkType   type;
    long double      multiplier = 1.5L; ///< Stacks multiplicatively
    uint8_t          purchased  = 0;
};

// Perk costs grow x3.5 per rung (5 CR -> ~112K CR). Ordered by cost; set with
// tools/econ_sim.py (see Economy.hpp). New perks go at the END (flags are positional).
inline std::vector<MarketPerk> marketPerks = {
    { "Data Cache",       ErisNum::ErisNum(5.0),      MarketPerkType::DATA_PROD,        1.5L  },
    { "Signal Filter",    ErisNum::ErisNum(18.0),     MarketPerkType::INFO_PROD,        1.5L  },
    { "Parallel Streams", ErisNum::ErisNum(61.0),     MarketPerkType::DATA_PROD,        2.0L  },
    { "Deep Indexing",    ErisNum::ErisNum(214.0),    MarketPerkType::INFO_PROD,        2.0L  },
    { "Chaos Tap",        ErisNum::ErisNum(750.0),    MarketPerkType::ENTROPY_PROD,     1.75L },
    { "Data Broker",      ErisNum::ErisNum(2600.0),   MarketPerkType::DATA_SELL_VALUE,  1.5L  },
    { "Info Arbitrage",   ErisNum::ErisNum(9200.0),   MarketPerkType::INFO_SELL_VALUE,  2.0L  },
    { "Risk Protocol",    ErisNum::ErisNum(32000.0),  MarketPerkType::MARKET_VOL_CAP,   1.5L  },
    { "Extreme Leverage", ErisNum::ErisNum(112000.0), MarketPerkType::MARKET_VOL_CAP,   2.0L  },
};

inline void packMarket(const std::vector<MarketPerk>& perks, Flags::Words& out) {
    Flags::pack(out, perks, [](const MarketPerk& p) { return p.purchased != 0; });
}

inline void unpackMarket(std::vector<MarketPerk>& perks, const Flags::Words& w) {
    Flags::unpack(w, perks, [](MarketPerk& p, bool on) { p.purchased = on ? 1 : 0; });
}

template <typename GS>
void buyMarketPerk(std::vector<MarketPerk>& perks, GS& gs, size_t index) {
    if (index >= perks.size()) return;
    MarketPerk& p = perks[index];
    if (p.purchased) return;
    if (gs.credits < p.cost) return;
    gs.credits   -= p.cost;
    p.purchased   = 1;
    packMarket(perks, gs.marketFlags);
}

[[nodiscard]] inline long double getMarketPerkMultiplier(
    MarketPerkType type,
    const std::vector<MarketPerk>& perks = marketPerks)
{
    long double mult = 1.0L;
    for (const MarketPerk& p : perks)
        if (p.purchased && p.type == type)
            mult *= p.multiplier;
    return mult;
}

[[nodiscard]] inline long double dataProdPerkMult()      { return getMarketPerkMultiplier(MarketPerkType::DATA_PROD);       }
[[nodiscard]] inline long double infoProdPerkMult()      { return getMarketPerkMultiplier(MarketPerkType::INFO_PROD);       }
[[nodiscard]] inline long double entropyProdPerkMult()   { return getMarketPerkMultiplier(MarketPerkType::ENTROPY_PROD);    }
[[nodiscard]] inline long double marketVolCapPerkMult()  { return getMarketPerkMultiplier(MarketPerkType::MARKET_VOL_CAP);  }
[[nodiscard]] inline long double dataSellValuePerkMult() { return getMarketPerkMultiplier(MarketPerkType::DATA_SELL_VALUE); }
[[nodiscard]] inline long double infoSellValuePerkMult() { return getMarketPerkMultiplier(MarketPerkType::INFO_SELL_VALUE); }