#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

inline void save(
	time_t timestamp,
	// 1.0.x stuff
	long double bits,
	long double bytes,
	long double allBits,
	long double allClickedBits,
	long double allBytes,
	long double bitsPerSecond,
	long double hotfixMult,
	long long timesInitialised,
	// 1.1.x stuff
	long double malbits,
	long double malbytes,
	long double allMalbits,
	long double allMalbytes,
	long long timesCorrupted,
	CorruptionLevel corruption,

	std::vector<Logic>& logicGate,
	std::vector<Hotfix>& hotfix,
	std::vector<DirNodes>& root,
	std::vector<KernelNodes>& kernel
) {
	json saveData;

	auto round = [](long double v) -> long double {return std::round(v * 100.0) / 100.0; };

	saveData["T"] = timestamp;
	saveData["B"] = round(bits);
	saveData["BY"] = round(bytes);
	saveData["AB"] = round(allBits);
	saveData["ACB"] = round(allClickedBits);
	saveData["ABY"] = round(allBytes);
	saveData["BPS"] = round(bitsPerSecond);
	saveData["HFM"] = round(hotfixMult);
	saveData["TINIT"] = timesInitialised;

	saveData["MB"] = round(malbits);
	saveData["MBY"] = round(malbytes);
	saveData["AMB"] = round(allMalbits);
	saveData["AMBY"] = round(allMalbytes);
	saveData["TCRPT"] = timesCorrupted;
	saveData["CRPTL"] = corruption;

	saveData["LOGIC"] = logicGate;
	saveData["HOTFIX"] = hotfix;
	saveData["ROOT"] = root;
	saveData["KRNL"] = kernel;

	std::ofstream file("save.json");

	if (file.is_open()) file << std::setw(4) << saveData << std::endl; file.close();
	if (!file.is_open()) std::cerr << "Unable to open save.json." << std::endl;
}