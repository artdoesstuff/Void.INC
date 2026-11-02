#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Misc/Globals/GVariables.hpp"

inline void drawOfflineUI(sf::RenderWindow& window, sf::Vector2f& centre) {
	sf::Vector2f boxSize(600.f, 200.f);

	std::string text =
		"Connection restored. Analysis of bits harvested:\n"
		"The Star consumed -" + format(accOfflineBits) + " bits during your inactivity.\n"
		"The Void corrupted -" + format(accOfflineBits * (1.f - offlineMultiplier)) + " into -" + format(accOfflineMalbits) + " malbits.";

	drawTabBox(window, boxSize, "> void://bits/offline.bat", text,
		[&](sf::Vector2f startPos) {},
		[&](){ showOffline = false; playSFX("button"); });
}