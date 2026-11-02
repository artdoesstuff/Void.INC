#pragma once

/**
 * @file Research.hpp
 * @brief Void.INC — Research: timed, permanent techs.
 *
 * Like Supports you pay a resource, but a tech then takes TIME to finish.
 * One tech researches at a time. Techs are permanent — Fragmentation does not
 * reset them, and an in-progress tech keeps going.
 *
 * Research mainly improves the MARKET (sell value, price ceilings, sell
 * fraction cap, stock ranges). It can also nudge Supports by a few percent,
 * speed itself up, and lengthen Glitches.
 *
 * State: GameState::researchFlags (done, positional — append new techs at the
 * END), researchActive (index or -1) and researchElapsed (effective seconds).
 * Progress advances at researchSpeed(), so Glitches / the speed techs can
 * change the rate mid-research without breaking anything.
 *
 * Every number in the table below is a balance knob.
 */

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "Flags.hpp"
#include "Glitches.hpp"
#include "Infrastructure.hpp"
#include "Supports.hpp"     // RESOURCE::COST + supportPool()

enum class ResearchEffect {
    SELL_VALUE,        ///< multiplies Credits from selling Data / Information
    MARKET_CEIL,       ///< multiplies the Data / Information price ceiling
    SELL_FRAC_MAX,     ///< adds to the max sell fraction (additive)
    SUPPORT_BOOST,     ///< multiplies an installed Support line's total boost (small)
    RESEARCH_SPEED,    ///< multiplies research progress speed
    STOCK_RANGE,       ///< multiplies stock price headroom (takes effect from the next run)
    GLITCH_DURATION,   ///< multiplies how long Glitches last
};

struct ResearchNode {
    std::string         name;
    std::string         desc;
    RESOURCE::COST      resource;
    double              cost;
    double              seconds;      ///< at research speed 1.0
    std::vector<size_t> requires_;
    ResearchEffect      effect;
    double              value;
    uint8_t             done = 0;
};

inline std::vector<ResearchNode> researchTree = {
    // ---- Market ----
    { "Order Book",         "Data and Information sell for x1.10 Credits.",           RESOURCE::COST::CREDITS,     300,   90,  {},  ResearchEffect::SELL_VALUE,      1.10 },
    { "Depth Charts",       "Data / Info price ceiling x1.15.",                       RESOURCE::COST::CREDITS,     900,   180, {0}, ResearchEffect::MARKET_CEIL,     1.15 },
    { "Liquidity Pools",    "Max sell fraction +5%.",                                 RESOURCE::COST::CREDITS,     2500,  360, {0}, ResearchEffect::SELL_FRAC_MAX,   0.05 },
    { "Volatility Models",  "Stock price headroom x1.2 (from your next run).",        RESOURCE::COST::CREDITS,     8000,  600, {1}, ResearchEffect::STOCK_RANGE,     1.20 },
    // ---- Support / research ----
    { "Firmware Patch",     "Installed Supports are 2% stronger.",                    RESOURCE::COST::INFORMATION, 600,   240, {},  ResearchEffect::SUPPORT_BOOST,   1.02 },
    { "Parallel Compiler",  "Research speed x1.25.",                                  RESOURCE::COST::INFORMATION, 2500,  480, {4}, ResearchEffect::RESEARCH_SPEED,  1.25 },
    // ---- Entropy ----
    { "Cold Storage",       "Glitches last x1.25 as long.",                           RESOURCE::COST::ENTROPY,     40,    300, {4}, ResearchEffect::GLITCH_DURATION, 1.25 },
    { "Chaos Theory",       "Installed Supports are a further 3% stronger.",          RESOURCE::COST::ENTROPY,     150,   900, {6}, ResearchEffect::SUPPORT_BOOST,   1.03 },
};

inline void packResearch(const std::vector<ResearchNode>& nodes, Flags::Words& out) {
    Flags::pack(out, nodes, [](const ResearchNode& n) { return n.done != 0; });
}

inline void unpackResearch(std::vector<ResearchNode>& nodes, const Flags::Words& w) {
    Flags::unpack(w, nodes, [](ResearchNode& n, bool on) { n.done = on ? 1 : 0; });
}

[[nodiscard]] inline double researchProduct(ResearchEffect e, const std::vector<ResearchNode>& nodes = researchTree) {
    double m = 1.0;
    for (const ResearchNode& n : nodes)
        if (n.done && n.effect == e) m *= n.value;
    return m;
}

[[nodiscard]] inline double researchSum(ResearchEffect e, const std::vector<ResearchNode>& nodes = researchTree) {
    double s = 0.0;
    for (const ResearchNode& n : nodes)
        if (n.done && n.effect == e) s += n.value;
    return s;
}

