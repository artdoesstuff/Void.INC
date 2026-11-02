#pragma once
#include "GIncludes.hpp"

// Global references
inline std::string voidVersion = "v1.1.2-release"; // Version control for the game (Updater purposes)

inline sf::Texture screenSnapshot;
inline bool snapshotTaken = false;

inline bool showStart = true;
inline bool start = false;

inline bool reinitialisation = false;
inline bool initialisation = false;
inline bool showReinitialisationPopup = false;

inline bool corrupting = false;
inline bool showCorruptPopup = false;

inline bool showOffline = false;

inline float loadingProgress = 0.0f;
inline float timer = 0.0f;

inline sf::Clock deltaClock;
inline sf::Clock elapsedClock;
inline sf::Clock cooldown;
inline sf::Clock ambienceClock;
inline sf::Clock sessionClock;

enum class StartState {
    IDLE,
    TRANSITION
};
inline StartState currentStartStep = StartState::IDLE;

enum class ReinitState {
    IDLE,
    VORTEX_EXPANSION,
    VORTEX_SHRINK,
    LOADING_BAR,
    DIR
};
inline ReinitState currentReinitStep = ReinitState::IDLE;

enum class InitState {
    IDLE,
    LOADING_BAR,
	VORTEX_EXPANSION
};
inline InitState currentInitStep = InitState::IDLE;

enum class CorruptState {
    IDLE,
    CORRUPT,
    LOADING_BAR,
    KERNEL
};
inline CorruptState currentCorruptStep = CorruptState::IDLE;

// Click states
inline bool canClickStart = true; // Start menu only
inline bool canClick = false; // Pre-reinitialisation
inline bool canClickInit = false; // Post-initialisation
inline bool canClickOptions = false; // Options menu only
inline bool canClickCorrupt = false; // Post-corruption

enum class Tab {
    NONE,
    // Main screen Tabs
    LOGIC,
    HOTFIX,
    STATS,
    INIT,
    LOGS,
    // Directory Tabs
    REINIT,
    CORRUPT,
    // Root Tabs
    REBOOT
};
inline Tab activeTab = Tab::NONE;
inline float tabProgress = 0.f;

enum class Download { X1, X5, X10, X50, X100, MAX };
inline Download currentBuy = Download::X1;

inline float pi = 3.14159f;

// Dynamic shenanigans
inline float costMult = 1.0f; // Change the cost of all logicGates
inline float byteMultiplier = 1.0f; // Change the byte gain rate from reinit.bat specifically (does NOT affect conversion)

// Inflation multipliers
inline float logicGateInflation = 1.135f; // Inflates the cost of logicGate (base*(1.135^amt))
inline float hotfixGrowth = 1.1875f; // Recursive growth mult for hotfixes

// Important (and needed) data
inline long double bits = 0.0L; // Current bits
inline long double allBits = 0.0L; // Total accumulated bits
inline long double allClickedBits = 0.0L; // Total accumulated bits from clicks
inline long double accOfflineBits = 0.0L; // Bits accumulated while offline

inline long double hotfixMult = 1.0L; // Multiplier for hotfixes (upgrades)

inline long double bitsPerSecond = 0.0L; // Current bits per second
inline long double realBitsPerSecond = 0.0L;
inline long double bitsPerClick = 1.0L; // Bits per click
inline long double clickMultiplier = 1.0L; // Click multiplier

inline long double bytes = 0.0L; // Current bytes (from Re-initializing)
inline long double allBytes = 0.0L; // Total accumulated bytes
inline long long timesInitialised = 0LL; // Re-initializing is "Ascending"
inline long double bitsToBytesRate = 1e-8L; // Rate of bits to bytes conversion rate
inline long double bitMultiplier = 1.0L; // Multiplier for bits per second (from Directory patches)

inline long double bitsFromPatch = 0.0L; // Applies to Patch_!! (currently)

inline float patch_1Mult = 1.0f; // Multiplier from Patch_1
inline long double patch_3_2Mult = 1.0L; // Multiplier from Patch_3_2
inline long double patch_7_2Mult = 1.0L; // Multiplier from Patch_7_2
inline long double patch_C_4Mult = 1.0L; // Multiplier from Patch_C_4

inline long double mal_synergyMult = 1.0L; // Multiplier from MAL_SYNERGY

inline long double malbits = 0.0L; // Malicious bits (for 1.1 use)
inline long double allMalbits = 0.0L; // Total accumulated malbits
inline long double bitsToMalbitsRate = 1e-33L; // Rate of bits to malbits conversion
inline long double accOfflineMalbits = 0.0L; // Malbits accumulated while offline

inline long double malbytes = 0.0L; // Malicious bytes (for 1.1 use)
inline long double allMalbytes = 0.0L; // Total accumulated malbytes
inline long long timesCorrupted = 0LL; // Corrupting is "Transcending"
inline long double malbyteMultiplier = 1.0L; // Multiplier for bits per second (from Corrupting the kernel)

enum class CorruptionLevel { C5, C10, C15, C20, C25, C50, COUNT };
inline CorruptionLevel currentCorruption = CorruptionLevel::C5;

inline float getCorruptionValue(CorruptionLevel level) {
    switch (level) {
        case CorruptionLevel::C5:  return 0.05f;
        case CorruptionLevel::C10: return 0.10f;
        case CorruptionLevel::C15: return 0.15f;
        case CorruptionLevel::C20: return 0.20f;
        case CorruptionLevel::C25: return 0.25f;
        case CorruptionLevel::C50: return 0.50f;
        default: return 0.05f;
    }
}

inline float corruptOffline = getCorruptionValue(currentCorruption); // Base conversion for offline (Malbits)
inline float offlineMultiplier = 1.f - corruptOffline; // How much earned offline

inline sf::Font jetBrainsMono("Assets/Font/JetBrainsMono.ttf"); // Main font (kind of obvious)

// Options variables
inline bool showOptions = false; // Toggles the options menu

inline bool renderEffects = true; // Toggles rendering effects in the Void.INC/Effects folder
inline bool quickStart = false; // Toggles the start menu
inline bool muteAll = false; // Toggles all in-game sounds
inline bool muteSFX = false; // Toggles sound effects
inline bool muteAmbience = false; // Toggles all ambient sounds