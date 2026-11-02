#pragma once

/**
 * @file Glitches.hpp
 * @brief Void.INC — Glitches: short, Entropy-bought buffs ("potions").
 *
 * A Glitch is bought with Entropy (consumed) and runs for a fixed time. It
 * can boost a currency, speed up research, or push market VOLATILITY up or
 * down — it never changes a market's value or cost, only how wildly it moves.
 *
 * Buying a Glitch that is already running adds its duration on top (capped at
 * MAX_STACK x the duration, so a long idle session can't bank hours of buff).
 * Different Glitches stack with each other; two with the same effect multiply.
 *
 * Timers live in GameState::glitchTimers (seconds remaining, indexed like
 * the `glitches` list — append new Glitches at the END). Fragmentation clears
 * them.
 *
 * Every number in the table below is a balance knob.
 */

#include <algorithm>
#include <string>
#include <vector>
#include "dependancies/ErisNum.hpp"

enum class GlitchEffect {
    DATA_PROD,        ///< multiplies Data production
    INFO_PROD,        ///< multiplies Information gained
    RESEARCH_SPEED,   ///< multiplies research progress speed
    STOCK_VOL,        ///< scales stock / Voidcoin volatility (swings, not prices)
    TRADE_VOL,        ///< scales Data / Information market volatility
};

struct Glitch {
    std::string  name;
    std::string  desc;
    double       entropyCost;
    double       seconds;
    GlitchEffect effect;
    double       value;
};

inline const std::vector<Glitch> glitches = {
    { "Overclock",         "Data production x2.",                          20, 60,  GlitchEffect::DATA_PROD,      2.0 },
    { "Packet Storm",      "Information gained x2.",                       20, 60,  GlitchEffect::INFO_PROD,      2.0 },
    { "Time Dilation",     "Research runs 3x faster.",                     35, 120, GlitchEffect::RESEARCH_SPEED, 3.0 },
    { "Volatility Spike",  "Stocks swing x1.8 harder (prices unchanged).", 30, 90,  GlitchEffect::STOCK_VOL,      1.8 },
    { "Dampener",          "Stocks swing x0.4 as hard (prices unchanged).",30, 90,  GlitchEffect::STOCK_VOL,      0.4 },
    { "Signal Jam",        "Data / Info markets swing x1.8 harder.",       25, 90,  GlitchEffect::TRADE_VOL,      1.8 },
    { "Static",            "Data / Info markets swing x0.4 as hard.",      25, 90,  GlitchEffect::TRADE_VOL,      0.4 },
};

namespace GlitchRules {
    inline constexpr double MAX_STACK = 5.0;   // remaining time never exceeds MAX_STACK x one duration
}

template <typename GS>
inline void ensureGlitchTimers(GS& gs) {
    if (gs.glitchTimers.size() != glitches.size()) gs.glitchTimers.resize(glitches.size(), 0.0);
}

template <typename GS>
[[nodiscard]] inline double glitchRemaining(const GS& gs, size_t i) {
    return i < gs.glitchTimers.size() ? std::max(0.0, gs.glitchTimers[i]) : 0.0;
}

template <typename GS>
[[nodiscard]] inline bool glitchActive(const GS& gs, size_t i) { return glitchRemaining(gs, i) > 0.0; }

template <typename GS>
[[nodiscard]] inline bool anyGlitchActive(const GS& gs) {
    for (size_t i = 0; i < glitches.size(); ++i) if (glitchActive(gs, i)) return true;
    return false;
}

/// Product of every RUNNING Glitch with this effect (1.0 when none).
template <typename GS>
[[nodiscard]] inline double glitchMult(const GS& gs, GlitchEffect e) {
    double m = 1.0;
    for (size_t i = 0; i < glitches.size(); ++i)
        if (glitches[i].effect == e && glitchActive(gs, i)) m *= glitches[i].value;
    return m;
}

[[nodiscard]] inline double cheapestGlitchCost() {
    double c = glitches.empty() ? 0.0 : glitches[0].entropyCost;
    for (const Glitch& g : glitches) c = std::min(c, g.entropyCost);
    return c;
}

/// durationMult comes from research (longer Glitches); 1.0 = as listed.
template <typename GS>
[[nodiscard]] inline bool canBuyGlitch(const GS& gs, size_t i, double durationMult = 1.0) {
    if (i >= glitches.size()) return false;
    const double dur = glitches[i].seconds * durationMult;
    return gs.entropy >= ErisNum::ErisNum(glitches[i].entropyCost)
        && glitchRemaining(gs, i) < dur * GlitchRules::MAX_STACK - 1e-9;
}

template <typename GS>
inline bool buyGlitch(GS& gs, size_t i, double durationMult = 1.0) {
    if (!canBuyGlitch(gs, i, durationMult)) return false;
    ensureGlitchTimers(gs);
    const double dur = glitches[i].seconds * durationMult;
    gs.entropy -= ErisNum::ErisNum(glitches[i].entropyCost);
    gs.glitchTimers[i] = std::min(gs.glitchTimers[i] + dur, dur * GlitchRules::MAX_STACK);
    return true;
}

template <typename GS>
inline void tickGlitches(GS& gs, double dt) {
    if (dt <= 0.0) return;
    for (double& t : gs.glitchTimers) t = std::max(0.0, t - dt);
}

template <typename GS>
inline void clearGlitches(GS& gs) {
    for (double& t : gs.glitchTimers) t = 0.0;
}
