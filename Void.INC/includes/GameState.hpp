#pragma once

#include <cstdint>
#include <vector>
#include "dependancies/ErisNum.hpp"
#include "dependancies/SimpleDat.hpp"

struct GameState {
    // ---- Save header -----------------------------------------------------
    // Written first. A file whose magic/version differ is never loaded as a
    // live save; GameMechanics::loadOrNew quarantines it instead.
    // Bump kSaveVersion whenever the field list or flag ordering changes.
    static constexpr uint32_t kSaveMagic   = 0x44494F56u; // "VOID"
    static constexpr uint32_t kSaveVersion = 7u;
    uint32_t saveMagic   = kSaveMagic;
    uint32_t saveVersion = kSaveVersion;

    ErisNum::ErisNum credits        = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum data           = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum information    = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum entropy        = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum dataFragments  = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum fragmentEchoes = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum dataAxioms     = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum null           = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum heat           = ErisNum::ErisNum(0.0); // Offline-only currency (kelvin): accumulates while offline, spent to run the game at 5x speed (~11.9K per second). Cap 2,500 K, upgradable.
    ErisNum::ErisNum cache          = ErisNum::ErisNum(0.0); // Offline-only currency: accumulates from Information (at least 170:1); buys offline-only upgrades and certain Supports/Glitches

    ErisNum::ErisNum allCreditProduced      = ErisNum::ErisNum(0.0);

    // Data produced since the last Fragmentation. Drives the Data-rate log term, so
    // every run starts from base speed. allDataProduced below stays lifetime.
    ErisNum::ErisNum runDataProduced        = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum allDataProduced        = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allInformationProduced = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allEntropyProduced     = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum allDataFragments       = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allFragmentEchoes      = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allDataAxioms          = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allNull                = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum allHeat                = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum allCache               = ErisNum::ErisNum(0.0);

    ErisNum::ErisNum fragmentation  = ErisNum::ErisNum(0.0); // Data Fragments
    ErisNum::ErisNum singularity    = ErisNum::ErisNum(0.0); // Fragment Echoes
    ErisNum::ErisNum convergence    = ErisNum::ErisNum(0.0); // Data Axioms
    ErisNum::ErisNum nullification  = ErisNum::ErisNum(0.0); // Null

    double voidRate         = 0.0;
    int    finishedTutorial = 0;

    // Owned / unlocked flags, positional per list (see Flags.hpp). No 64-entry cap.
    std::vector<uint64_t> supportFlags;
    std::vector<uint64_t> marketFlags;
    std::vector<uint64_t> achievementFlags;
    std::vector<uint64_t> fragmentTreeFlags;
    std::vector<uint64_t> researchFlags;      // done techs (Research.hpp)

    // Research in progress (index into researchTree, -1 = idle) and its progress.
    int32_t researchActive  = -1;
    double  researchElapsed = 0.0;
    // Seconds remaining per Glitch, indexed like `glitches` (Glitches.hpp).
    std::vector<double> glitchTimers;

    // Infrastructure (Infrastructure.hpp): size x size grid, 0 = empty else kind+1.
    int32_t infraSize = 3;
    std::vector<uint8_t> infraCells;
    bool    autoSupports = false;                  // Autoloader on/off
    std::vector<uint64_t> glitchAutoFlags;         // Glitch Scheduler: which Glitches to keep running
    bool    infraUnlocked = false;
    double  autoTimer = 0.0;                       // not saved

    // Merchant (Merchant.hpp)
    bool    merchantPresent = false;
    double  merchantTimer   = 0.0;                 // until arrival, or until he leaves
    std::vector<uint8_t>  merchantOffers;          // current stock (offer ids)
    std::vector<uint8_t>  merchantLevels;          // per upgrade
    std::vector<uint64_t> cosmeticFlags;           // owned cosmetics
    int32_t cosmeticEquipped = -1;

    // Quality Assurance (Quality.hpp)
    bool    qaUnlocked  = false;
    int32_t qaLevelData = 0, qaLevelInfo = 0;
    int32_t qaFloorData = 0, qaFloorInfo = 0;      // % — below this, production becomes Entropy
    double  qaQualData  = 50.0, qaQualInfo = 50.0; // live quality %

    // Tab unlock latches — once true they stay true (see GameMechanics::updateUnlocks).
    bool marketUnlocked      = false;
    bool fragmentUnlocked    = false;
    bool researchUnlocked    = false;
    bool glitchUnlocked      = false;

    bool pinCredits         = false;
    bool pinData            = false;
    bool pinInformation     = false;
    bool pinEntropy         = false;
    bool pinDataFragments   = false;
    bool pinFragmentEchoes  = false;
    bool pinDataAxioms      = false;
    bool pinNull            = false;

