#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

inline void saveOptions(bool renderEffects, bool quickStart, bool muteAll, bool muteSFX, bool muteAmbience) {
	std::ofstream file("options.txt");
	if (!file.is_open()) { std::cerr << "Unable to open options.txt.\n"; return; }

	file << renderEffects << '\n'
		 << quickStart << '\n'
		 << muteAll << '\n'
		 << muteSFX << '\n'
		 << muteAmbience << '\n';
}

inline void loadOptions(bool& renderEffects, bool& quickStart, bool& muteAll, bool& muteSFX, bool& muteAmbience) {
	std::ifstream file("options.txt");
	if (!file.is_open()) {
		std::cerr << "No options file found.\n";
		renderEffects = true;
		quickStart = muteAll = muteSFX = muteAmbience = false;
		return;
	}

	auto readBool = [&](bool& b) {
		std::string line;
		std::getline(file, line);
		b = (line == "1");
	};

	readBool(renderEffects);
	readBool(quickStart);
	readBool(muteAll);
	readBool(muteSFX);
	readBool(muteAmbience);
}