#pragma once

/**
 * @file Achievements.hpp
 * @brief Void.INC — Achievements system.
 *
 * Achievements are pure milestones — no credit reward, just a flag the
 * player can chase. Each one has:
 *   condition  — which stat to watch (AchievementCondition enum)
 *   threshold  — ErisNum (or, for FINISHED_TUTORIAL/VOID_RATE, an
 *                ErisNum-wrapped scalar) value that must be reached/held
 *   name       — short display name
 *   desc       — one-line description shown under the name
 *
 * Unlock state is stored in GameState::achievementFlags (see Flags.hpp),
 * same pattern as supportFlags / marketFlags. No entry cap.
 *
 * Call checkAchievements(achievements, gs) every tick (or every few
 * seconds — it's cheap); it iterates and unlocks newly-met ones, then
 * syncs the mask.
 */

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include "dependancies/ErisNum.hpp"
#include "Flags.hpp"

enum class AchievementCondition {
    ALL_CREDIT_PRODUCED,
    ALL_DATA_PRODUCED,
    ALL_INFO_PRODUCED,
    ALL_ENTROPY_PRODUCED,
    ALL_DATA_FRAGMENTS_PRODUCED,
    ALL_FRAGMENT_ECHOES_PRODUCED,
    ALL_DATA_AXIOMS_PRODUCED,
    ALL_NULL_PRODUCED,
    ALL_HEAT_PRODUCED,
    ALL_CACHE_PRODUCED,

    HELD_CREDITS,
    HELD_DATA,
    HELD_INFORMATION,
    HELD_ENTROPY,
    HELD_DATA_FRAGMENTS,
    HELD_FRAGMENT_ECHOES,
    HELD_DATA_AXIOMS,
    HELD_NULL,
    HELD_HEAT,
    HELD_CACHE,

    FINISHED_TUTORIAL,          ///< gs.finishedTutorial (0/1)
    VOID_RATE,                  ///< gs.voidRate (double, current — not all-time)

    FRAGMENTATION_COUNT,        ///< gs.fragmentation  (Data Fragments resets)
    SINGULARITY_COUNT,          ///< gs.singularity    (Fragment Echoes resets)
    CONVERGENCE_COUNT,          ///< gs.convergence    (Data Axioms resets)     — unused for now
    NULLIFICATION_COUNT,        ///< gs.nullification  (Null resets)            — unused for now

    STOCK_PNL_TOTAL,            ///< sum of positive stock/Voidcoin P&L (lifetime profit)
};

struct Achievement {
    std::string          name;
    std::string          desc;
    AchievementCondition condition;
    ErisNum::ErisNum     threshold;     ///< unlock when stat >= threshold
    uint8_t              unlocked = 0;
};