    bool pinHeat            = false;
    bool pinCache           = false;

    bool pinStockUnitsA     = false;
    bool pinStockUnitsB     = false;
    bool pinStockUnitsC     = false;
    bool pinVoidcoinUnits   = false;

    ErisNum::ErisNum stockUnitsA   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum stockUnitsB   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum stockUnitsC   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum voidcoinUnits = ErisNum::ErisNum(0.0);

    // Price/range scale of the stock markets. Re-taken from lifetime earnings only when a
    // run starts (Fragmentation), so held positions can't drift upward while you earn.
    double stockScale          = 1.0;

    double stockMarketValueA   = 0.5;
    double stockMarketValueB   = 0.5;
    double stockMarketValueC   = 0.5;
    double voidcoinMarketValue = 0.5;

    ErisNum::ErisNum stockPnLA   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum stockPnLB   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum stockPnLC   = ErisNum::ErisNum(0.0);
    ErisNum::ErisNum voidcoinPnL = ErisNum::ErisNum(0.0);
};

template <>
struct SimpleDat::detail::Serializer<ErisNum::ErisNum> {
    static void write(std::ostream& os, const ErisNum::ErisNum& v) {
        std::string s = v.serialize();
        Serializer<std::string>::write(os, s);
    }
    static ErisNum::ErisNum read(std::istream& is) {
        std::string s = Serializer<std::string>::read(is);
        return ErisNum::ErisNum::deserialize(s);
    }
};

