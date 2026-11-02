#pragma once

#include "../Misc/Globals/GIncludes.hpp"
#include "../Misc/Globals/GFunctions.hpp"
#include "../Misc/Globals/GVariables.hpp"
#include "../Hardware/Logic.hpp"
#include "../Hardware/Hotfixes.hpp"
#include "../Initialisation/Initialisation.hpp"
#include "../UI/Core/Directory.hpp"

struct KernelNodes {
    std::string name;
    std::string desc;
    long double malbytes;
    int overwritten = 0;
    sf::CircleShape nodeCircle;
    sf::Vector2f pos;

    KernelNodes(std::string n, std::string t, long double c)
        : name(n), desc(t), malbytes(c) {
        nodeCircle.setRadius(25.f);
        nodeCircle.setOrigin({ 25.f, 25.f });
        nodeCircle.setPosition(pos);
        nodeCircle.setFillColor(sf::Color(20, 5, 5));
        nodeCircle.setOutlineColor(sf::Color(255, 50, 50));
        nodeCircle.setOutlineThickness(2.f);
    }
};

inline std::vector<KernelNodes> kernelTree;

inline void initKernelTree() {
    kernelTree.emplace_back("SYS_BREACH", "Gain full access to the kernel.", 10000.0L);
    kernelTree.emplace_back("NULL_THREAD", "x2 Malbits.", 50000L);
    kernelTree.emplace_back("DATA_SCAVENGER", "Logic don't reset on Reinitialisation.", 1e6L);
    kernelTree.emplace_back("MAL_SYNERGY", "Malbit-scaled loop (+1% per Malbit @ 10B% Cap.", 5e12L);
    kernelTree.emplace_back("INIT_OVERRIDE", "Start with 100M Bits.", 25000.0L);
}

inline void corrupt(sf::RenderWindow& window) {
    logicGateList.clear();
    hotfixList.clear();
    dirTree.clear();

    initLogicGates();
    initHotfixes();
    initDirTree();

	positionTreeNodes(window.getSize());

    bits = kernelTree[4].overwritten ? 1e8L : 0.0L;
    bytes = 0.0L;
    malbits = 0.0L;

    bitsPerSecond = 0.0L;
    bitsFromPatch = 0.0L;

    hotfixMult = 1.0L;

    patch_1Mult = 1.0f;
    patch_3_2Mult = 1.0L;
    patch_7_2Mult = 1.0L;
    patch_C_4Mult = 1.0L;
}

inline long double getPendingMalbytes(long double bytes, long double malbits) {
    return std::round((std::sqrt(std::max(0.0L, bytes / 1e66L)) * (1.0L + std::sqrt(std::max(0.0L, malbits / 100.0L)))) * 100.0L) / 100.0L;
}

inline void drawCorruptPopup(sf::RenderWindow& window, bool& startCorrupt, sf::Vector2f& centre) {
    sf::Vector2f boxSize(600.f, 220.f);

    long double pending = getPendingMalbytes(bytes, malbits);

    auto triggerCorrupt = [&]() {
        startCorrupt = true;
        reinitialisation = false;
		    initialisation = false;

        malbytes += pending;
        allMalbytes += pending;
        corrupt(window);
        timesCorrupted++;

        activeTab = Tab::NONE;
		currentReinitStep = ReinitState::IDLE;

        showCorruptPopup = false;
        canClickCorrupt = true;
    };

    auto cancelCorrupt = [&]() {
        activeTab = Tab::NONE;
        showCorruptPopup = false;
    };

    std::string text = "WARNING: Unknown File Publisher.\n"
        "Unverified contents detected. Execution may result in\n"
        "EXTREME CONSEQUENCES. Terminal system integrity at risk.\n\n"
        + format(bytes) + " Bytes will get corrupted into -" + format(pending, true) + " Malbytes.\n\n"
        "Proceed with operation? (Y/N) > ";

    drawTabBox(window, boxSize, "> void://root/not_sus.bat", text, [&](sf::Vector2f startPos) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y)) { triggerCorrupt(); playSFX("button"); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) { cancelCorrupt(); playSFX("button"); }
    }, [&](){ cancelCorrupt(); });
}

inline void to_json(json& j, const KernelNodes& kn) {
    j = json{ {"n", kn.name}, {"o", kn.overwritten} };
}

inline void from_json(const json& j, KernelNodes& kn) {
    j.at("n").get_to(kn.name);
    j.at("o").get_to(kn.overwritten);
}