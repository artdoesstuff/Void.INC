#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include "GameState.hpp"
#include "Economy.hpp"
#include "Supports.hpp"
#include "Marketplace.hpp"
#include "FragmentTree.hpp"
#include "Research.hpp"
#include "Glitches.hpp"
#include "Infrastructure.hpp"
#include "CreditMarket.hpp"

namespace GameMechanics {
    static constexpr double BASE_DATA_RATE     = 1.0;
    static constexpr float  AUTO_SAVE_INTERVAL = 30.f;
    static constexpr double VOID_RATE_HARD_MAX = 0.95; // absolute ceiling, whatever the tree says

    // Current void-rate ceiling: base 50% plus Fragment-tree bonuses.
    [[nodiscard]] inline double voidRateMax() {
        return std::min(Economy::VOID_RATE_BASE_MAX + treeVoidMaxBonus(), VOID_RATE_HARD_MAX);
    }

    // (1 + lifetime Fragments) ^ FRAG_BOOST_EXP. Uses the LIFETIME total, so
    // spending Fragments in the tree never weakens it.
    [[nodiscard]] inline ErisNum::ErisNum fragmentBoost(const GameState& gs) {
        return (ErisNum::ErisNum(1.0) + gs.allDataFragments).pow(Economy::FRAG_BOOST_EXP);
    }

    // Support-line multiplier with the Research "stronger Supports" bonus. The
    // bonus only applies to a line that has at least one Support installed, so
    // it can never make an empty line worth more than x1.
    [[nodiscard]] inline ErisNum::ErisNum supportBoost(RESOURCE::BOOST line) {
        ErisNum::ErisNum m = getBoostMultiplier(line);
        for (const SupportData& u : supports)
            if (u.installed && u.boost == line)
                return m * ErisNum::ErisNum(researchSupportMult());
        return m;
    }

    // Data per second. The log term is driven by the CURRENT RUN's production
    // (runDataProduced), so Fragmentation resets the climb; the Fragment boost
    // and tree nodes are what carry over.
    [[nodiscard]] inline ErisNum::ErisNum dataRate(const GameState& gs) {
        ErisNum::ErisNum logVal = ErisNum::log10(gs.runDataProduced + ErisNum::ErisNum(1.0));
        ErisNum::ErisNum base   = ErisNum::ErisNum(BASE_DATA_RATE) * (ErisNum::ErisNum(1.0) + logVal * ErisNum::ErisNum(0.5));
        return base * supportBoost(RESOURCE::BOOST::DATA)
                    * ErisNum::ErisNum(glitchMult(gs, GlitchEffect::DATA_PROD))
                    * ErisNum::ErisNum(infraMult(gs, InfraEffect::DATA_PROD))
                    * ErisNum::ErisNum(merchantMult(gs, MerchantEffect::DATA_PROD))
                    * ErisNum::ErisNum(static_cast<double>(dataProdPerkMult()))
                    * ErisNum::ErisNum(treeDataMult())
                    * fragmentBoost(gs);
    }

    // Automation (Autoloader / Glitch Scheduler). Runs every AUTO_PERIOD seconds.
    inline void runAutomation(GameState& gs) {
        if (gs.autoSupports && infraHas(gs, InfraEffect::AUTO_SUPPORTS)) {
            for (size_t i = 0; i < supports.size(); ++i)          // cheapest-first order of the list
                if (!supports[i].installed && canAffordSupport(supports[i], gs)) {
                    installSupport(supports, gs, i);
                    break;                                          // one per pass
                }
        }
        if (infraHas(gs, InfraEffect::AUTO_GLITCHES)) {
            ensureGlitchTimers(gs);
            const double dm = researchGlitchDurMult();
            for (size_t i = 0; i < glitches.size(); ++i) {
                if (!Flags::get(gs.glitchAutoFlags, i)) continue;
                // Re-buy once less than one full duration remains: never wastes time at the cap.
                if (glitchRemaining(gs, i) < glitches[i].seconds * dm * 0.5 && canBuyGlitch(gs, i, dm))
                    buyGlitch(gs, i, dm);
            }
        }
    }

