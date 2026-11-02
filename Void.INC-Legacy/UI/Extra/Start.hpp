#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Misc/Globals/GAudio.hpp"
#include "../../UserData/Local/Saving.hpp"
#include "../../UserData/Local/Options.hpp"

inline void drawStartUI(sf::RenderWindow& window, sf::RenderStates& states, Star& star, float et, float dt, sf::Vector2f& sPos, sf::Vector2f& centre) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    float h = window.getSize().y;

    auto t  = [&](const std::string& str, int size, sf::Vector2f pos, sf::Color col = {243, 238, 225}) {
        sf::Text t(jetBrainsMono, str, size);
        t.setPosition(pos);
        t.setFillColor(col);
        if (!start) window.draw(t);
        return t.getGlobalBounds().contains(mousePos);
    };

    t("Void.INC", 124, {80, h * 0.05f});
    t(voidVersion, 28, {120, h * 0.15f + 20.f}, {140, 140, 140});

    bool pHover = t("Enter", 48, {80, h * 0.5f}, sf::Color(243, 238, 225));
    bool oHover = t("Options", 48, {80, h * 0.6f}, sf::Color(243, 238, 225));
    bool qHover = t("Quit", 48, {80, h * 0.7f}, sf::Color(243, 238, 225));

    if (pHover) t("Enter", 48, {80, h * 0.5f}, {200, 200, 30});
    if (oHover) t("Options", 48, {80, h * 0.6f}, {200, 200, 30});
    if (qHover) t("Quit", 48, {80, h * 0.7f}, {200, 200, 30});

    updateStar(star, sPos, et, 1.f, allBits);
    window.draw(star.star, states);

    if (renderEffects) {
        updateStream(window, sPos, dt, 2);
        for (auto& d : dataStream) window.draw(d.bit);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
        if (canClickStart) {
            if (pHover) { start = true; playSFX("button"); }
            if (oHover) { showOptions = true; canClickStart = false; canClickOptions = true; playSFX("button"); }
            if (qHover) {
                save(time(nullptr), bits, bytes, allBits, allClickedBits, allBytes, bitsPerSecond, hotfixMult, timesInitialised, malbits, malbytes, allMalbits, allMalbytes, timesCorrupted, currentCorruption, logicGateList, hotfixList, dirTree, kernelTree);
                saveOptions(renderEffects, quickStart, muteAll, muteSFX, muteAmbience);
                playSFX("button");
                window.close();
            }
        }
        if (showOptions) drawOptionsUI(window, showOptions, centre);
        cooldown.restart();
    }
}