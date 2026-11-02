#pragma once

/**
 * @file CreditMarket.hpp
 * @brief Void.INC — Credit Market simulation.
 *
 * Two volatile exchanges:
 *   DATA        market value [0, +200%] → multiplier [0.0, 3.0]
 *   INFORMATION market value [0, +500%] → multiplier [0.0, 6.0]
 *
 * The market graph ticks every TICK_INTERVAL seconds so the player
 * has time to react between price movements.
 *
 * Selling is manual (SELL button in the Market tab). One click sells
 * `sellFrac` of the current balance:
 *   data credits        = data        * dataSellFrac * dataMarketMult * Economy::SELL_DATA_BASE * perks
 *   information credits = information * infoSellFrac * infoMarketMult * Economy::SELL_INFO_BASE * perks
 *
 * Both sell fractions are player-controlled in 5% increments, capped at 90%.
 *
 * Usage:
 *   CreditMarket market;
 *   // each frame:
 *   market.update(gs, dt);
 */

#include <array>
#include <cmath>
#include <random>
#include "dependancies/ErisNum.hpp"
#include "Marketplace.hpp"
#include "Economy.hpp"
#include "FragmentTree.hpp"
#include "Research.hpp"

// Forward declaration — GameState included by whoever calls market.update()
struct GameState;

inline constexpr float  MARKET_TICK_INTERVAL = 1.f;   // seconds between price moves
inline constexpr size_t MARKET_HISTORY_LEN   = 60;    // points kept for the graph

// Same OU random-walk model as DATA/INFO below, just looped since there
// are 4 of them. Nothing here is pinned to a late-game magnitude:
//
//   price per unit = kStockBasePrice[i] * stockScale * multiplier
//   multiplier     = 1 + current * maxBonus                 (current in [0,1])
//   maxBonus       = kStockBaseRange[i] * (1 + 0.5 * log10(stockScale))
//
// stockScale follows lifetime earnings (stockScaleFor) and is re-taken only
// when a run starts — Fragmentation wipes holdings — so a held position's
// price can't drift upward just because the player keeps earning.
inline constexpr size_t NUM_STOCKS        = 3;
inline constexpr size_t NUM_STOCK_MARKETS = NUM_STOCKS + 1;   // + Voidcoin-like
inline constexpr size_t VOIDCOIN_INDEX     = NUM_STOCKS;       // last slot

// Multiplier headroom at stockScale = 1: A swings within 1x..4x, Voidcoin 1x..31x.
inline constexpr std::array<float, NUM_STOCK_MARKETS> kStockBaseRange = { 3.f, 6.f, 12.f, 30.f };

// Per-tick (1 s) volatility, measured rather than guessed: over a 60 s window
// the price moves about 11% / 15% / 21% / 28% (1 sigma) for A / B / C / Voidcoin,
// with rare crashes of roughly -40% / -50% / -65% / -85%.
struct StockVol { float theta, sigma, crashMag, spikeMag, crashProb, spikeProb; };
inline constexpr std::array<StockVol, NUM_STOCK_MARKETS> kStockVol = {{
    { 0.08f, 0.016f, 0.040f, 0.030f, 0.004f, 0.004f },   // STOCK A
    { 0.08f, 0.017f, 0.045f, 0.032f, 0.004f, 0.004f },   // STOCK B
    { 0.07f, 0.019f, 0.050f, 0.035f, 0.004f, 0.004f },   // STOCK C
    { 0.06f, 0.021f, 0.060f, 0.045f, 0.005f, 0.005f },   // VOIDCOIN (trends run longer, crashes hurt)
}};

// Price scale for a run, from lifetime Credits earned: sqrt growth, never below 1.
[[nodiscard]] inline double stockScaleFor(const ErisNum::ErisNum& lifetimeCredits) {
    const double s = (lifetimeCredits / ErisNum::ErisNum(Economy::STOCK_SCALE_DIVISOR)).sqrt().toDouble();
    if (!std::isfinite(s)) return 1e150;
    return s > 1.0 ? s : 1.0;
}

inline constexpr std::array<const char*, NUM_STOCK_MARKETS> kStockNames = {
    "STOCK A", "STOCK B", "STOCK C", "VOIDCOIN"
};

