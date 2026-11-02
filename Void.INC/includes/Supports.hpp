#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "dependancies/ErisNum.hpp"
#include "Flags.hpp"


struct RESOURCE {
    enum class COST { CREDITS, DATA, INFORMATION, ENTROPY };
    enum class EXTRA_COST { NONE, CREDITS, DATA, INFORMATION, ENTROPY };
    enum class BOOST { DATA, INFORMATION, ENTROPY };
};

struct SupportData {
    std::string             name;
    RESOURCE::COST          resource;                       // Resource cost
    ErisNum::ErisNum        cost;                           // Cost value (1:1 with both resources)
    RESOURCE::BOOST         boost;                          // What resource will the upgrade boost
    RESOURCE::EXTRA_COST    extra = RESOURCE::EXTRA_COST::NONE;   // Extra resource Cost
    long double             boost_multiplier = 1.4L;
    uint8_t                 installed = 0;
};

// Support ladder — numbers set with tools/econ_sim.py (see Economy.hpp).
// Costs grow x4 per rung while each rung gives x1.4, so every purchase takes
// roughly 1.4x longer than the last instead of staying flat or exploding.
// Upgrade 2 is the early gate that introduces Entropy: it needs 50 of each.
// New entries go at the END of the list (flags are positional).
inline std::vector<SupportData> supports = {
    // ---- Data supports (cost Data, boost Data) ----
    { "Upgrade 1",  RESOURCE::COST::DATA, ErisNum::ErisNum(50),      RESOURCE::BOOST::DATA },
    { "Upgrade 2",  RESOURCE::COST::DATA, ErisNum::ErisNum(50),      RESOURCE::BOOST::DATA, RESOURCE::EXTRA_COST::ENTROPY },
    { "Upgrade 3",  RESOURCE::COST::DATA, ErisNum::ErisNum(200),     RESOURCE::BOOST::DATA },
    { "Upgrade 4",  RESOURCE::COST::DATA, ErisNum::ErisNum(800),     RESOURCE::BOOST::DATA },
    { "Upgrade 5",  RESOURCE::COST::DATA, ErisNum::ErisNum(3200),    RESOURCE::BOOST::DATA },
    { "Upgrade 6",  RESOURCE::COST::DATA, ErisNum::ErisNum(12800),   RESOURCE::BOOST::DATA },
    { "Upgrade 7",  RESOURCE::COST::DATA, ErisNum::ErisNum(51200),   RESOURCE::BOOST::DATA },
    { "Upgrade 8",  RESOURCE::COST::DATA, ErisNum::ErisNum(204800),  RESOURCE::BOOST::DATA },
    { "Upgrade 9",  RESOURCE::COST::DATA, ErisNum::ErisNum(819200),  RESOURCE::BOOST::DATA },
    { "Upgrade 10", RESOURCE::COST::DATA, ErisNum::ErisNum(3276800), RESOURCE::BOOST::DATA },

    // ---- Information supports (cost Information, boost Information) ----
    { "Index 1", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(20),    RESOURCE::BOOST::INFORMATION },
    { "Index 2", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(80),    RESOURCE::BOOST::INFORMATION },
    { "Index 3", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(320),   RESOURCE::BOOST::INFORMATION },
    { "Index 4", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(1280),  RESOURCE::BOOST::INFORMATION },
    { "Index 5", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(5120),  RESOURCE::BOOST::INFORMATION },
    { "Index 6", RESOURCE::COST::INFORMATION, ErisNum::ErisNum(20480), RESOURCE::BOOST::INFORMATION },

    // ---- Entropy supports (cost Entropy, boost Entropy recovered from the void) ----
    { "Chaos 1", RESOURCE::COST::ENTROPY, ErisNum::ErisNum(15),  RESOURCE::BOOST::ENTROPY },
    { "Chaos 2", RESOURCE::COST::ENTROPY, ErisNum::ErisNum(60),  RESOURCE::BOOST::ENTROPY },
    { "Chaos 3", RESOURCE::COST::ENTROPY, ErisNum::ErisNum(240), RESOURCE::BOOST::ENTROPY },
    { "Chaos 4", RESOURCE::COST::ENTROPY, ErisNum::ErisNum(960), RESOURCE::BOOST::ENTROPY },
};


inline void packSupport(const std::vector<SupportData>& ups, Flags::Words& out) {
    Flags::pack(out, ups, [](const SupportData& u) { return u.installed != 0; });
}

inline void unpackSupport(std::vector<SupportData>& ups, const Flags::Words& w) {
    Flags::unpack(w, ups, [](SupportData& u, bool on) { u.installed = on ? 1 : 0; });
}

