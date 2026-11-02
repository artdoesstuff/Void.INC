#pragma once

/**
 * @file Flags.hpp
 * @brief Void.INC — growable bit-flag storage for "owned / unlocked" state.
 *
 * Replaces the old fixed uint64_t masks (which capped Supports, Marketplace
 * perks and Achievements at 64 entries each). Flags live in a plain
 * std::vector<uint64_t>, which SimpleDat serializes natively, and grow on
 * demand — there is no entry cap.
 *
 * Flags are POSITIONAL: bit i belongs to the i-th entry of its list. Append
 * new entries at the END of a list, or bump GameState::kSaveVersion when
 * reordering / inserting (old saves are quarantined, not loaded).
 */

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Flags {
    using Words = std::vector<std::uint64_t>;

    [[nodiscard]] inline bool get(const Words& w, std::size_t i) {
        const std::size_t k = i >> 6;
        return k < w.size() && ((w[k] >> (i & 63)) & 1ULL) != 0;
    }

    inline void set(Words& w, std::size_t i, bool on) {
        const std::size_t k = i >> 6;
        if (k >= w.size()) w.resize(k + 1, 0ULL);
        if (on) w[k] |=  (1ULL << (i & 63));
        else    w[k] &= ~(1ULL << (i & 63));
    }

    /// Rebuild `out` from a list: bit i = flagOf(items[i]).
    template <typename Container, typename FlagOf>
    void pack(Words& out, const Container& items, FlagOf flagOf) {
        out.assign((items.size() + 63) / 64, 0ULL);
        for (std::size_t i = 0; i < items.size(); ++i)
            if (flagOf(items[i])) out[i >> 6] |= (1ULL << (i & 63));
    }

    /// Apply flags to a list: apply(items[i], bit i). Missing bits read as false.
    template <typename Container, typename Apply>
    void unpack(const Words& w, Container& items, Apply apply) {
        for (std::size_t i = 0; i < items.size(); ++i)
            apply(items[i], get(w, i));
    }
}
