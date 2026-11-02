#pragma once

#include "../Misc/Globals/GIncludes.hpp"
#include "../Misc/Globals/GVariables.hpp"

inline void updateBits(float dt) {
    bitsToBytesRate = dirTree[0].patched ? 1e-6L : 1e-8L;
    if (dirTree[3].patched) bitsToBytesRate = 3e-7L;
    if (dirTree[16].patched) bitsToBytesRate = 5e-7L;

    bitMultiplier = 1.0L;
    clickMultiplier = 1.0L;
    bitsPerClick = 1.0L;
    costMult = 1.0f;
    byteMultiplier = 1.0f;

    patch_3_2Mult = 1.0L;
    patch_7_2Mult = 1.0L;
    patch_C_4Mult = 1.0L;
    mal_synergyMult = 1.0L;

    if (dirTree[1].patched && patch_1Clock.getElapsedTime().asSeconds() >= 30.f && !dirTree[1].disabled) {
        patch_1Mult = 1.5f + static_cast<float>(rand()) / (RAND_MAX / 0.2f);
        patch_1Clock.restart();
    }

    if (dirTree[1].patched && !dirTree[1].disabled) bitMultiplier *= 1.6L;
    if (dirTree[2].patched)  { bitMultiplier *= 3.0L; clickMultiplier *= 2.0L; }
    if (dirTree[3].patched)  bitMultiplier *= 5.5L;
    if (dirTree[5].patched)  { bitMultiplier *= 12.0L; clickMultiplier *= 3.0L; }
    if (dirTree[7].patched)  { bitMultiplier *= 50.0L; byteMultiplier *= 1.5f; dirTree[1].disabled = dirTree[12].disabled = dirTree[13].disabled = 1; }
    if (dirTree[8].patched)  { bitMultiplier *= 100.0L; clickMultiplier *= 5.0L; costMult *= 0.9f; }
    if (dirTree[9].patched)  bitMultiplier *= 4.0L;
    if (dirTree[10].patched) bitMultiplier *= 8.0L;
    if (dirTree[11].patched) bitMultiplier *= 35.0L;
    if (dirTree[12].patched && !dirTree[12].disabled) bitMultiplier *= 1.5L;
    if (dirTree[13].patched && !dirTree[13].disabled) clickMultiplier *= 1.5L;
    if (dirTree[14].patched) { bitMultiplier *= 60.0L; clickMultiplier *= 0.8L; byteMultiplier *= 1.75f; }
    if (dirTree[15].patched) { bitMultiplier *= 999.0L; clickMultiplier *= 0.5L; }
    if (dirTree[16].patched) bitMultiplier *= 250.0L;
    if (dirTree[17].patched) bitMultiplier *= 450.0L;
    if (dirTree[18].patched) bitMultiplier *= 600.0L;
    if (dirTree[20].patched) bitMultiplier *= 3.5L;
    if (dirTree[21].patched) bitMultiplier *= 6500.0L;
    if (dirTree[22].patched) bitMultiplier *= 22500.0L;
    if (dirTree[24].patched) bitMultiplier *= 85000.0L;
    if (dirTree[26].patched) bitMultiplier *= 400.0L;
    if (dirTree[27].patched) bitMultiplier *= 4.5L;
    if (dirTree[29].patched) bitMultiplier *= 175000.0L;
    if (dirTree[30].patched) bitMultiplier *= 950000.0L;
    if (dirTree[31].patched) bitMultiplier *= 5500000.0L;

    if (dirTree[32].patched) {
        patch_C_4Mult = std::min(1.0L + (bytes * 0.1L), 10000000.0L);
    } else if (dirTree[19].patched && !dirTree[19].disabled) {
        patch_7_2Mult = std::min(1.0L + (bytes * 0.01L), 3500.0L);
    } else if (dirTree[4].patched && !dirTree[4].disabled) {
        patch_3_2Mult = std::min(1.0L + (bytes * 0.002L), 100.0L);
    }

    if (kernelTree[1].overwritten) {
        mal_synergyMult = std::min(1.0L + (malbits * 0.01L), 100000000.0L);
    }

    bitsPerSecond = 0.L;
    for (size_t i = 0; i < logicGateList.size(); ++i) {
        if (i < 2 && dirTree[5].patched) continue;
        if (i < 4 && dirTree[15].patched) continue;
        if (i < 7 && dirTree[21].patched) continue;

        long double indivMult = 1.0L;
        if (i < 7 && dirTree[6].patched) indivMult = std::min(1.0L + (logicGateList[i].ver * 0.05L), 50.0L);
        if (i >= 6 && dirTree[28].patched) indivMult = std::min(1.0L + (logicGateList[i].ver * 0.2L), 100.0L);

        bitsPerSecond += (logicGateList[i].bps * logicGateList[i].ver) * indivMult;
    }

    realBitsPerSecond = (bitsPerSecond * hotfixMult * bitMultiplier * patch_1Mult)
                        * patch_3_2Mult * patch_7_2Mult * patch_C_4Mult
                        * mal_synergyMult + bitsFromPatch;

    bits += realBitsPerSecond * dt;
    allBits += realBitsPerSecond * dt;
}