// Credit price per unit at multiplier = 1x (i.e. market value = 0).
// Flavor-only placeholder values — tune freely.
inline constexpr std::array<double, NUM_STOCK_MARKETS> kStockBasePrice = {
    100.0, 2500.0, 9500.0, 20000.0
};

// Raw [0,1] market value; display percentage = value * maxPct
struct MarketSeries {
    std::array<float, MARKET_HISTORY_LEN> history {};  // 0 = oldest, back = newest
    float current    = 0.5f;   // [0, 1]
    float velocity   = 0.f;    // Ornstein-Uhlenbeck drift

    // Display range: multiplier = 1 + current * maxBonus
    // DATA: maxBonus = 2.0  → [1x, 3x]
    // INFO: maxBonus = 5.0  → [1x, 6x]
    float maxBonus = 2.f;

    [[nodiscard]] float multiplier() const {
        return 1.f + current * maxBonus;
    }

    [[nodiscard]] float displayPct() const {
        return current * maxBonus * 100.f;   // 0 → +200% / +500%
    }

    void init(float bonus, float startVal) {
        maxBonus = bonus;
        current  = startVal;
        history.fill(startVal);
    }

    // Ornstein-Uhlenbeck mean-reverting random walk with crash spikes.
    // theta = reversion speed, sigma = base volatility, crashMag/spikeMag =
    // the bias kick applied on a crash/spike tick, crashProb/spikeProb =
    // their per-tick odds. Defaults match DATA/INFO's original tuning.
    void tick(std::mt19937& rng,
              float theta     = 0.22f,
              float sigma     = 0.28f,
              float crashMag  = 0.35f,
              float spikeMag  = 0.25f,
              float crashProb = 0.10f,
              float spikeProb = 0.08f) {
        std::normal_distribution<float> nd(0.f, 1.f);
        std::uniform_real_distribution<float> ud(0.f, 1.f);

        float noise = nd(rng);

        // Per-tick chance of a crash (drop toward 0) or spike (jump toward 1)
        float crashBias = 0.f;
        if (ud(rng) < crashProb)       crashBias = -crashMag;  // crash
        else if (ud(rng) < spikeProb)  crashBias =  spikeMag;  // spike

        velocity = velocity * 0.6f
                 + theta * (0.5f - current)
                 + sigma * noise
                 + crashBias;
        current  = std::clamp(current + velocity, 0.f, 1.f);

        // Shift history left, push new value
        for (size_t i = 0; i < MARKET_HISTORY_LEN - 1; ++i)
            history[i] = history[i + 1];
        history[MARKET_HISTORY_LEN - 1] = current;
    }
};

struct CreditMarket {
    MarketSeries dataSeries;
    MarketSeries infoSeries;

    // 3 vanilla stocks + 1 Voidcoin-like market, driven as a loop.
    std::array<MarketSeries, NUM_STOCK_MARKETS> stockSeries;

    // Units to buy/sell per click of the BUY/SELL button, per market.
    std::array<int, NUM_STOCK_MARKETS> stockTradeQty { 1, 1, 1, 1 };

    // Sell allocation — fraction of current balance sold per SELL click.
    // Both capped at kSellMax (90%), stepped in 0.05 increments.
    float dataSellFrac = 0.f;   // [0, kSellMax]
    float infoSellFrac = 0.f;   // [0, kSellMax]

    static constexpr float kSellStep     = 0.05f;
    static constexpr float kSellMax      = 0.90f;   // base cap; Research can raise it (sellMax())
    static constexpr float kSellMaxHard  = 0.99f;   // absolute cap
    [[nodiscard]] static float sellMax() {
        return std::min(kSellMaxHard, kSellMax + static_cast<float>(researchSellFracBonus()));
    }
    static constexpr int   kTradeQtyStep = 1;
    static constexpr int   kTradeQtyMax  = 1000;

    // Internal
    float  tickTimer_  = 0.f;
    double stockScale_ = 1.0;
    std::mt19937 rng_ { std::random_device{}() };

