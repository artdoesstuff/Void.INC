#pragma once

/**
 * @file FragmentTree.hpp
 * @brief Void.INC — the Fragmentation "Ascension" tree.
 *
 * Nodes are bought once with Data Fragments (held balance — spending them
 * does NOT lower the Fragment production boost, which uses the lifetime
 * total). The tree is permanent: Fragmentation never resets it.
 *
 * A node can be bought when every node in `requires` is owned (AND).
 * Owned state lives in GameState::fragmentTreeFlags (positional, Flags.hpp):
 * append new nodes at the END of the list, or bump GameState::kSaveVersion.
 *
 * x / y place each node in the main game area, normalised to [0,1].
 */

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "Flags.hpp"
#include "dependancies/ErisNum.hpp"

enum class TreeEffect {
    DATA_PROD,       ///< multiplies Data production
    INFO_PROD,       ///< multiplies Information gained
    ENTROPY_PROD,    ///< multiplies Entropy recovered from the void
    SELL_VALUE,      ///< multiplies Credits from selling Data / Information
    FRAGMENT_GAIN,   ///< multiplies Fragments gained per Fragmentation
    KEEP_SUPPORTS,   ///< first N Supports survive Fragmentation (largest value wins)
    VOID_MAX,        ///< adds to the void rate ceiling (additive)
};

struct TreeNode {
    std::string         name;
    std::string         desc;
    double              cost;
    std::vector<size_t> requires_;
    TreeEffect          effect;
    double              value;
    float               x = 0.5f;
    float               y = 0.5f;
    uint8_t             purchased = 0;
};

inline std::vector<TreeNode> fragmentTree = {
    // 0 — root
    { "Residual Signal",   "Data production x2.",                            1,  {},     TreeEffect::DATA_PROD,     2.0, 0.50f, 0.88f },
    // 1, 2 — first branches
    { "Echo Index",        "Information gained x2.",                         3,  {0},    TreeEffect::INFO_PROD,     2.0, 0.26f, 0.70f },
    { "Fault Line",        "Entropy recovered from the void x2.",            3,  {0},    TreeEffect::ENTROPY_PROD,  2.0, 0.74f, 0.70f },
    // 3, 4, 5
    { "Persistent Mounts", "Your first 2 Supports survive Fragmentation.",   8,  {1},    TreeEffect::KEEP_SUPPORTS, 2.0, 0.14f, 0.48f },
    { "Wider Void",        "Void rate ceiling +10%.",                        10, {2},    TreeEffect::VOID_MAX,      0.10, 0.86f, 0.48f },
    { "Broker Shards",     "Data and Information sell for x1.5 Credits.",    12, {1},    TreeEffect::SELL_VALUE,    1.5, 0.38f, 0.48f },
    // 6 needs BOTH 3 and 5; 7 continues the right branch
    { "Shard Resonance",   "Fragments gained per Fragmentation x1.5.",       25, {3, 5}, TreeEffect::FRAGMENT_GAIN, 1.5, 0.26f, 0.26f },
    { "Deep Fracture",     "Data production x3.",                            40, {4},    TreeEffect::DATA_PROD,     3.0, 0.74f, 0.26f },
};

inline void packTree(const std::vector<TreeNode>& nodes, Flags::Words& out) {
    Flags::pack(out, nodes, [](const TreeNode& n) { return n.purchased != 0; });
}

inline void unpackTree(std::vector<TreeNode>& nodes, const Flags::Words& w) {
    Flags::unpack(w, nodes, [](TreeNode& n, bool on) { n.purchased = on ? 1 : 0; });
}

[[nodiscard]] inline bool treePrereqsMet(size_t i, const std::vector<TreeNode>& nodes = fragmentTree) {
    if (i >= nodes.size()) return false;
    for (size_t r : nodes[i].requires_)
        if (r >= nodes.size() || !nodes[r].purchased) return false;
    return true;
}

template <typename GS>
[[nodiscard]] inline bool canBuyTreeNode(const GS& gs, size_t i, const std::vector<TreeNode>& nodes = fragmentTree) {
    if (i >= nodes.size() || nodes[i].purchased) return false;
    return treePrereqsMet(i, nodes) && gs.dataFragments >= ErisNum::ErisNum(nodes[i].cost);
}

template <typename GS>
inline bool buyTreeNode(GS& gs, size_t i, std::vector<TreeNode>& nodes = fragmentTree) {
    if (!canBuyTreeNode(gs, i, nodes)) return false;
    gs.dataFragments -= ErisNum::ErisNum(nodes[i].cost);
    nodes[i].purchased = 1;
    packTree(nodes, gs.fragmentTreeFlags);
    return true;
}

[[nodiscard]] inline double treeProduct(TreeEffect e, const std::vector<TreeNode>& nodes = fragmentTree) {
    double m = 1.0;
    for (const TreeNode& n : nodes)
        if (n.purchased && n.effect == e) m *= n.value;
    return m;
}

[[nodiscard]] inline double treeSum(TreeEffect e, const std::vector<TreeNode>& nodes = fragmentTree) {
    double s = 0.0;
    for (const TreeNode& n : nodes)
        if (n.purchased && n.effect == e) s += n.value;
    return s;
}

[[nodiscard]] inline double treeDataMult()         { return treeProduct(TreeEffect::DATA_PROD);     }
[[nodiscard]] inline double treeInfoMult()         { return treeProduct(TreeEffect::INFO_PROD);     }
[[nodiscard]] inline double treeEntropyMult()      { return treeProduct(TreeEffect::ENTROPY_PROD);  }
[[nodiscard]] inline double treeSellMult()         { return treeProduct(TreeEffect::SELL_VALUE);    }
[[nodiscard]] inline double treeFragmentGainMult() { return treeProduct(TreeEffect::FRAGMENT_GAIN); }
[[nodiscard]] inline double treeVoidMaxBonus()     { return treeSum(TreeEffect::VOID_MAX);          }

[[nodiscard]] inline size_t treeKeepSupports(const std::vector<TreeNode>& nodes = fragmentTree) {
    double best = 0.0;
    for (const TreeNode& n : nodes)
        if (n.purchased && n.effect == TreeEffect::KEEP_SUPPORTS) best = std::max(best, n.value);
    return static_cast<size_t>(best);
}