    inline void tick(GameState& gs, double dt) {
        if (dt <= 0.0) return;

        tickGlitches(gs, dt);
        tickResearch(gs, dt);
        tickMerchant(gs, dt);
        gs.autoTimer += dt;
        if (gs.autoTimer >= InfraRules::AUTO_PERIOD) { gs.autoTimer = 0.0; runAutomation(gs); }

        ErisNum::ErisNum produced = dataRate(gs) * ErisNum::ErisNum(dt);
        gs.data              += produced;
        gs.allDataProduced   += produced;
        gs.runDataProduced   += produced;

        // Information is generated ALONGSIDE Data at a fixed 2:1 split (Data
        // earns twice as much as Information, base). Data is not drained by
        // this — it is a deliberate split of the same production, not a
        // conversion cost. INFORMATION supports/perks/tree boost only the
        // Information side.
        ErisNum::ErisNum converted = produced * ErisNum::ErisNum(0.5);
        ErisNum::ErisNum grossInfo = converted * supportBoost(RESOURCE::BOOST::INFORMATION)
                                                * ErisNum::ErisNum(glitchMult(gs, GlitchEffect::INFO_PROD))
                                                * ErisNum::ErisNum(infraMult(gs, InfraEffect::INFO_PROD))
                                                * ErisNum::ErisNum(merchantMult(gs, MerchantEffect::INFO_PROD))
                                                * ErisNum::ErisNum(static_cast<double>(infoProdPerkMult()))
                                                * ErisNum::ErisNum(treeInfoMult());

        // The void intercepts a voidRate fraction of THIS TICK'S incoming
        // Information before it's banked — it no longer drains the existing
        // gs.information balance. grossInfo already has dt baked in (via
        // `produced`), so no extra dt factor is applied here. Part of the
        // intercepted slice is recovered as Entropy; the rest is just lost.
        ErisNum::ErisNum intercepted = grossInfo * ErisNum::ErisNum(gs.voidRate);
        ErisNum::ErisNum infoGain    = grossInfo - intercepted;

        gs.information              += infoGain;
        gs.allInformationProduced   += infoGain;

        if (gs.voidRate > 0.0) {
            // ENTROPY supports/perks/tree boost only the amount of Entropy
            // recovered from the intercepted slice.
            ErisNum::ErisNum entropyGain = intercepted * ErisNum::ErisNum(0.2) * supportBoost(RESOURCE::BOOST::ENTROPY)
                                                        * ErisNum::ErisNum(infraMult(gs, InfraEffect::ENTROPY_PROD))
                                                        * ErisNum::ErisNum(merchantMult(gs, MerchantEffect::ENTROPY_PROD))
                                                        * ErisNum::ErisNum(static_cast<double>(entropyProdPerkMult()))
                                                        * ErisNum::ErisNum(treeEntropyMult());
            gs.entropy              += entropyGain;
            gs.allEntropyProduced   += entropyGain;
        }
    }

    inline void setVoidPercent(GameState& gs, double v) {
        gs.voidRate = std::clamp(v, 0.0, voidRateMax());
    }

    // What gets consumed: Data, Information and Entropy HELD at the moment of
    // Fragmentation, weighted by scarcity. Credits are wiped but add nothing.

    [[nodiscard]] inline ErisNum::ErisNum fragmentValue(const GameState& gs) {
        return gs.data
             + gs.information * ErisNum::ErisNum(Economy::FRAG_WEIGHT_INFO)
             + gs.entropy     * ErisNum::ErisNum(Economy::FRAG_WEIGHT_ENTROPY);
    }

    [[nodiscard]] inline ErisNum::ErisNum fragmentGain(const GameState& gs) {
        const ErisNum::ErisNum v = fragmentValue(gs);
        if (v <= ErisNum::ErisNum(0.0)) return ErisNum::ErisNum(0.0);
        const ErisNum::ErisNum ratio = v / ErisNum::ErisNum(Economy::FRAG_THRESHOLD);
        return (ratio.pow(Economy::FRAG_GAIN_EXP) * ErisNum::ErisNum(treeFragmentGainMult())).floor();
    }

    // Fragment value needed for the NEXT whole Fragment.
    [[nodiscard]] inline ErisNum::ErisNum fragmentNextTarget(const GameState& gs) {
        const ErisNum::ErisNum next = (fragmentGain(gs) + ErisNum::ErisNum(1.0))
                                    / ErisNum::ErisNum(treeFragmentGainMult());
        return ErisNum::ErisNum(Economy::FRAG_THRESHOLD) * next.pow(1.0 / Economy::FRAG_GAIN_EXP);
    }

    [[nodiscard]] inline bool canFragment(const GameState& gs) {
        return fragmentGain(gs) >= ErisNum::ErisNum(1.0);
    }

    // Reset the run. Returns false (and changes nothing) if the gain is < 1.
    //   Wiped : Data, Information, Entropy, Credits, per-run production,
    //           Supports (except the first N the tree keeps), Marketplace
    //           perks, stock/Voidcoin holdings (they are converted Credits).
    //   Kept  : Fragments, the tree, lifetime counters, achievements, void
    //           rate, pins, unlocks, Heat/Cache, market prices, trade P&L.
    inline bool fragment(GameState& gs) {
        const ErisNum::ErisNum gain = fragmentGain(gs);
        if (gain < ErisNum::ErisNum(1.0)) return false;

        gs.dataFragments    += gain;
        gs.allDataFragments += gain;
        gs.fragmentation    += ErisNum::ErisNum(1.0);

        const ErisNum::ErisNum zero(0.0);
        gs.data = gs.information = gs.entropy = gs.credits = zero;
        gs.runDataProduced = zero;

        clearGlitches(gs);   // Glitches are per-run; research is permanent

        gs.stockUnitsA = gs.stockUnitsB = gs.stockUnitsC = gs.voidcoinUnits = zero;
        gs.stockScale  = stockScaleFor(gs.allCreditProduced);   // new run, new price level

        const size_t keep = treeKeepSupports();
        for (size_t i = keep; i < supports.size(); ++i) supports[i].installed = 0;
        packSupport(supports, gs.supportFlags);

        for (MarketPerk& p : marketPerks) p.purchased = 0;
        packMarket(marketPerks, gs.marketFlags);

        return true;
    }