SIMPLEDAT_SERIALIZER_BEGIN(GameState)
    SIMPLEDAT_FIELD_WRITE(saveMagic)
    SIMPLEDAT_FIELD_WRITE(saveVersion)

    SIMPLEDAT_FIELD_WRITE(credits)

    SIMPLEDAT_FIELD_WRITE(data)
    SIMPLEDAT_FIELD_WRITE(information)
    SIMPLEDAT_FIELD_WRITE(entropy)

    SIMPLEDAT_FIELD_WRITE(dataFragments)
    SIMPLEDAT_FIELD_WRITE(fragmentEchoes)
    SIMPLEDAT_FIELD_WRITE(dataAxioms)
    SIMPLEDAT_FIELD_WRITE(null)

    SIMPLEDAT_FIELD_WRITE(heat)
    SIMPLEDAT_FIELD_WRITE(cache)

    SIMPLEDAT_FIELD_WRITE(allCreditProduced)
    SIMPLEDAT_FIELD_WRITE(runDataProduced)

    SIMPLEDAT_FIELD_WRITE(allDataProduced)
    SIMPLEDAT_FIELD_WRITE(allInformationProduced)
    SIMPLEDAT_FIELD_WRITE(allEntropyProduced)

    SIMPLEDAT_FIELD_WRITE(allDataFragments)
    SIMPLEDAT_FIELD_WRITE(allFragmentEchoes)
    SIMPLEDAT_FIELD_WRITE(allDataAxioms)
    SIMPLEDAT_FIELD_WRITE(allNull)

    SIMPLEDAT_FIELD_WRITE(allHeat)
    SIMPLEDAT_FIELD_WRITE(allCache)

    SIMPLEDAT_FIELD_WRITE(fragmentation)
    SIMPLEDAT_FIELD_WRITE(singularity)
    SIMPLEDAT_FIELD_WRITE(convergence)
    SIMPLEDAT_FIELD_WRITE(nullification)

    SIMPLEDAT_FIELD_WRITE(voidRate)
    SIMPLEDAT_FIELD_WRITE(finishedTutorial)
    SIMPLEDAT_FIELD_WRITE(supportFlags)
    SIMPLEDAT_FIELD_WRITE(marketFlags)
    SIMPLEDAT_FIELD_WRITE(achievementFlags)
    SIMPLEDAT_FIELD_WRITE(fragmentTreeFlags)
    SIMPLEDAT_FIELD_WRITE(marketUnlocked)
    SIMPLEDAT_FIELD_WRITE(fragmentUnlocked)
    SIMPLEDAT_FIELD_WRITE(researchUnlocked)
    SIMPLEDAT_FIELD_WRITE(glitchUnlocked)
    SIMPLEDAT_FIELD_WRITE(researchFlags)
    SIMPLEDAT_FIELD_WRITE(researchActive)
    SIMPLEDAT_FIELD_WRITE(researchElapsed)
    SIMPLEDAT_FIELD_WRITE(glitchTimers)
    SIMPLEDAT_FIELD_WRITE(infraSize)
    SIMPLEDAT_FIELD_WRITE(infraCells)
    SIMPLEDAT_FIELD_WRITE(autoSupports)
    SIMPLEDAT_FIELD_WRITE(glitchAutoFlags)
    SIMPLEDAT_FIELD_WRITE(infraUnlocked)
    SIMPLEDAT_FIELD_WRITE(merchantPresent)
    SIMPLEDAT_FIELD_WRITE(merchantTimer)
    SIMPLEDAT_FIELD_WRITE(merchantOffers)
    SIMPLEDAT_FIELD_WRITE(merchantLevels)
    SIMPLEDAT_FIELD_WRITE(cosmeticFlags)
    SIMPLEDAT_FIELD_WRITE(cosmeticEquipped)
    SIMPLEDAT_FIELD_WRITE(qaUnlocked)
    SIMPLEDAT_FIELD_WRITE(qaLevelData)
    SIMPLEDAT_FIELD_WRITE(qaLevelInfo)
    SIMPLEDAT_FIELD_WRITE(qaFloorData)
    SIMPLEDAT_FIELD_WRITE(qaFloorInfo)
    SIMPLEDAT_FIELD_WRITE(qaQualData)
    SIMPLEDAT_FIELD_WRITE(qaQualInfo)

    SIMPLEDAT_FIELD_WRITE(pinCredits)
    SIMPLEDAT_FIELD_WRITE(pinData)
    SIMPLEDAT_FIELD_WRITE(pinInformation)
    SIMPLEDAT_FIELD_WRITE(pinEntropy)
    SIMPLEDAT_FIELD_WRITE(pinDataFragments)
    SIMPLEDAT_FIELD_WRITE(pinFragmentEchoes)
    SIMPLEDAT_FIELD_WRITE(pinDataAxioms)
    SIMPLEDAT_FIELD_WRITE(pinNull)

    SIMPLEDAT_FIELD_WRITE(pinHeat)
    SIMPLEDAT_FIELD_WRITE(pinCache)

    SIMPLEDAT_FIELD_WRITE(pinStockUnitsA)
    SIMPLEDAT_FIELD_WRITE(pinStockUnitsB)
    SIMPLEDAT_FIELD_WRITE(pinStockUnitsC)
    SIMPLEDAT_FIELD_WRITE(pinVoidcoinUnits)

    SIMPLEDAT_FIELD_WRITE(stockUnitsA)
    SIMPLEDAT_FIELD_WRITE(stockUnitsB)
    SIMPLEDAT_FIELD_WRITE(stockUnitsC)
    SIMPLEDAT_FIELD_WRITE(voidcoinUnits)

    SIMPLEDAT_FIELD_WRITE(stockScale)
    SIMPLEDAT_FIELD_WRITE(stockMarketValueA)
    SIMPLEDAT_FIELD_WRITE(stockMarketValueB)
    SIMPLEDAT_FIELD_WRITE(stockMarketValueC)
    SIMPLEDAT_FIELD_WRITE(voidcoinMarketValue)

    SIMPLEDAT_FIELD_WRITE(stockPnLA)
    SIMPLEDAT_FIELD_WRITE(stockPnLB)
    SIMPLEDAT_FIELD_WRITE(stockPnLC)
    SIMPLEDAT_FIELD_WRITE(voidcoinPnL)

