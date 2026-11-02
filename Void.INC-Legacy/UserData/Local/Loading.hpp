#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

inline void load(
	time_t& timestamp,
	// 1.0.x stuff
	long double& bits,
	long double& bytes,
	long double& allBits,
	long double& allClickedBits,
	long double& allBytes,
	long double& bitsPerSecond,
	long double& hotfixMult,
	long long& timesInitialised,
	// 1.1.x stuff
	long double& malbits,
	long double& malbytes,
	long double& allMalbits,
	long double& allMalbytes,
	long long& timesCorrupted,
	CorruptionLevel& corruption,

	std::vector<Logic>& logicGate,
	std::vector<Hotfix>& hotfix,
	std::vector<DirNodes>& root,
	std::vector<KernelNodes>& kernel
) {
	std::ifstream file("save.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "No save file found." << std::endl;
		timestamp = time(nullptr);
		return;
	}

	json saveData; file >> saveData;

	timestamp = saveData.value("T", time(nullptr));
	bits = saveData.value("B", 0.0L);;
	bytes = saveData.value("BY", 0.0L);
	allBits = saveData.value("AB", 0.0L);
	allClickedBits = saveData.value("ACB", 0.0L);
	allBytes = saveData.value("ABY", 0.0L);
	timesInitialised = saveData.value("TINIT", 0LL);

	malbits = saveData.value("MB", 0.L);
	malbytes = saveData.value("MBY", 0.L);
	allMalbits = saveData.value("AMB", 0.L);
	allMalbytes = saveData.value("AMBY", 0.L);
	timesCorrupted = saveData.value("TCRPT", 0LL);
	int rawCorruption = saveData.value("CRPTL", 0);

	if (rawCorruption >= 0 && rawCorruption < static_cast<int>(CorruptionLevel::COUNT)) {
		corruption = static_cast<CorruptionLevel>(rawCorruption);
	}
	else {
		corruption = CorruptionLevel::C5;
	}

	bitsPerSecond = 0.0L;
	hotfixMult = 1.0L;

	if (saveData.contains("ROOT") && saveData["ROOT"].is_array()) {
        for (const auto& item : saveData["ROOT"]) {
            for (auto& actualPatch : dirTree) {
                if (item.contains("n") && actualPatch.name == item["n"]) {
                    actualPatch.patched = item.value("p", 0);
                }
            }
        }
    }

    if (saveData.contains("KRNL") && saveData["KRNL"].is_array()) {
        for (const auto& item : saveData["KRNL"]) {
            for (auto& actualOverwrite : kernelTree) {
                if (item.contains("n") && actualOverwrite.name == item["n"]) {
                    actualOverwrite.overwritten = item.value("o", 0);
                }
            }
        }
    }

    costMult = 1.0L;
    if (dirTree[8].patched)  costMult *= 0.90f;
    if (dirTree[11].patched) costMult *= 0.95f;
    if (dirTree[23].patched) costMult *= 0.85f;
    if (dirTree[31].patched) costMult *= 0.85f;

    if (saveData.contains("LOGIC") && saveData["LOGIC"].is_array()) {
        for (const auto& item : saveData["LOGIC"]) {
            std::string name = item.value("n", "");
            int ver = item.value("v", 0);

            for (auto& lg : logicGate) {
                if (lg.name == name) {
                    lg.ver = ver;
                    lg.currentCost = lg.baseCost * std::pow(logicGateInflation, lg.ver) * costMult;
                    bitsPerSecond += (lg.ver * lg.bps);
                    break;
                }
            }
        }
    }

    if (saveData.contains("HOTFIX") && saveData["HOTFIX"].is_array()) {
        for (const auto& item : saveData["HOTFIX"]) {
            std::string name = item.value("n", "");
            int isWritten = item.value("w", 0);

            for (auto& hf : hotfix) {
                if (hf.name == name) {
                    hf.written = isWritten;
                    if (hf.written == 1) hotfixMult += hf.bitMult;
                    break;
                }
            }
        }
    }
}