#pragma once

/**
 * @file Infrastructure.hpp
 * @brief Void.INC — Infrastructure: a Credit-built grid of machines, plus Power.
 *
 * Theme: a data-centre on the edge of the void. Buildings are placed one per
 * cell on a square grid that you expand with Credits. GENERATORS make Power;
 * every other building DRAWS Power. When demand exceeds supply, every consumer
 * runs at efficiency = supply / demand (its bonus shrinks proportionally), so
 * an over-built grid degrades gracefully instead of switching off.
 *
 * Bonuses are multiplicative per building: n Data Nodes give (1+b)^n. Costs
 * rise 1.35x per building of the same kind. Infrastructure is permanent —
 * Fragmentation wipes Credits, not machines.
 *
 * Two buildings unlock AUTOMATION: the Autoloader installs Supports for you,
 * the Glitch Scheduler keeps ticked Glitches running.
 *
 * State (GameState): infraSize (grid is size x size), infraCells (one byte per
 * cell: 0 empty, else kind+1 — append new kinds at the END), autoSupports,
 * glitchAutoFlags (positional per Glitch).
 *
 * Every number in the table below is a balance knob.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include "dependancies/ErisNum.hpp"

enum class InfraEffect {
    POWER,            ///< generator: adds `value` Power
    DATA_PROD,        ///< x(1+value) Data per building
    INFO_PROD,        ///< x(1+value) Information gained
    ENTROPY_PROD,     ///< x(1+value) Entropy recovered
    SELL_VALUE,       ///< x(1+value) Credits from selling
    RESEARCH_SPEED,   ///< x(1+value) research speed
    AUTO_SUPPORTS,    ///< automation: install Supports
    AUTO_GLITCHES,    ///< automation: keep chosen Glitches running
};

struct InfraKind {
    std::string name;
    std::string desc;
    double      baseCost;    ///< Credits for the first one
    double      power;       ///< Power DRAWN (consumers); 0 for generators
    InfraEffect effect;
    double      value;
};

inline const std::vector<InfraKind> infraKinds = {
    { "Solar Array",       "Generates 5 Power.",                          150,   0, InfraEffect::POWER,          5.0  },
    { "Fusion Core",       "Generates 30 Power.",                         4000,  0, InfraEffect::POWER,          30.0 },
    { "Data Node",         "Data production +10% each.",                  200,   3, InfraEffect::DATA_PROD,      0.10 },
    { "Index Server",      "Information gained +10% each.",               250,   3, InfraEffect::INFO_PROD,      0.10 },
    { "Entropy Siphon",    "Entropy recovered +10% each.",                400,   4, InfraEffect::ENTROPY_PROD,   0.10 },
    { "Exchange Terminal", "Credits from selling +5% each.",              600,   4, InfraEffect::SELL_VALUE,     0.05 },
    { "Research Lab",      "Research speed +10% each.",                   800,   4, InfraEffect::RESEARCH_SPEED, 0.10 },
    { "Autoloader",        "Automation: installs Supports when affordable.", 3000, 6, InfraEffect::AUTO_SUPPORTS, 0.0 },
    { "Glitch Scheduler",  "Automation: keeps ticked Glitches running.",  3000,  6, InfraEffect::AUTO_GLITCHES,  0.0  },
};

namespace InfraRules {
    inline constexpr double COST_GROWTH   = 1.35;
    inline constexpr double REFUND        = 0.5;     // fraction of the current price returned on demolish
    inline constexpr int    START_SIZE    = 3;
    inline constexpr int    MAX_SIZE      = 8;
    inline constexpr double EXPAND_BASE   = 500.0;   // Credits for 3 -> 4
    inline constexpr double EXPAND_GROWTH = 5.0;     // each further size costs this much more
    inline constexpr double AUTO_PERIOD   = 1.0;     // seconds between automation passes
}

template <typename GS>
inline void ensureInfraGrid(GS& gs) {
    if (gs.infraSize < InfraRules::START_SIZE) gs.infraSize = InfraRules::START_SIZE;
    if (gs.infraSize > InfraRules::MAX_SIZE)   gs.infraSize = InfraRules::MAX_SIZE;
    gs.infraCells.resize(static_cast<size_t>(gs.infraSize) * gs.infraSize, 0);
    for (uint8_t& c : gs.infraCells) if (c > infraKinds.size()) c = 0;   // unknown kind
}

template <typename GS>
[[nodiscard]] inline size_t infraCount(const GS& gs, size_t kind) {
    size_t n = 0;
    for (uint8_t c : gs.infraCells) if (c == kind + 1) ++n;
    return n;
}

[[nodiscard]] inline double infraCostFor(size_t kind, size_t owned) {
    return infraKinds[kind].baseCost * std::pow(InfraRules::COST_GROWTH, static_cast<double>(owned));
}

template <typename GS>
[[nodiscard]] inline double infraCost(const GS& gs, size_t kind) {
    return infraCostFor(kind, infraCount(gs, kind));
}

[[nodiscard]] inline double cheapestInfraCost() {
    double c = infraKinds[0].baseCost;
    for (const InfraKind& k : infraKinds) c = std::min(c, k.baseCost);
    return c;
}

template <typename GS>
[[nodiscard]] inline double powerSupply(const GS& gs) {
    double p = 0.0;
    for (size_t k = 0; k < infraKinds.size(); ++k)
        if (infraKinds[k].effect == InfraEffect::POWER) p += infraKinds[k].value * static_cast<double>(infraCount(gs, k));
    return p;
}

template <typename GS>
[[nodiscard]] inline double powerDemand(const GS& gs) {
    double p = 0.0;
    for (size_t k = 0; k < infraKinds.size(); ++k)
        p += infraKinds[k].power * static_cast<double>(infraCount(gs, k));
    return p;
}

/// 1.0 when supply covers demand (or nothing draws), else supply/demand.
template <typename GS>
[[nodiscard]] inline double powerEfficiency(const GS& gs) {
    const double d = powerDemand(gs);
    if (d <= 0.0) return 1.0;
    return std::clamp(powerSupply(gs) / d, 0.0, 1.0);
}

/// Product over buildings of the effect, each building's bonus scaled by efficiency.
template <typename GS>
[[nodiscard]] inline double infraMult(const GS& gs, InfraEffect e) {
    const double eff = powerEfficiency(gs);
    double m = 1.0;
    for (size_t k = 0; k < infraKinds.size(); ++k) {
        if (infraKinds[k].effect != e) continue;
        const size_t n = infraCount(gs, k);
        if (n) m *= std::pow(1.0 + infraKinds[k].value * eff, static_cast<double>(n));
    }
    return m;
}

template <typename GS>
[[nodiscard]] inline bool infraHas(const GS& gs, InfraEffect e) {
    for (size_t k = 0; k < infraKinds.size(); ++k)
        if (infraKinds[k].effect == e && infraCount(gs, k) > 0) return true;
    return false;
}

template <typename GS>
[[nodiscard]] inline bool canPlace(const GS& gs, size_t cell, size_t kind) {
    return kind < infraKinds.size() && cell < gs.infraCells.size() && gs.infraCells[cell] == 0
        && gs.credits >= ErisNum::ErisNum(infraCost(gs, kind));
}

template <typename GS>
inline bool placeBuilding(GS& gs, size_t cell, size_t kind) {
    if (!canPlace(gs, cell, kind)) return false;
    gs.credits -= ErisNum::ErisNum(infraCost(gs, kind));
    gs.infraCells[cell] = static_cast<uint8_t>(kind + 1);
    return true;
}

/// Demolish returns REFUND of what the LAST one of that kind would cost now.
template <typename GS>
inline bool demolishBuilding(GS& gs, size_t cell) {
    if (cell >= gs.infraCells.size() || gs.infraCells[cell] == 0) return false;
    const size_t kind = gs.infraCells[cell] - 1u;
    const size_t owned = infraCount(gs, kind);
    gs.infraCells[cell] = 0;
    gs.credits += ErisNum::ErisNum(infraCostFor(kind, owned - 1) * InfraRules::REFUND);
    return true;
}

template <typename GS>
[[nodiscard]] inline bool canExpand(const GS& gs) { return gs.infraSize < InfraRules::MAX_SIZE; }

template <typename GS>
[[nodiscard]] inline double expandCost(const GS& gs) {
    return InfraRules::EXPAND_BASE * std::pow(InfraRules::EXPAND_GROWTH, static_cast<double>(gs.infraSize - InfraRules::START_SIZE));
}

template <typename GS>
inline bool expandGrid(GS& gs) {
    if (!canExpand(gs) || gs.credits < ErisNum::ErisNum(expandCost(gs))) return false;
    gs.credits -= ErisNum::ErisNum(expandCost(gs));
    const int old = gs.infraSize;
    std::vector<uint8_t> next(static_cast<size_t>(old + 1) * (old + 1), 0);
    for (int r = 0; r < old; ++r)
        for (int c = 0; c < old; ++c)
            next[static_cast<size_t>(r) * (old + 1) + c] = gs.infraCells[static_cast<size_t>(r) * old + c];
    gs.infraCells = std::move(next);
    gs.infraSize  = old + 1;
    return true;
}

/// Unlock latch: something is buildable (or already built).
template <typename GS>
[[nodiscard]] inline bool infraWorthShowing(const GS& gs) {
    for (uint8_t c : gs.infraCells) if (c) return true;
    return gs.credits >= ErisNum::ErisNum(cheapestInfraCost());
}