    CreditMarket() {
        // DATA range:  0 → +200%   (bonus = 2)
        // INFO range:  0 → +500%   (bonus = 5)
        dataSeries.init(2.f, 0.5f);
        infoSeries.init(5.f, 0.3f);

        for (size_t i = 0; i < NUM_STOCK_MARKETS; ++i)
            stockSeries[i].init(kStockBaseRange[i], 0.5f);
    }

    // Re-derive every stock's multiplier headroom from the price scale.
    void setStockScale(double scale) {
        stockScale_ = (std::isfinite(scale) && scale >= 1.0) ? scale : 1.0;
        const float widen = (1.f + 0.5f * static_cast<float>(std::log10(stockScale_)))
                          * static_cast<float>(researchStockRangeMult());
        for (size_t i = 0; i < NUM_STOCK_MARKETS; ++i)
            stockSeries[i].maxBonus = kStockBaseRange[i] * widen;
    }
    [[nodiscard]] double stockScale() const { return stockScale_; }

    // Call once, right after construction, with the just-loaded GameState.
    // Without this the stock/Voidcoin markets would reset to their 0.5
    // starting value every time the game is relaunched — letting players
    // "reroll" a bad market by quitting and reopening. Loading the last
    // known value (and re-filling the graph history with it) closes that.
    template <typename GS>
    void restoreFromSave(const GS& gs) {
        auto restoreOne = [](MarketSeries& s, double saved) {
            s.current = std::clamp(static_cast<float>(saved), 0.f, 1.f);
            s.history.fill(s.current);
            s.velocity = 0.f;
        };
        restoreOne(stockSeries[0],             gs.stockMarketValueA);
        restoreOne(stockSeries[1],             gs.stockMarketValueB);
        restoreOne(stockSeries[2],             gs.stockMarketValueC);
        restoreOne(stockSeries[VOIDCOIN_INDEX], gs.voidcoinMarketValue);
        setStockScale(gs.stockScale);
    }

    // ---- update — call every frame from Hud::update
    template <typename GS>
    void update(GS& gs, float dt) {
        // A new run (Fragmentation) re-prices the stocks.
        if (gs.stockScale != stockScale_) setStockScale(gs.stockScale);

        tickTimer_ += dt;
        while (tickTimer_ >= MARKET_TICK_INTERVAL) {
            tickTimer_ -= MARKET_TICK_INTERVAL;
            // Recompute vol caps from perks before each tick
            float volMult = static_cast<float>(marketVolCapPerkMult());
            const float ceil = static_cast<float>(researchCeilMult());
            dataSeries.maxBonus = 2.f * volMult * ceil;
            infoSeries.maxBonus = 5.f * volMult * ceil;

            // Glitches change how WILDLY a market moves, never where it sits.
            const float tv = static_cast<float>(glitchMult(gs, GlitchEffect::TRADE_VOL));
            const float sv = static_cast<float>(glitchMult(gs, GlitchEffect::STOCK_VOL));
            dataSeries.tick(rng_, 0.22f, 0.28f * tv, 0.35f * tv, 0.25f * tv);
            infoSeries.tick(rng_, 0.22f, 0.28f * tv, 0.35f * tv, 0.25f * tv);

            // Stocks / Voidcoin — same model, looped, each with its own measured
            // volatility (kStockVol). Perks that raise the DATA/INFO ceiling do
            // not touch these, so buying one can't make held shares jump.
            for (size_t i = 0; i < NUM_STOCK_MARKETS; ++i) {
                const StockVol& v = kStockVol[i];
                stockSeries[i].tick(rng_, v.theta, v.sigma * sv, v.crashMag * sv, v.spikeMag * sv, v.crashProb, v.spikeProb);
            }
        }

        // Keep GameState's persisted market values current every frame, so
        // whichever save fires next (F5 / autosave / Escape / window close)
        // always captures the latest value rather than a stale one.
        gs.stockMarketValueA  = static_cast<double>(stockSeries[0].current);
        gs.stockMarketValueB  = static_cast<double>(stockSeries[1].current);
        gs.stockMarketValueC  = static_cast<double>(stockSeries[2].current);
        gs.voidcoinMarketValue = static_cast<double>(stockSeries[VOIDCOIN_INDEX].current);
    }