// COST and EXTRA_COST are different enums with DIFFERENT underlying values
// (EXTRA_COST has NONE at 0), so they must never be compared via a raw
// static_cast<int>. Always go through these helpers.

[[nodiscard]] constexpr RESOURCE::COST extraToCost(RESOURCE::EXTRA_COST r) {
    switch (r) {
        case RESOURCE::EXTRA_COST::CREDITS:     return RESOURCE::COST::CREDITS;
        case RESOURCE::EXTRA_COST::DATA:        return RESOURCE::COST::DATA;
        case RESOURCE::EXTRA_COST::INFORMATION: return RESOURCE::COST::INFORMATION;
        case RESOURCE::EXTRA_COST::ENTROPY:     return RESOURCE::COST::ENTROPY;
        case RESOURCE::EXTRA_COST::NONE:        break;
    }
    return RESOURCE::COST::DATA; // unreachable for NONE — callers check first
}

[[nodiscard]] constexpr bool sameResource(RESOURCE::COST a, RESOURCE::EXTRA_COST b) {
    return b != RESOURCE::EXTRA_COST::NONE && extraToCost(b) == a;
}

// Resolve the balance for a cost resource. Works for const and non-const GS.
template <typename GS>
[[nodiscard]] inline auto& supportPool(GS& gs, RESOURCE::COST r) {
    switch (r) {
        case RESOURCE::COST::CREDITS:     return gs.credits;
        case RESOURCE::COST::DATA:        return gs.data;
        case RESOURCE::COST::INFORMATION: return gs.information;
        case RESOURCE::COST::ENTROPY:     return gs.entropy;
    }
    return gs.data;
}

// Primary-cost affordability only.
template <typename GS>
[[nodiscard]] inline bool canAffordSupportPrimary(const SupportData& u, const GS& gs) {
    return supportPool(gs, u.resource) >= u.cost;
}

// Extra-cost affordability. If the extra resource is the same as the primary
// one, the single pool must cover BOTH payments (2x cost).
template <typename GS>
[[nodiscard]] inline bool canAffordSupportExtra(const SupportData& u, const GS& gs) {
    if (u.extra == RESOURCE::EXTRA_COST::NONE) return true;
    if (sameResource(u.resource, u.extra))
        return supportPool(gs, u.resource) >= u.cost * ErisNum::ErisNum(2.0);
    return supportPool(gs, extraToCost(u.extra)) >= u.cost;
}

template <typename GS>
[[nodiscard]] inline bool canAffordSupport(const SupportData& u, const GS& gs) {
    return canAffordSupportPrimary(u, gs) && canAffordSupportExtra(u, gs);
}

template <typename GS>
void installSupport(std::vector<SupportData>& ups, GS& gs, size_t index) {
    if (index >= ups.size()) return;
    SupportData& u = ups[index];
    if (u.installed) return;
    if (!canAffordSupport(u, gs)) return;

    supportPool(gs, u.resource) -= u.cost;
    if (u.extra != RESOURCE::EXTRA_COST::NONE)
        supportPool(gs, extraToCost(u.extra)) -= u.cost;   // same pool => 2x total

    u.installed = 1;
    packSupport(ups, gs.supportFlags);
}

// Every installed support multiplies the production of whichever resource
// it boosts (RESOURCE_BOOST). Multiple installed supports targeting the
// same resource stack multiplicatively (x1.5 and x1.5 -> x2.25).

[[nodiscard]] inline ErisNum::ErisNum getBoostMultiplier(
    RESOURCE::BOOST resource,
    const std::vector<SupportData>& ups = supports)
{
    ErisNum::ErisNum mult(1.0);
    for (const SupportData& u : ups) {
        if (u.installed && u.boost == resource) {
            mult *= ErisNum::ErisNum(u.boost_multiplier);
        }
    }
    return mult;
}

// Convenience overloads — one per resource, so call sites read clearly.
[[nodiscard]] inline ErisNum::ErisNum dataBoostMultiplier(const std::vector<SupportData>& ups = supports) {
    return getBoostMultiplier(RESOURCE::BOOST::DATA, ups);
}
[[nodiscard]] inline ErisNum::ErisNum informationBoostMultiplier(const std::vector<SupportData>& ups = supports) {
    return getBoostMultiplier(RESOURCE::BOOST::INFORMATION, ups);
}
[[nodiscard]] inline ErisNum::ErisNum entropyBoostMultiplier(const std::vector<SupportData>& ups = supports) {
    return getBoostMultiplier(RESOURCE::BOOST::ENTROPY, ups);
}