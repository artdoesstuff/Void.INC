#pragma once

/**
 * @file Economy.hpp
 * @brief Void.INC — every economy tuning knob in one place.
 *
 * These numbers were set from a pacing simulation (tools/econ_sim.py), which
 * mirrors GameMechanics::tick and plays a simple greedy bot. Re-run it after
 * changing anything here or adding Supports / Perks / Tree nodes:
 *
 *   python3 tools/econ_sim.py hours=8
 *
 * Pacing targets the current values were tuned to (bot, not a human):
 *   ~1 min    Market unlocks (1 Credit of trade value becomes reachable)
 *   ~4 min    first Marketplace perk
 *   ~25 min   current Support ladder is essentially bought out
 *   ~55 min   first Fragmentation (+1 Fragment)
 *   then      runs of ~15-70 min that lengthen as thresholds climb
 *
 * Nothing in the economy is a 1:1 conversion: Fragment gain is a power law
 * (concave), the Fragment boost is a power law of lifetime Fragments, and
 * Information / Entropy are weighted by how scarce they are relative to Data.
 */

namespace Economy {
    // credits = amount_sold * market_multiplier * BASE * perk/tree multipliers
    inline constexpr double SELL_DATA_BASE = 1e-3;
    inline constexpr double SELL_INFO_BASE = 5e-3;   // Information is the scarcer of the two

    inline constexpr double VOID_RATE_BASE_MAX = 0.5;   // tree nodes can raise it (see GameMechanics::voidRateMax)

    // value  V = Data + FRAG_WEIGHT_INFO * Information + FRAG_WEIGHT_ENTROPY * Entropy
    // gain     = floor( (V / FRAG_THRESHOLD) ^ FRAG_GAIN_EXP * treeGainMult )
    // boost    = (1 + lifetime Fragments) ^ FRAG_BOOST_EXP   (multiplies Data production)
    inline constexpr double FRAG_THRESHOLD       = 6e7;
    inline constexpr double FRAG_WEIGHT_INFO     = 4.0;
    inline constexpr double FRAG_WEIGHT_ENTROPY  = 40.0;
    inline constexpr double FRAG_GAIN_EXP        = 0.45;
    inline constexpr double FRAG_BOOST_EXP       = 0.8;

    // stockScale = max(1, sqrt(lifetime Credits / STOCK_SCALE_DIVISOR)); see CreditMarket.hpp.
    inline constexpr double STOCK_SCALE_DIVISOR = 1e3;
}
