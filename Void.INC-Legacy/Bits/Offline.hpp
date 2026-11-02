#pragma once

#include "../Corruption/Corruption.hpp"
#include "../Misc/Globals/GIncludes.hpp"
#include "../Hardware/Logic.hpp"
#include "../Initialisation/Initialisation.hpp"
#include "../Misc/Globals/GVariables.hpp"

inline void offline(time_t timestamp, long double& bits, long double& allBits, long double bitsPerSecond, long double hotfixMult) {
    time_t elapsedTime = time(nullptr) - timestamp;
    if (elapsedTime <= 0) return;

    long double eBPS = 0.L;
    for (size_t i = 0; i < logicGateList.size(); ++i) {
        if (i < 2 && dirTree[5].patched) continue;
        if (i < 4 && dirTree[15].patched) continue;
        if (i < 7 && dirTree[21].patched) continue;

        long double indivMult = 1.0L;
        if (i < 7 && dirTree[6].patched) indivMult = std::min(1.0L + (logicGateList[i].ver * 0.05L), 50.0L);
        if (i >= 6 && dirTree[28].patched) indivMult = std::min(1.0L + (logicGateList[i].ver * 0.2L), 100.0L);

        eBPS += (logicGateList[i].bps * logicGateList[i].ver) * indivMult;
    }

    long double dirMult = 1.0L;
    const std::vector<std::pair<int, long double>> staticPatches = {
        {2, 3.0L},      // 2
        {3, 5.5L},      // 3_1
        {5, 12.0L},     // 4_1
        {7, 50.0L},     // 5_1
        {8, 100.0L},    // 5_2
        {9, 4.0L},      // 3
        {10, 8.0L},     // 4
        {11, 35.0L},    // 5
        {14, 60.0L},    // !
        {15, 999.0L},   // @
        {16, 250.0L},   // 6
        {17, 450.0L},   // 7
        {18, 600.0L},   // 7_1
        {20, 3.5L},     // 2_1
        {21, 6500.0L},  // !!
        {22, 22500.0L}, // A
        {24, 85000.0L}, // C
        {26, 400.0L},   // 6_2
        {27, 4.5L},     // 2_2
        {29, 175000.0L},// C_1
        {30, 950000.0L},// C_2
        {31, 5500000.0L}// C_3
    };

    for (const auto& [idx, mult] : staticPatches) {
        if (dirTree[idx].patched) dirMult *= mult;
    }

    long double dynamicMult = 1.0L;
    if (dirTree[32].patched) {
        dynamicMult = std::min(1.0L + (bytes * 0.1L), 10000000.0L);
    }
    else if (dirTree[19].patched && !dirTree[19].disabled) {
        dynamicMult = std::min(1.0L + (bytes * 0.01L), 3500.0L);
    }
    else if (dirTree[4].patched && !dirTree[4].disabled) {
        dynamicMult = std::min(1.0L + (bytes * 0.002L), 100.0L);
    }

    if (dirTree[1].patched && !dirTree[1].disabled) dirMult *= 1.6L * 1.65L; // Base 1.6x + Avg 1.65x patch_1Mult boost
    if (dirTree[12].patched && !dirTree[12].disabled) dirMult *= 1.5L;
    if (kernelTree[1].overwritten) dirMult *= std::min(1.0L + (malbits * 0.01L), 100000000.0L);

    long double offlineBits = (elapsedTime * eBPS * hotfixMult * dirMult * dynamicMult) * offlineMultiplier;

    long double malbitMult = kernelTree[4].overwritten ? 2.0L : 1.0L;
    long double offlineMalbits = (std::sqrt(offlineBits / 1e30L) * (1.f - offlineMultiplier)) * malbitMult;

    bits += offlineBits; allBits += offlineBits; accOfflineBits += offlineBits;
    if (offlineBits >= 1e30L) {
        malbits += offlineMalbits; allMalbits += offlineMalbits; accOfflineMalbits += offlineMalbits;
    }
}