    // Clamp / repair anything a corrupt or hand-edited save could poison.
    inline void sanitize(GameState& gs) {
        if (!std::isfinite(gs.voidRate)) gs.voidRate = 0.0;
        gs.voidRate = std::clamp(gs.voidRate, 0.0, VOID_RATE_HARD_MAX);
        if (!std::isfinite(gs.stockScale) || gs.stockScale < 1.0) gs.stockScale = 1.0;
        ensureInfraGrid(gs);
        ensureMerchantState(gs);
        ensureGlitchTimers(gs);
        for (double& t : gs.glitchTimers)
            if (!std::isfinite(t) || t < 0.0) t = 0.0;
        if (gs.researchActive < -1 || gs.researchActive >= static_cast<int32_t>(researchTree.size()))
            gs.researchActive = -1;
        if (!std::isfinite(gs.researchElapsed) || gs.researchElapsed < 0.0 || gs.researchActive < 0)
            gs.researchElapsed = 0.0;
    }

    // Latch tab unlocks. Cheap — call every tick. A tab unlocks the moment the
    // player can actually earn something there, and then stays unlocked:
    //   Market   : Data or Information could be sold for >= 1 Credit
    //   Fragment : a Fragmentation would yield >= 1 Fragment
    inline void updateUnlocks(GameState& gs) {
        ensureInfraGrid(gs);
        const ErisNum::ErisNum zero(0.0);
        if (!gs.marketUnlocked &&
            (tradeValueReachesOneCredit(gs) || gs.allCreditProduced > zero))
            gs.marketUnlocked = true;

        if (!gs.fragmentUnlocked &&
            (canFragment(gs) || gs.allDataFragments > zero || gs.fragmentation > zero))
            gs.fragmentUnlocked = true;

        if (!gs.infraUnlocked && infraWorthShowing(gs)) gs.infraUnlocked = true;

        // Research: something to research is affordable (or already underway/done).
        if (!gs.researchUnlocked && researchWorthShowing(gs)) gs.researchUnlocked = true;
        // Glitches: enough Entropy has ever been produced to buy the cheapest one.
        if (!gs.glitchUnlocked &&
            (gs.allEntropyProduced >= ErisNum::ErisNum(cheapestGlitchCost()) || anyGlitchActive(gs)))
            gs.glitchUnlocked = true;
    }

    // Atomic (SimpleDat writes to .tmp then renames). Never throws.
    inline bool save(const GameState& gs, const std::string& path = "save.dat") {
        try {
            SimpleDat::save(path, gs);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[save] failed: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "[save] failed: unknown error\n";
        }
        return false;
    }

    // Throws if the file is missing, corrupt, or from another save version.
    [[nodiscard]] inline GameState load(const std::string& path = "save.dat") {
        GameState gs = SimpleDat::load<GameState>(path);
        if (gs.saveMagic != GameState::kSaveMagic || gs.saveVersion != GameState::kSaveVersion)
            throw std::runtime_error("save header mismatch (magic/version)");
        sanitize(gs);
        return gs;
    }

    // Never destroys a save it could not read: an unreadable file is renamed
    // to "<path>.broken-<unixtime>" before a fresh game starts, so the next
    // autosave can't overwrite it. After a successful load, "<path>.bak"
    // is refreshed with that known-good file.
    [[nodiscard]] inline GameState loadOrNew(const std::string& path = "save.dat") {
        namespace fs = std::filesystem;
        std::error_code ec;
        if (!fs::exists(path, ec)) return GameState{};

        std::string why;
        try {
            GameState gs = load(path);
            fs::copy_file(path, path + ".bak", fs::copy_options::overwrite_existing, ec);
            return gs;
        } catch (const std::exception& e) {
            why = e.what();
        } catch (...) {
            why = "unknown error";
        }

        // Unique name: never overwrite an earlier quarantined file.
        const std::string stamp = path + ".broken-" + std::to_string(static_cast<long long>(std::time(nullptr)));
        std::string quarantined = stamp;
        for (int n = 1; fs::exists(quarantined, ec); ++n)
            quarantined = stamp + "-" + std::to_string(n);
        fs::rename(path, quarantined, ec);
        std::cerr << "[load] could not read " << path << " (" << why << "); "
                  << (ec ? "could not move it aside" : "kept as " + quarantined)
                  << " — starting a new game.\n";
        return GameState{};
    }

    [[nodiscard]] inline float dataLog10Float(const GameState& gs) {
        if (gs.allDataProduced <= ErisNum::ErisNum(1.0)) return 0.f;
        return static_cast<float>(ErisNum::log10(gs.allDataProduced).toDouble());
    }
}