[[nodiscard]] inline double researchSellMult()        { return researchProduct(ResearchEffect::SELL_VALUE);      }
[[nodiscard]] inline double researchCeilMult()        { return researchProduct(ResearchEffect::MARKET_CEIL);     }
[[nodiscard]] inline double researchSellFracBonus()   { return researchSum    (ResearchEffect::SELL_FRAC_MAX);   }
[[nodiscard]] inline double researchSupportMult()     { return researchProduct(ResearchEffect::SUPPORT_BOOST);   }
[[nodiscard]] inline double researchStockRangeMult()  { return researchProduct(ResearchEffect::STOCK_RANGE);     }
[[nodiscard]] inline double researchGlitchDurMult()   { return researchProduct(ResearchEffect::GLITCH_DURATION); }

[[nodiscard]] inline bool researchPrereqsMet(size_t i, const std::vector<ResearchNode>& nodes = researchTree) {
    if (i >= nodes.size()) return false;
    for (size_t r : nodes[i].requires_)
        if (r >= nodes.size() || !nodes[r].done) return false;
    return true;
}

template <typename GS>
[[nodiscard]] inline bool canAffordResearch(const GS& gs, size_t i, const std::vector<ResearchNode>& nodes = researchTree) {
    return i < nodes.size() && supportPool(gs, nodes[i].resource) >= ErisNum::ErisNum(nodes[i].cost);
}

template <typename GS>
[[nodiscard]] inline bool canStartResearch(const GS& gs, size_t i, const std::vector<ResearchNode>& nodes = researchTree) {
    return i < nodes.size() && gs.researchActive < 0 && !nodes[i].done
        && researchPrereqsMet(i, nodes) && canAffordResearch(gs, i, nodes);
}

template <typename GS>
inline bool startResearch(GS& gs, size_t i, std::vector<ResearchNode>& nodes = researchTree) {
    if (!canStartResearch(gs, i, nodes)) return false;
    supportPool(gs, nodes[i].resource) -= ErisNum::ErisNum(nodes[i].cost);
    gs.researchActive  = static_cast<int32_t>(i);
    gs.researchElapsed = 0.0;
    return true;
}

template <typename GS>
[[nodiscard]] inline double researchSpeed(const GS& gs) {
    return researchProduct(ResearchEffect::RESEARCH_SPEED) * glitchMult(gs, GlitchEffect::RESEARCH_SPEED)
         * infraMult(gs, InfraEffect::RESEARCH_SPEED);
}

/// [0,1] progress of the active research (0 when idle).
template <typename GS>
[[nodiscard]] inline double researchFraction(const GS& gs, const std::vector<ResearchNode>& nodes = researchTree) {
    if (gs.researchActive < 0 || static_cast<size_t>(gs.researchActive) >= nodes.size()) return 0.0;
    return std::clamp(gs.researchElapsed / nodes[static_cast<size_t>(gs.researchActive)].seconds, 0.0, 1.0);
}

/// Real seconds left at the current speed.
template <typename GS>
[[nodiscard]] inline double researchSecondsLeft(const GS& gs, const std::vector<ResearchNode>& nodes = researchTree) {
    if (gs.researchActive < 0 || static_cast<size_t>(gs.researchActive) >= nodes.size()) return 0.0;
    const double left = nodes[static_cast<size_t>(gs.researchActive)].seconds - gs.researchElapsed;
    return std::max(0.0, left) / std::max(researchSpeed(gs), 1e-9);
}

template <typename GS>
inline void tickResearch(GS& gs, double dt, std::vector<ResearchNode>& nodes = researchTree) {
    if (dt <= 0.0 || gs.researchActive < 0) return;
    if (static_cast<size_t>(gs.researchActive) >= nodes.size()) { gs.researchActive = -1; return; }   // stale save
    ResearchNode& n = nodes[static_cast<size_t>(gs.researchActive)];
    gs.researchElapsed += dt * researchSpeed(gs);
    if (gs.researchElapsed >= n.seconds) {
        n.done = 1;
        packResearch(nodes, gs.researchFlags);
        gs.researchActive  = -1;
        gs.researchElapsed = 0.0;
    }
}

/// True once there is something worth researching: anything done/active, or any
/// tech whose prerequisites are met and whose cost is covered.
template <typename GS>
[[nodiscard]] inline bool researchWorthShowing(const GS& gs, const std::vector<ResearchNode>& nodes = researchTree) {
    if (gs.researchActive >= 0) return true;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].done) return true;
        if (researchPrereqsMet(i, nodes) && canAffordResearch(gs, i, nodes)) return true;
    }
    return false;
}