    // Same formula sellData() uses; read-only, no state mutation.
    template <typename GS>
    [[nodiscard]] double previewSellData(const GS& gs) const {
        if (dataSellFrac <= 0.f || gs.data <= ErisNum::ErisNum(0.0)) return 0.0;
        double sold = gs.data.toDouble() * dataSellFrac;
        return sold
             * static_cast<double>(dataSeries.multiplier())
             * Economy::SELL_DATA_BASE
             * static_cast<double>(dataSellValuePerkMult())
             * treeSellMult() * researchSellMult() * infraMult(gs, InfraEffect::SELL_VALUE);
    }

    template <typename GS>
    [[nodiscard]] double previewSellInfo(const GS& gs) const {
        if (infoSellFrac <= 0.f || gs.information <= ErisNum::ErisNum(0.0)) return 0.0;
        double sold = gs.information.toDouble() * infoSellFrac;
        return sold
             * static_cast<double>(infoSeries.multiplier())
             * Economy::SELL_INFO_BASE
             * static_cast<double>(infoSellValuePerkMult())
             * treeSellMult() * researchSellMult() * infraMult(gs, InfraEffect::SELL_VALUE);
    }

    template <typename GS>
    void sellData(GS& gs) {
        double earned = previewSellData(gs);
        if (earned <= 0.0) return;
        double sold = gs.data.toDouble() * dataSellFrac;
        gs.data               -= ErisNum::ErisNum(sold);
        gs.credits            += ErisNum::ErisNum(earned);
        gs.allCreditProduced  += ErisNum::ErisNum(earned);
        if (gs.data < ErisNum::ErisNum(0.0)) gs.data = ErisNum::ErisNum(0.0);
    }

    template <typename GS>
    void sellInfo(GS& gs) {
        double earned = previewSellInfo(gs);
        if (earned <= 0.0) return;
        double sold = gs.information.toDouble() * infoSellFrac;
        gs.information        -= ErisNum::ErisNum(sold);
        gs.credits            += ErisNum::ErisNum(earned);
        gs.allCreditProduced  += ErisNum::ErisNum(earned);
        if (gs.information < ErisNum::ErisNum(0.0)) gs.information = ErisNum::ErisNum(0.0);
    }

    // Price per unit scales with the market's current multiplier — the
    // same value that scales credit yield for DATA/INFO.
    [[nodiscard]] double stockPrice(size_t idx) const {
        if (idx >= NUM_STOCK_MARKETS) return 0.0;
        return kStockBasePrice[idx] * stockScale_ * static_cast<double>(stockSeries[idx].multiplier());
    }

    template <typename GS>
    static ErisNum::ErisNum& stockUnitsFor(GS& gs, size_t idx) {
        switch (idx) {
            case 0:  return gs.stockUnitsA;
            case 1:  return gs.stockUnitsB;
            case 2:  return gs.stockUnitsC;
            default: return gs.voidcoinUnits;
        }
    }
    // Const overload — for read-only preview use, no mutable access needed.
    template <typename GS>
    static const ErisNum::ErisNum& stockUnitsFor(const GS& gs, size_t idx) {
        switch (idx) {
            case 0:  return gs.stockUnitsA;
            case 1:  return gs.stockUnitsB;
            case 2:  return gs.stockUnitsC;
            default: return gs.voidcoinUnits;
        }
    }

    // Read-only; buyStock()/sellStock() call these too, so the number shown
    // to the player is always exactly what they'll get.
    [[nodiscard]] double previewBuyStock(size_t idx, int qty = -1) const {
        if (idx >= NUM_STOCK_MARKETS) return 0.0;
        const int n = (qty > 0) ? qty : stockTradeQty[idx];
        if (n <= 0) return 0.0;
        return stockPrice(idx) * static_cast<double>(n);
    }

    template <typename GS>
    [[nodiscard]] double previewSellStock(const GS& gs, size_t idx, int qty = -1) const {
        if (idx >= NUM_STOCK_MARKETS) return 0.0;
        const ErisNum::ErisNum& units = stockUnitsFor(gs, idx);
        if (units <= ErisNum::ErisNum(0.0)) return 0.0;

        const int n = (qty > 0) ? qty : stockTradeQty[idx];
        if (n <= 0) return 0.0;

        ErisNum::ErisNum sellAmt(static_cast<double>(n));
        if (units < sellAmt) sellAmt = units;   // can't sell more than owned

        return stockPrice(idx) * sellAmt.toDouble();
    }

