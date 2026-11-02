#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

inline void versionSave(const std::string& version) {
	std::ofstream file("V.json");
	if (!file.is_open()) { std::cerr << "Unable to open V.json.\n"; return; }

	json saveData;
	saveData["V"] = version;
	file << std::setw(4) << saveData << '\n';
}