SIMPLEDAT_SERIALIZER_END(GameState)
    SIMPLEDAT_FIELD_READ(saveMagic)
    SIMPLEDAT_FIELD_READ(saveVersion)

    SIMPLEDAT_FIELD_READ(credits)

    SIMPLEDAT_FIELD_READ(data)
    SIMPLEDAT_FIELD_READ(information)
    SIMPLEDAT_FIELD_READ(entropy)

    SIMPLEDAT_FIELD_READ(dataFragments)
    SIMPLEDAT_FIELD_READ(fragmentEchoes)
    SIMPLEDAT_FIELD_READ(dataAxioms)
    SIMPLEDAT_FIELD_READ(null)

    SIMPLEDAT_FIELD_READ(heat)
    SIMPLEDAT_FIELD_READ(cache)

    SIMPLEDAT_FIELD_READ(allCreditProduced)
    SIMPLEDAT_FIELD_READ(runDataProduced)

    SIMPLEDAT_FIELD_READ(allDataProduced)
    SIMPLEDAT_FIELD_READ(allInformationProduced)
    SIMPLEDAT_FIELD_READ(allEntropyProduced)

    SIMPLEDAT_FIELD_READ(allDataFragments)
    SIMPLEDAT_FIELD_READ(allFragmentEchoes)
    SIMPLEDAT_FIELD_READ(allDataAxioms)
    SIMPLEDAT_FIELD_READ(allNull)

    SIMPLEDAT_FIELD_READ(allHeat)
    SIMPLEDAT_FIELD_READ(allCache)

    SIMPLEDAT_FIELD_READ(fragmentation)
    SIMPLEDAT_FIELD_READ(singularity)
    SIMPLEDAT_FIELD_READ(convergence)
    SIMPLEDAT_FIELD_READ(nullification)

    SIMPLEDAT_FIELD_READ(voidRate)
    SIMPLEDAT_FIELD_READ(finishedTutorial)
    SIMPLEDAT_FIELD_READ(supportFlags)
    SIMPLEDAT_FIELD_READ(marketFlags)
    SIMPLEDAT_FIELD_READ(achievementFlags)
    SIMPLEDAT_FIELD_READ(fragmentTreeFlags)
    SIMPLEDAT_FIELD_READ(marketUnlocked)
    SIMPLEDAT_FIELD_READ(fragmentUnlocked)
    SIMPLEDAT_FIELD_READ(researchUnlocked)
    SIMPLEDAT_FIELD_READ(glitchUnlocked)
    SIMPLEDAT_FIELD_READ(researchFlags)
    SIMPLEDAT_FIELD_READ(researchActive)
    SIMPLEDAT_FIELD_READ(researchElapsed)
    SIMPLEDAT_FIELD_READ(glitchTimers)
    SIMPLEDAT_FIELD_READ(infraSize)
    SIMPLEDAT_FIELD_READ(infraCells)
    SIMPLEDAT_FIELD_READ(autoSupports)
    SIMPLEDAT_FIELD_READ(glitchAutoFlags)
    SIMPLEDAT_FIELD_READ(infraUnlocked)
    SIMPLEDAT_FIELD_READ(merchantPresent)
    SIMPLEDAT_FIELD_READ(merchantTimer)
    SIMPLEDAT_FIELD_READ(merchantOffers)
    SIMPLEDAT_FIELD_READ(merchantLevels)
    SIMPLEDAT_FIELD_READ(cosmeticFlags)
    SIMPLEDAT_FIELD_READ(cosmeticEquipped)
    SIMPLEDAT_FIELD_READ(qaUnlocked)
    SIMPLEDAT_FIELD_READ(qaLevelData)
    SIMPLEDAT_FIELD_READ(qaLevelInfo)
    SIMPLEDAT_FIELD_READ(qaFloorData)
    SIMPLEDAT_FIELD_READ(qaFloorInfo)
    SIMPLEDAT_FIELD_READ(qaQualData)
    SIMPLEDAT_FIELD_READ(qaQualInfo)

    SIMPLEDAT_FIELD_READ(pinCredits)
    SIMPLEDAT_FIELD_READ(pinData)
    SIMPLEDAT_FIELD_READ(pinInformation)
    SIMPLEDAT_FIELD_READ(pinEntropy)
    SIMPLEDAT_FIELD_READ(pinDataFragments)
    SIMPLEDAT_FIELD_READ(pinFragmentEchoes)
    SIMPLEDAT_FIELD_READ(pinDataAxioms)
    SIMPLEDAT_FIELD_READ(pinNull)

    SIMPLEDAT_FIELD_READ(pinHeat)
    SIMPLEDAT_FIELD_READ(pinCache)

    SIMPLEDAT_FIELD_READ(pinStockUnitsA)
    SIMPLEDAT_FIELD_READ(pinStockUnitsB)
    SIMPLEDAT_FIELD_READ(pinStockUnitsC)
    SIMPLEDAT_FIELD_READ(pinVoidcoinUnits)

    SIMPLEDAT_FIELD_READ(stockUnitsA)
    SIMPLEDAT_FIELD_READ(stockUnitsB)
    SIMPLEDAT_FIELD_READ(stockUnitsC)
    SIMPLEDAT_FIELD_READ(voidcoinUnits)

    SIMPLEDAT_FIELD_READ(stockScale)
    SIMPLEDAT_FIELD_READ(stockMarketValueA)
    SIMPLEDAT_FIELD_READ(stockMarketValueB)
    SIMPLEDAT_FIELD_READ(stockMarketValueC)
    SIMPLEDAT_FIELD_READ(voidcoinMarketValue)

    SIMPLEDAT_FIELD_READ(stockPnLA)
    SIMPLEDAT_FIELD_READ(stockPnLB)
    SIMPLEDAT_FIELD_READ(stockPnLC)
    SIMPLEDAT_FIELD_READ(voidcoinPnL)
SIMPLEDAT_READ_END()