    // Credits received from selling minus credits spent buying, lifetime.
    template <typename GS>
    static ErisNum::ErisNum& pnlFor(GS& gs, size_t idx) {
        switch (idx) {
            case 0:  return gs.stockPnLA;
            case 1:  return gs.stockPnLB;
            case 2:  return gs.stockPnLC;
            default: return gs.voidcoinPnL;
        }
    }

    // Sum of all 4 markets' net P&L — "are you up or down overall".
    template <typename GS>
    [[nodiscard]] static ErisNum::ErisNum totalStockPnL(const GS& gs) {
        return gs.stockPnLA + gs.stockPnLB + gs.stockPnLC + gs.voidcoinPnL;
    }

    // qty <= 0 means "use this market's configured trade quantity".
    template <typename GS>
    void buyStock(GS& gs, size_t idx, int qty = -1) {
        if (idx >= NUM_STOCK_MARKETS) return;
        const int n = (qty > 0) ? qty : stockTradeQty[idx];
        if (n <= 0) return;

        const ErisNum::ErisNum cost(previewBuyStock(idx, n));
        if (gs.credits < cost) return;

        gs.credits -= cost;
        stockUnitsFor(gs, idx) += ErisNum::ErisNum(static_cast<double>(n));
        pnlFor(gs, idx)        -= cost;
    }

    template <typename GS>
    void sellStock(GS& gs, size_t idx, int qty = -1) {
        if (idx >= NUM_STOCK_MARKETS) return;
        ErisNum::ErisNum& units = stockUnitsFor(gs, idx);
        if (units <= ErisNum::ErisNum(0.0)) return;

        const int n = (qty > 0) ? qty : stockTradeQty[idx];
        if (n <= 0) return;

        const ErisNum::ErisNum proceeds(previewSellStock(gs, idx, n));

        ErisNum::ErisNum sellAmt(static_cast<double>(n));
        if (units < sellAmt) sellAmt = units;   // can't sell more than owned

        units                  -= sellAmt;
        gs.credits             += proceeds;
        gs.allCreditProduced   += proceeds;
        pnlFor(gs, idx)     += proceeds;
    }

    void stepTradeQty(size_t idx, int dir) {
        if (idx >= NUM_STOCK_MARKETS) return;
        stockTradeQty[idx] = std::clamp(stockTradeQty[idx] + dir * kTradeQtyStep, 1, kTradeQtyMax);
    }

    void stepDataFrac(int dir) {
        dataSellFrac = std::clamp(dataSellFrac + dir * kSellStep, 0.f, sellMax());
    }
    void stepInfoFrac(int dir) {
        infoSellFrac = std::clamp(infoSellFrac + dir * kSellStep, 0.f, sellMax());
    }

    [[nodiscard]] float tickProgress() const {
        return tickTimer_ / MARKET_TICK_INTERVAL;   // [0, 1)
    }
};

// The Market tab unlocks as soon as selling what you hold COULD be worth one
// Credit: best case = max sell fraction at the top of the price range, with
// every sell-value perk / tree node applied.
template <typename GS>
[[nodiscard]] inline bool tradeValueReachesOneCredit(const GS& gs) {
    const double vol  = static_cast<double>(marketVolCapPerkMult());
    const double tree = treeSellMult() * researchSellMult() * infraMult(gs, InfraEffect::SELL_VALUE);
    const double ceil = researchCeilMult();
    const double sell = static_cast<double>(CreditMarket::sellMax());
    const double dataBest = gs.data.toDouble()        * sell * (1.0 + 2.0 * vol * ceil)
                          * Economy::SELL_DATA_BASE * static_cast<double>(dataSellValuePerkMult()) * tree;
    const double infoBest = gs.information.toDouble() * sell * (1.0 + 5.0 * vol * ceil)
                          * Economy::SELL_INFO_BASE * static_cast<double>(infoSellValuePerkMult()) * tree;
    return dataBest >= 1.0 || infoBest >= 1.0;
}