inline std::vector<Achievement> achievements = {
    { "First Earnings",     "Produce 100 Credits total.",          AchievementCondition::ALL_CREDIT_PRODUCED, ErisNum::ErisNum(100.0) },
    { "Steady Income",      "Produce 10,000 Credits total.",       AchievementCondition::ALL_CREDIT_PRODUCED, ErisNum::ErisNum(10'000.0) },
    { "Corporate Ledger",   "Produce 1,000,000 Credits total.",    AchievementCondition::ALL_CREDIT_PRODUCED, ErisNum::ErisNum(1'000'000.0) },
    { "Economy of Scale",   "Produce 1e12 Credits total.",         AchievementCondition::ALL_CREDIT_PRODUCED, ErisNum::ErisNum(1e12) },

    { "First Packet",       "Produce your first unit of Data.",    AchievementCondition::ALL_DATA_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Data Hoarder",       "Produce 1,000 Data total.",           AchievementCondition::ALL_DATA_PRODUCED, ErisNum::ErisNum(1'000.0) },
    { "Megabyte Mind",      "Produce 1,000,000 Data total.",       AchievementCondition::ALL_DATA_PRODUCED, ErisNum::ErisNum(1'000'000.0) },
    { "Petabyte Phantom",   "Produce 1e15 Data total.",            AchievementCondition::ALL_DATA_PRODUCED, ErisNum::ErisNum(1e15) },
    { "Exabyte Entity",     "Produce 1e18 Data total.",            AchievementCondition::ALL_DATA_PRODUCED, ErisNum::ErisNum(1e18) },

    { "Signal Detected",       "Produce your first unit of Information.", AchievementCondition::ALL_INFO_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Pattern Recognition",   "Produce 1,000 Information total.",        AchievementCondition::ALL_INFO_PRODUCED, ErisNum::ErisNum(1'000.0) },
    { "Inference Engine",      "Produce 1,000,000 Information total.",    AchievementCondition::ALL_INFO_PRODUCED, ErisNum::ErisNum(1'000'000.0) },

    { "Noise in the System",  "Produce your first unit of Entropy.",     AchievementCondition::ALL_ENTROPY_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Chaos Architect",      "Produce 10,000 Entropy total.",           AchievementCondition::ALL_ENTROPY_PRODUCED, ErisNum::ErisNum(10'000.0) },
    { "Entropy Engine",       "Produce 1,000,000 Entropy total.",        AchievementCondition::ALL_ENTROPY_PRODUCED, ErisNum::ErisNum(1'000'000.0) },

    { "Fragment Found",       "Produce your first Data Fragment.",       AchievementCondition::ALL_DATA_FRAGMENTS_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Fragment Collector",   "Produce 100 Data Fragments total.",       AchievementCondition::ALL_DATA_FRAGMENTS_PRODUCED, ErisNum::ErisNum(100.0) },
    { "Fragment Hoard",       "Produce 1,000,000 Data Fragments total.", AchievementCondition::ALL_DATA_FRAGMENTS_PRODUCED, ErisNum::ErisNum(1'000'000.0) },

    { "First Echo",           "Produce your first Fragment Echo.",       AchievementCondition::ALL_FRAGMENT_ECHOES_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Echo Chamber",         "Produce 100 Fragment Echoes total.",      AchievementCondition::ALL_FRAGMENT_ECHOES_PRODUCED, ErisNum::ErisNum(100.0) },
    { "Resonance Cascade",    "Produce 1,000,000 Fragment Echoes total.",AchievementCondition::ALL_FRAGMENT_ECHOES_PRODUCED, ErisNum::ErisNum(1'000'000.0) },

    { "First Axiom",          "Produce your first Data Axiom.",          AchievementCondition::ALL_DATA_AXIOMS_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Axiomatic",            "Produce 100 Data Axioms total.",          AchievementCondition::ALL_DATA_AXIOMS_PRODUCED, ErisNum::ErisNum(100.0) },

    { "Touch the Null",       "Produce your first unit of Null.",        AchievementCondition::ALL_NULL_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Null and Void",        "Produce 100 Null total.",                 AchievementCondition::ALL_NULL_PRODUCED, ErisNum::ErisNum(100.0) },

    { "Warming Up",           "Produce your first unit of Heat.",        AchievementCondition::ALL_HEAT_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Running Hot",          "Produce 1,000 Heat total.",               AchievementCondition::ALL_HEAT_PRODUCED, ErisNum::ErisNum(1'000.0) },
    { "Thermal Maximum",      "Produce 2,500 Heat total (the cap).",     AchievementCondition::ALL_HEAT_PRODUCED, ErisNum::ErisNum(2'500.0) },

    { "Cached Away",          "Produce your first unit of Cache.",       AchievementCondition::ALL_CACHE_PRODUCED, ErisNum::ErisNum(1.0) },
    { "Deep Cache",           "Produce 1,000 Cache total.",              AchievementCondition::ALL_CACHE_PRODUCED, ErisNum::ErisNum(1'000.0) },

    { "Pocket Change",        "Hold 100 Credits at once.",               AchievementCondition::HELD_CREDITS, ErisNum::ErisNum(100.0) },
    { "Liquid Assets",        "Hold 10,000 Credits at once.",            AchievementCondition::HELD_CREDITS, ErisNum::ErisNum(10'000.0) },
    { "Venture Capital",      "Hold 1,000,000 Credits at once.",         AchievementCondition::HELD_CREDITS, ErisNum::ErisNum(1'000'000.0) },

    { "Data Reserve",         "Hold 10,000 Data at once.",               AchievementCondition::HELD_DATA, ErisNum::ErisNum(10'000.0) },
    { "Information Reserve",  "Hold 10,000 Information at once.",        AchievementCondition::HELD_INFORMATION, ErisNum::ErisNum(10'000.0) },
    { "Entropy Reserve",      "Hold 10,000 Entropy at once.",            AchievementCondition::HELD_ENTROPY, ErisNum::ErisNum(10'000.0) },

    { "Fragments in Hand",    "Hold 50 Data Fragments at once.",         AchievementCondition::HELD_DATA_FRAGMENTS,  ErisNum::ErisNum(50.0) },
    { "Echoes in Hand",       "Hold 50 Fragment Echoes at once.",        AchievementCondition::HELD_FRAGMENT_ECHOES, ErisNum::ErisNum(50.0) },
    { "Axioms in Hand",       "Hold 50 Data Axioms at once.",            AchievementCondition::HELD_DATA_AXIOMS,     ErisNum::ErisNum(50.0) },
    { "Null in Hand",         "Hold 50 Null at once.",                   AchievementCondition::HELD_NULL,            ErisNum::ErisNum(50.0) },

    { "Building Heat",        "Hold 750 Heat at once.",                  AchievementCondition::HELD_HEAT,  ErisNum::ErisNum(750.0) },
    { "Stockpiled Cache",     "Hold 500 Cache at once.",                 AchievementCondition::HELD_CACHE, ErisNum::ErisNum(500.0) },

    { "Orientation Complete", "Finish the tutorial.",                    AchievementCondition::FINISHED_TUTORIAL, ErisNum::ErisNum(1.0) },

    { "Touched by Void",      "Set your void rate above 0%.",            AchievementCondition::VOID_RATE, ErisNum::ErisNum(0.01) },
    { "Halfway Gone",         "Reach 25% void rate.",                    AchievementCondition::VOID_RATE, ErisNum::ErisNum(0.25) },
    { "Half Consumed",        "Reach the maximum 50% void rate.",        AchievementCondition::VOID_RATE, ErisNum::ErisNum(0.50) },

    { "First Fracture",       "Undergo your first Fragmentation.",       AchievementCondition::FRAGMENTATION_COUNT, ErisNum::ErisNum(1.0) },
    { "Fractured Again",      "Undergo 10 Fragmentations.",              AchievementCondition::FRAGMENTATION_COUNT, ErisNum::ErisNum(10.0) },

    { "Singularity Achieved", "Complete your first Singularity.",        AchievementCondition::SINGULARITY_COUNT, ErisNum::ErisNum(1.0) },

    // Convergence / Nullification have no production path yet — these sit
    // locked until that content exists, same pattern as the rest.
    { "Convergence Point",    "Undergo your first Convergence.",         AchievementCondition::CONVERGENCE_COUNT, ErisNum::ErisNum(1.0) },
    { "Nullified",            "Undergo your first Nullification.",       AchievementCondition::NULLIFICATION_COUNT, ErisNum::ErisNum(1.0) },

    { "First Trade",          "Earn any Credits from selling stocks or Voidcoin.",      AchievementCondition::STOCK_PNL_TOTAL, ErisNum::ErisNum(1.0) },
    { "Bull Run",             "Earn 100,000 Credits total from stock/Voidcoin sells.",  AchievementCondition::STOCK_PNL_TOTAL, ErisNum::ErisNum(100'000.0) },
    { "Market Maker",         "Earn 1e9 Credits total from stock/Voidcoin sells.",      AchievementCondition::STOCK_PNL_TOTAL, ErisNum::ErisNum(1'000'000'000.0) },
};

inline void packAchievements(const std::vector<Achievement>& achs, Flags::Words& out) {
    Flags::pack(out, achs, [](const Achievement& a) { return a.unlocked != 0; });
}

inline void unpackAchievements(std::vector<Achievement>& achs, const Flags::Words& w) {
    Flags::unpack(w, achs, [](Achievement& a, bool on) { a.unlocked = on ? 1 : 0; });
}

template <typename GS>
[[nodiscard]] inline ErisNum::ErisNum resolveCondition(
    const GS& gs, AchievementCondition cond)
{
    switch (cond) {
        case AchievementCondition::ALL_CREDIT_PRODUCED:         return gs.allCreditProduced;
        case AchievementCondition::ALL_DATA_PRODUCED:           return gs.allDataProduced;
        case AchievementCondition::ALL_INFO_PRODUCED:           return gs.allInformationProduced;
        case AchievementCondition::ALL_ENTROPY_PRODUCED:        return gs.allEntropyProduced;
        case AchievementCondition::ALL_DATA_FRAGMENTS_PRODUCED: return gs.allDataFragments;
        case AchievementCondition::ALL_FRAGMENT_ECHOES_PRODUCED:return gs.allFragmentEchoes;
        case AchievementCondition::ALL_DATA_AXIOMS_PRODUCED:    return gs.allDataAxioms;
        case AchievementCondition::ALL_NULL_PRODUCED:           return gs.allNull;
        case AchievementCondition::ALL_HEAT_PRODUCED:           return gs.allHeat;
        case AchievementCondition::ALL_CACHE_PRODUCED:          return gs.allCache;

        case AchievementCondition::HELD_CREDITS:         return gs.credits;
        case AchievementCondition::HELD_DATA:             return gs.data;
        case AchievementCondition::HELD_INFORMATION:      return gs.information;
        case AchievementCondition::HELD_ENTROPY:          return gs.entropy;
        case AchievementCondition::HELD_DATA_FRAGMENTS:   return gs.dataFragments;
        case AchievementCondition::HELD_FRAGMENT_ECHOES:  return gs.fragmentEchoes;
        case AchievementCondition::HELD_DATA_AXIOMS:      return gs.dataAxioms;
        case AchievementCondition::HELD_NULL:             return gs.null;
        case AchievementCondition::HELD_HEAT:             return gs.heat;
        case AchievementCondition::HELD_CACHE:            return gs.cache;

        case AchievementCondition::FINISHED_TUTORIAL:
            return ErisNum::ErisNum(static_cast<double>(gs.finishedTutorial));
        case AchievementCondition::VOID_RATE:
            return ErisNum::ErisNum(gs.voidRate);

        case AchievementCondition::FRAGMENTATION_COUNT:   return gs.fragmentation;
        case AchievementCondition::SINGULARITY_COUNT:     return gs.singularity;
        case AchievementCondition::CONVERGENCE_COUNT:     return gs.convergence;
        case AchievementCondition::NULLIFICATION_COUNT:   return gs.nullification;

        case AchievementCondition::STOCK_PNL_TOTAL: {
            // Sum of all per-market P&L values that are currently positive.
            // P&L tracks net (sells - buys), so this approximates lifetime
            // profit; a position still underwater doesn't count against it.
            ErisNum::ErisNum total(0.0);
            auto addIfPositive = [&](const ErisNum::ErisNum& v) {
                if (!v.isNegative()) total += v;
            };
            addIfPositive(gs.stockPnLA);
            addIfPositive(gs.stockPnLB);
            addIfPositive(gs.stockPnLC);
            addIfPositive(gs.voidcoinPnL);
            return total;
        }
    }
    return ErisNum::ErisNum(0.0);
}

template <typename GS>
inline void checkAchievements(std::vector<Achievement>& achs, GS& gs) {
    bool any = false;
    for (Achievement& a : achs) {
        if (a.unlocked) continue;
        if (resolveCondition(gs, a.condition) >= a.threshold) {
            a.unlocked = 1;
            any = true;
        }
    }
    if (any)
        packAchievements(achs, gs.achievementFlags);
}

// Returns [0.0, 1.0]; log-scaled so huge thresholds don't sit at 0% forever.
template <typename GS>
[[nodiscard]] inline float achievementProgress(
    const std::vector<Achievement>& achs,
    const GS& gs,
    size_t idx)
{
    if (idx >= achs.size()) return 0.f;
    if (achs[idx].unlocked) return 1.f;
    const ErisNum::ErisNum val = resolveCondition(gs, achs[idx].condition);
    if (val <= ErisNum::ErisNum(0.0)) return 0.f;
    const ErisNum::ErisNum& thr = achs[idx].threshold;
    if (thr <= ErisNum::ErisNum(0.0)) return 1.f;
    // True linear ratio — val/threshold — so the bar accurately reflects
    // how close the player actually is, rather than a log-compressed
    // approximation that reads as "almost there" far too early.
    double ratio = (val / thr).toDouble();
    return std::clamp(static_cast<float>(ratio), 0.f, 1.f);
}