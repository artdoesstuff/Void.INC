#pragma once

/**
 * @file Hud.hpp
 * @brief Void.INC — HUD rendering layer (imgui-sfml, SFML 3).
 *
 * Setup (CMake):
 *   git clone --branch SFML3 https://github.com/SFML/imgui-sfml.git
 *   # In CMakeLists.txt:
 *   add_subdirectory(imgui-sfml)
 *   target_link_libraries(Void.INC PRIVATE ImGui-SFML::ImGui-SFML)
 *
 * In main.cpp, before the game loop:
 *   ImGui::SFML::Init(window);
 * Each frame (inside loop):
 *   ImGui::SFML::Update(window, deltaClock.restart());
 *   // ... call hud.update() and hud.drawImGui() here ...
 *   ImGui::SFML::Render(window);
 * On exit:
 *   ImGui::SFML::Shutdown();
 *
 * ---- Layout (1280×720) -----------------------------------------------
 *
 *  ┌──────────────────────────────────────────────────────────────────┐
 *  │                                        │[Stats][Upgrades][Frag] │
 *  │        spiral (left 640px)             │  ← folder tabs (right) │
 *  │                                        │                         │
 *  │   [panel tweens out from right half]   │                         │
 *  │                                        │                         │
 *  ├──────────────────────────────────────────────────────────────────┤
 *  │  DATA  12.3K   INFORMATION  4.5K   ENTROPY  0   (bottom bar)    │
 *  └──────────────────────────────────────────────────────────────────┘
 *
 *  Folder tabs sit flush on the right edge, labels rendered vertically.
 *  Clicking a tab tweens a panel leftward into the right half (never
 *  past x=640). Clicking the same tab again tweens it back. Switching
 *  tabs snaps content instantly but does not re-run the tween.
 *  The bottom currency bar is shown when the Stats tab is closed OR
 *  when the player ticks "Pin currencies to bottom" inside Stats.
 */

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <array>
#include "GameState.hpp"
#include "GameMechanics.hpp"
#include "Supports.hpp"
#include "CreditMarket.hpp"
#include "Marketplace.hpp"
#include "Achievements.hpp"
#include "FragmentTree.hpp"

namespace Palette {
    // SFML colours (kept for SFML draw calls)
    inline constexpr sf::Color Black         {  0,   0,   0, 255};
    inline constexpr sf::Color Divider       { 25,  25,  25, 255};
    inline constexpr sf::Color TextDim       { 60,  60,  60, 255};
    inline constexpr sf::Color TextMid       {120, 120, 120, 255};
    inline constexpr sf::Color TextBright    {220, 220, 220, 255};
    inline constexpr sf::Color Cyan          {  0, 200, 255, 255};
    inline constexpr sf::Color Purple        {160,  80, 255, 255};
    inline constexpr sf::Color Red           {255,  60,  60, 255};
    inline constexpr sf::Color Green         { 80, 255, 160, 255};
    inline constexpr sf::Color Gold          {255, 200,  40, 255};

    // ImVec4 equivalents (0–1 range)
    inline constexpr ImVec4 IV_Black      {0.000f, 0.000f, 0.000f, 1.f};
    inline constexpr ImVec4 IV_BgPanel    {0.040f, 0.040f, 0.040f, 0.97f};
    inline constexpr ImVec4 IV_BgHeader   {0.000f, 0.000f, 0.000f, 1.f};
    inline constexpr ImVec4 IV_Divider    {0.100f, 0.100f, 0.100f, 1.f};
    inline constexpr ImVec4 IV_TextDim    {0.235f, 0.235f, 0.235f, 1.f};
    inline constexpr ImVec4 IV_TextMid    {0.471f, 0.471f, 0.471f, 1.f};
    inline constexpr ImVec4 IV_TextBright {0.863f, 0.863f, 0.863f, 1.f};
    inline constexpr ImVec4 IV_Cyan       {0.000f, 0.784f, 1.000f, 1.f};
    inline constexpr ImVec4 IV_CyanDim    {0.000f, 0.784f, 1.000f, 0.15f};
    inline constexpr ImVec4 IV_Purple     {0.627f, 0.314f, 1.000f, 1.f};
    inline constexpr ImVec4 IV_PurpleDim  {0.627f, 0.314f, 1.000f, 0.15f};
    inline constexpr ImVec4 IV_Red        {1.000f, 0.235f, 0.235f, 1.f};
    inline constexpr ImVec4 IV_RedDim     {1.000f, 0.235f, 0.235f, 0.15f};
    inline constexpr ImVec4 IV_Green      {0.314f, 1.000f, 0.627f, 1.f};
    inline constexpr ImVec4 IV_Gold       {1.000f, 0.784f, 0.157f, 1.f};
    inline constexpr ImVec4 IV_TabBg      {0.020f, 0.020f, 0.020f, 1.f};
    inline constexpr ImVec4 IV_TabHover   {0.000f, 0.784f, 1.000f, 0.20f};
    inline constexpr ImVec4 IV_TabActive  {0.000f, 0.784f, 1.000f, 0.35f};
    inline constexpr ImVec4 IV_BtnHover   {0.000f, 0.784f, 1.000f, 0.18f};
    inline constexpr ImVec4 IV_BtnActive  {0.000f, 0.784f, 1.000f, 0.30f};
    inline constexpr ImVec4 IV_Transparent{0.000f, 0.000f, 0.000f, 0.f};
}

inline void applyTerminalStyle() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 0.f;
    s.FrameRounding     = 2.f;
    s.ScrollbarRounding = 0.f;
    s.GrabRounding      = 2.f;
    s.TabRounding       = 0.f;
    s.WindowBorderSize  = 0.f;
    s.FrameBorderSize   = 1.f;
    s.ItemSpacing       = {8.f, 6.f};
    s.FramePadding      = {6.f, 4.f};
    s.WindowPadding     = {12.f, 12.f};
    s.ScrollbarSize     = 8.f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]           = Palette::IV_BgPanel;
    c[ImGuiCol_ChildBg]            = Palette::IV_Black;
    c[ImGuiCol_PopupBg]            = Palette::IV_BgPanel;
    c[ImGuiCol_Border]             = Palette::IV_Divider;
    c[ImGuiCol_FrameBg]            = {0.08f, 0.08f, 0.08f, 1.f};
    c[ImGuiCol_FrameBgHovered]     = Palette::IV_CyanDim;
    c[ImGuiCol_FrameBgActive]      = Palette::IV_CyanDim;
    c[ImGuiCol_TitleBg]            = Palette::IV_BgHeader;
    c[ImGuiCol_TitleBgActive]      = Palette::IV_BgHeader;
    c[ImGuiCol_TitleBgCollapsed]   = Palette::IV_BgHeader;
    c[ImGuiCol_ScrollbarBg]        = Palette::IV_Black;
    c[ImGuiCol_ScrollbarGrab]      = Palette::IV_Divider;
    c[ImGuiCol_ScrollbarGrabHovered]= Palette::IV_TextDim;
    c[ImGuiCol_ScrollbarGrabActive] = Palette::IV_TextMid;
    c[ImGuiCol_CheckMark]          = Palette::IV_Cyan;
    c[ImGuiCol_SliderGrab]         = Palette::IV_Cyan;
    c[ImGuiCol_SliderGrabActive]   = Palette::IV_Cyan;
    c[ImGuiCol_Button]             = Palette::IV_Transparent;
    c[ImGuiCol_ButtonHovered]      = Palette::IV_BtnHover;
    c[ImGuiCol_ButtonActive]       = Palette::IV_BtnActive;
    c[ImGuiCol_Header]             = Palette::IV_CyanDim;
    c[ImGuiCol_HeaderHovered]      = Palette::IV_TabHover;
    c[ImGuiCol_HeaderActive]       = Palette::IV_TabActive;
    c[ImGuiCol_Separator]          = Palette::IV_Divider;
    c[ImGuiCol_Text]               = Palette::IV_TextBright;
    c[ImGuiCol_TextDisabled]       = Palette::IV_TextDim;
    c[ImGuiCol_Tab]                = Palette::IV_TabBg;
    c[ImGuiCol_TabHovered]         = Palette::IV_TabHover;
    c[ImGuiCol_TabSelected]        = Palette::IV_TabActive;
}

namespace Tween {
    /// Ease in-out cubic: smooth acceleration and deceleration.
    inline float easeInOut(float t) {
        t = std::clamp(t, 0.f, 1.f);
        return t < 0.5f
            ? 4.f * t * t * t
            : 1.f - std::pow(-2.f * t + 2.f, 3.f) / 2.f;
    }

    /// Advance a [0,1] progress value toward `target` at `speed` units/sec.
    inline float advance(float current, float target, float speed, float dt) {
        if (current < target) return std::min(current + speed * dt, target);
        if (current > target) return std::max(current - speed * dt, target);
        return current;
    }
}

enum class FolderID { Stats, Upgrades, Fragment, Market, Infra, None };

struct FolderTab {
    FolderID    id;
    std::string label;    ///< Rendered vertically on the tab button
    ImVec4      accent;   ///< Colour used for the active indicator stripe
};

struct RateSmoother {
    ErisNum::ErisNum prev {0.0};
    ErisNum::ErisNum rate {0.0};
    bool             primed = false;

    static constexpr double kAlpha = 0.10;

    // Feed this a monotonically increasing "total produced" counter, not a
    // spendable balance — otherwise spending/selling shows up as negative rate.
    void update(const ErisNum::ErisNum& current, double dt) {
        if (dt <= 0.0) return;
        if (!primed) {            // first frame: adopt the loaded value as baseline
            prev   = current;
            primed = true;
            return;
        }
        ErisNum::ErisNum inst = (current - prev) / ErisNum::ErisNum(dt);
        rate = rate * ErisNum::ErisNum(1.0 - kAlpha) + inst * ErisNum::ErisNum(kAlpha);
        prev = current;
    }
};

class Hud {
public:
    // ---- Constants -------------------------------------------------------
    static constexpr float kScreenW      = 1280.f;
    static constexpr float kScreenH      = 720.f;
    static constexpr float kSpiralBound  = 640.f;  ///< Panel never crosses this X
    static constexpr float kTabW         = 28.f;   ///< Width of each folder tab strip
    static constexpr float kTabH         = 120.f;  ///< Height of each folder tab button
    static constexpr float kTweenSpeed   = 3.5f;   ///< Progress units per second (0→1 in ~0.29s)
    static constexpr float kBottomBarH   = 36.f;   ///< Height of the bottom currency strip

    // Collapsed panel sits fully off-screen to the right; expanded sits at kSpiralBound.
    // We tween the panel's left-edge X between these two values.
    static constexpr float kPanelXClosed = kScreenW - kTabW;   // only tabs visible
    static constexpr float kPanelXOpen   = kSpiralBound;       // full panel visible

    explicit Hud(const GameState& gs) {
        tabs_ = {
            { FolderID::Stats,     "STATS",    Palette::IV_Cyan   },
            { FolderID::Upgrades,  "SUPPORT",  Palette::IV_Purple },
            { FolderID::Market,    "MARKET",   Palette::IV_Green  },
            { FolderID::Fragment,  "FRAGMENT", Palette::IV_Gold   },
            { FolderID::Infra,     "INFRA",    Palette::IV_Cyan   },
        };

        // Resume the stock/Voidcoin markets from where they were last saved,
        // instead of resetting to their 0.5 starting value on every launch.
        market_.restoreFromSave(gs);
    }

    /// Call every frame before drawImGui(). dt = frame time in seconds.
    void update(GameState& gs, float dt) {
        // Rate smoothers — driven by lifetime-production counters so that
        // spending or selling never makes a rate dip negative.
        rateCredits_.update(gs.allCreditProduced,     dt);
        rateData_.update(gs.allDataProduced,          dt);
        rateInfo_.update(gs.allInformationProduced,   dt);
        rateEntropy_.update(gs.allEntropyProduced,    dt);

        // Market tick — also deducts resources and adds credits
        market_.update(gs, dt);

        // Tween the panel X position
        float targetProgress = (activeFolder_ != FolderID::None) ? 1.f : 0.f;
        tweenProgress_ = Tween::advance(tweenProgress_, targetProgress, kTweenSpeed, dt);

        float easedT = Tween::easeInOut(tweenProgress_);
        currentPanelX_ = kPanelXClosed + (kPanelXOpen - kPanelXClosed) * easedT;
        // Safety: never obscure the spiral
        currentPanelX_ = std::max(currentPanelX_, kSpiralBound);
    }

    /// Call between ImGui::SFML::Update() and ImGui::SFML::Render().
    /// Also call drawSFML() for the non-ImGui elements (bottom bar bg, divider).
    void drawImGui(GameState& gs) {
        drawFolderTabs(gs);
        drawPanel(gs);
        drawTree(gs);
        drawStatusOverlay(gs);
        drawBottomBar(gs);
    }

    /// Call after ImGui::SFML::Render() for raw SFML elements.
    void drawSFML(sf::RenderTarget& target) const {
        drawDividerSFML(target);
    }

    /// Open / close the Fragment tree in the main game area (key T).
    /// Only available once the player has Fragmented at least once.
    void toggleTree(const GameState& gs) {
        if (gs.fragmentation > ErisNum::ErisNum(0.0)) treeOpen_ = !treeOpen_;
    }
    [[nodiscard]] bool treeOpen() const { return treeOpen_; }

    /// Jump to a sub-tab of the VOIDCOIN EXCHANGE panel on the next frame.
    enum class MarketTab { Trade = 0, Stocks = 1, Marketplace = 2 };
    void showMarketTab(MarketTab t) { marketTabRequest_ = static_cast<int>(t); }

    /// Screen rectangles of key buttons from the last frame. Used by the
    /// headless UI tests to click real widgets; harmless otherwise.
    struct Probe { ImVec2 min {0, 0}; ImVec2 max {0, 0}; bool valid = false; };
    [[nodiscard]] const Probe& probeFragment() const { return probeFragment_; }
    [[nodiscard]] const Probe& probeConfirm()  const { return probeConfirm_;  }
    [[nodiscard]] const Probe& probePerk()     const { return probePerk_;     }
    [[nodiscard]] const Probe& probeInfraCell(size_t i) const { return i < probeInfraCells_.size() ? probeInfraCells_[i] : kNoProbe_; }
    [[nodiscard]] const Probe& probeInfraPalette(size_t k) const { return k < probeInfraPal_.size() ? probeInfraPal_[k] : kNoProbe_; }
    [[nodiscard]] const Probe& probeInfraExpand() const { return probeInfraExpand_; }
    [[nodiscard]] const Probe& probeResearch() const { return probeResearch_; }
    [[nodiscard]] const Probe& probeGlitch()   const { return probeGlitch_;   }

    /// Jump to a sub-tab of the SUPPORT panel next frame (0 upgrades, 1 research, 2 glitches).
    void showSupportTab(int t) { supportTabRequest_ = t; }

private:
    bool  treeOpen_         = false;
    bool  confirmFragment_  = false;
    Probe probeFragment_;
    Probe probeConfirm_;
    Probe probePerk_;
    Probe probeResearch_;
    std::vector<Probe> probeInfraCells_;
    std::vector<Probe> probeInfraPal_;
    Probe probeInfraExpand_;
    Probe kNoProbe_;
    int   infraSel_ = 0;          // palette selection; -1 = demolish
    Probe probeGlitch_;
    int   supportTabRequest_ = -1;
    int   marketTabRequest_ = -1;   // sub-tab to force-select next frame, -1 = none
    std::vector<FolderTab> tabs_;
    FolderID activeFolder_  = FolderID::None;
    float    tweenProgress_ = 0.f;   ///< [0 = closed, 1 = open]
    float    currentPanelX_ = kPanelXClosed;

    RateSmoother rateCredits_;
    RateSmoother rateData_;
    RateSmoother rateInfo_;
    RateSmoother rateEntropy_;

    CreditMarket market_;

    // Pin flags are stored in GameState and passed by pointer each frame.
    // These pointers are refreshed in drawImGui() — never hold across frames.
    // (Declared here just for documentation; actual state lives in GameState.)

    void onTabClicked(FolderID id) {
        if (activeFolder_ == id) {
            // Same tab — close
            activeFolder_ = FolderID::None;
            // tweenProgress_ continues from wherever it is; Tween::advance handles it
        } else if (activeFolder_ == FolderID::None) {
            // Nothing open — open this one (tween plays)
            activeFolder_ = id;
        } else {
            // Different tab already open — switch content; the tween target is
            // still "open", so an in-flight slide simply finishes (no snap).
            activeFolder_ = id;
        }
    }

    // Market / Fragment stay hidden until their latch in GameState is set
    // (see GameMechanics::updateUnlocks). Hidden tabs take no slot, so the
    // visible ones always stack from the top.
    [[nodiscard]] static bool isTabUnlocked(const GameState& gs, FolderID id) {
        switch (id) {
            case FolderID::Market:   return gs.marketUnlocked;
            case FolderID::Fragment: return gs.fragmentUnlocked;
            case FolderID::Infra:    return gs.infraUnlocked;
            default:                 return true;
        }
    }

    void drawFolderTabs(const GameState& gs) {
        // One ImGui window per tab, stacked vertically on the right edge.
        // Each is a thin vertical strip with the label rotated via dummy +
        // text drawn manually using ImDrawList.


        int slot = 0;
        for (const FolderTab& tab : tabs_) {
            if (!isTabUnlocked(gs, tab.id)) continue;
            const bool isActive = (activeFolder_ == tab.id);

            float tabY = static_cast<float>(slot++) * (kTabH + 4.f);

            ImGui::SetNextWindowPos({kScreenW - kTabW, tabY});
            ImGui::SetNextWindowSize({kTabW, kTabH});
            ImGui::SetNextWindowBgAlpha(1.f);

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar    |
                ImGuiWindowFlags_NoResize      |
                ImGuiWindowFlags_NoMove        |
                ImGuiWindowFlags_NoScrollbar   |
                ImGuiWindowFlags_NoSavedSettings|
                ImGuiWindowFlags_NoBringToFrontOnFocus;

            std::string winId = "##tab_" + tab.label;
            ImGui::Begin(winId.c_str(), nullptr, flags);

            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 wpos    = ImGui::GetWindowPos();

            // Active indicator stripe on the left edge of the tab
            if (isActive) {
                dl->AddRectFilled(
                    {wpos.x, wpos.y},
                    {wpos.x + 3.f, wpos.y + kTabH},
                    ImGui::ColorConvertFloat4ToU32(tab.accent)
                );
            }

            // Invisible button covering the whole tab for click detection
            ImGui::SetCursorPos({0, 0});
            if (ImGui::InvisibleButton(("##btn_" + tab.label).c_str(), {kTabW, kTabH})) {
                onTabClicked(tab.id);
            }
            if (ImGui::IsItemHovered()) {
                dl->AddRectFilled(
                    {wpos.x, wpos.y},
                    {wpos.x + kTabW, wpos.y + kTabH},
                    ImGui::ColorConvertFloat4ToU32(Palette::IV_BtnHover)
                );
            }

            // Vertical label — rotate text 90° CCW using ImDrawList::AddText
            // We push each character top-to-bottom manually (simpler than
            // font rotation; monospace so spacing is predictable).
            {
                ImU32 labelCol = isActive
                    ? ImGui::ColorConvertFloat4ToU32(tab.accent)
                    : ImGui::ColorConvertFloat4ToU32(Palette::IV_TextMid);

                const float charH   = ImGui::GetTextLineHeight();
                const float totalH  = charH * static_cast<float>(tab.label.size());
                float       charY   = wpos.y + (kTabH - totalH) * 0.5f;
                const float charX   = wpos.x + (kTabW - ImGui::CalcTextSize("A").x) * 0.5f;

                for (char ch : tab.label) {
                    char buf[2] = {ch, '\0'};
                    dl->AddText({charX, charY}, labelCol, buf);
                    charY += charH;
                }
            }

            ImGui::End();
        }
    }

    void drawPanel(GameState& gs) {
        if (tweenProgress_ <= 0.001f) return; // fully closed, nothing to draw

        const float panelW = kScreenW - kTabW - currentPanelX_;
        if (panelW < 2.f) return;

        ImGui::SetNextWindowPos({currentPanelX_, 0.f});
        ImGui::SetNextWindowSize({panelW, kScreenH - kBottomBarH});
        ImGui::SetNextWindowBgAlpha(1.f);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar     |
            ImGuiWindowFlags_NoResize       |
            ImGuiWindowFlags_NoMove         |
            ImGuiWindowFlags_NoSavedSettings|
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("##panel", nullptr, flags);

        // Panel header label
        const char* title = "VOID.INC";
        if      (activeFolder_ == FolderID::Stats)    title = "// STATS";
        else if (activeFolder_ == FolderID::Upgrades) title = "// SUPPORT";
        else if (activeFolder_ == FolderID::Fragment)  title = "// FRAGMENT";
        else if (activeFolder_ == FolderID::Market)    title = "// VOIDCOIN EXCHANGE";
        else if (activeFolder_ == FolderID::Infra)     title = "// INFRASTRUCTURE";

        ImGui::TextColored(Palette::IV_Cyan, "%s", title);
        ImGui::Separator();
        ImGui::Spacing();

        switch (activeFolder_) {
            case FolderID::Stats:    drawStatsContent(gs);    break;
            case FolderID::Upgrades: drawUpgradesContent(gs); break;
            case FolderID::Fragment: drawFragmentContent(gs); break;
            case FolderID::Market:   drawMarketContent(gs);   break;
            case FolderID::Infra:    drawInfraContent(gs);    break;
            default: break;
        }

        ImGui::End();
    }

    void drawStatsContent(GameState& gs) {
        // Helper: draws a currency row, dimmed if not pinned; checkbox toggles pin.
        // 'unlocked' gates whether it appears at all (ascension currencies).
        auto currencyEntry = [&](const char* label,
                                 const ErisNum::ErisNum& value,
                                 const ErisNum::ErisNum& rate,
                                 const ImVec4& accent,
                                 bool* pin,
                                 bool unlocked = true)
        {
            if (!unlocked) return;
            // Checkbox — unique id via label
            std::string cbId = std::string("##pin_") + label;
            ImGui::Checkbox(cbId.c_str(), pin);
            ImGui::SameLine();
            // Dim everything if not pinned
            if (!*pin) ImGui::BeginDisabled();
            drawCurrencyRow(label, value, rate, accent);
            if (!*pin) ImGui::EndDisabled();
        };

        ImGui::TextColored(Palette::IV_TextDim, "PIN TO BOTTOM BAR");
        ImGui::Spacing();

        currencyEntry("CREDITS",     gs.credits,     rateCredits_.rate, Palette::IV_Gold,   &gs.pinCredits);
        currencyEntry("DATA",        gs.data,        rateData_.rate,    Palette::IV_Cyan,   &gs.pinData);
        currencyEntry("INFORMATION", gs.information, rateInfo_.rate,    Palette::IV_Purple, &gs.pinInformation);

        // ENTROPY + void bar on the same line. Only the ENTROPY label/value
        // dims with the pin checkbox — the void rate +/- controls must stay
        // usable even when ENTROPY isn't pinned, since voidRate is a global
        // game setting, not something gated behind pinning a currency.
        {
            bool* pin = &gs.pinEntropy;
            ImGui::Checkbox("##pin_ENTROPY", pin);
            ImGui::SameLine();
            if (!*pin) ImGui::BeginDisabled();

            ImGui::TextColored(Palette::IV_TextMid, "ENTROPY");
            ImGui::SameLine(160.f);
            ImGui::TextColored(Palette::IV_Red, "%s",
                gs.entropy.toString(ErisNum::FormatMode::LatinShort, 2).c_str());

            if (!*pin) ImGui::EndDisabled();

            ImGui::SameLine(0.f, 24.f);
            ImGui::PushStyleColor(ImGuiCol_Button,        Palette::IV_Transparent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);
            if (ImGui::SmallButton(" - ##void"))
                GameMechanics::setVoidPercent(gs, gs.voidRate - 0.05);
            ImGui::PopStyleColor(3);
            ImGui::SameLine(0.f, 4.f);

            {
                const float kVoidMax = static_cast<float>(GameMechanics::voidRateMax());
                const float frac = std::clamp(static_cast<float>(gs.voidRate) / kVoidMax, 0.f, 1.f);
                const float barW = 120.f;
                const float barH = ImGui::GetTextLineHeight();
                ImVec2 pos = ImGui::GetCursorScreenPos();
                pos.y += 4.f;
                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddRectFilled({pos.x, pos.y}, {pos.x + barW, pos.y + barH},
                                  ImGui::ColorConvertFloat4ToU32({0.08f, 0.08f, 0.08f, 1.f}), 2.f);
                if (frac > 0.f)
                    dl->AddRectFilled({pos.x, pos.y}, {pos.x + barW * frac, pos.y + barH},
                                      ImGui::ColorConvertFloat4ToU32(Palette::IV_Red), 2.f);
                dl->AddRect({pos.x, pos.y}, {pos.x + barW, pos.y + barH},
                            ImGui::ColorConvertFloat4ToU32(Palette::IV_Divider), 2.f);
                char pctBuf[16];
                std::snprintf(pctBuf, sizeof(pctBuf), "%.0f%%", gs.voidRate * 100.0);
                ImVec2 ts = ImGui::CalcTextSize(pctBuf);
                dl->AddText({pos.x + (barW - ts.x) * 0.5f, pos.y + (barH - ts.y) * 0.5f},
                            ImGui::ColorConvertFloat4ToU32(Palette::IV_TextBright), pctBuf);
                ImGui::Dummy({barW, barH});
            }

            ImGui::SameLine(0.f, 4.f);
            ImGui::PushStyleColor(ImGuiCol_Button,        Palette::IV_Transparent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);
            if (ImGui::SmallButton(" + ##void"))
                GameMechanics::setVoidPercent(gs, gs.voidRate + 0.05);
            ImGui::PopStyleColor(3);

            // Rate on second line (matching drawCurrencyRow style) — this is
            // part of the ENTROPY readout, so it dims with the pin too.
            if (!*pin) ImGui::BeginDisabled();
            ImGui::TextColored(Palette::IV_TextDim, "  +%s /s",
                rateEntropy_.rate.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
            if (!*pin) ImGui::EndDisabled();
            ImGui::Spacing();
        }

        // A row is shown once the player has EVER had the thing (lifetime
        // counter > 0), is holding some now, or has it pinned — a pinned row
        // must always stay visible so it can be un-pinned again.
        const ErisNum::ErisNum zero(0.0);
        auto shown = [&](const ErisNum::ErisNum& ever, const ErisNum::ErisNum& held, bool pin) {
            return ever > zero || held > zero || pin;
        };

        const bool showFrag  = shown(gs.allDataFragments,  gs.dataFragments,  gs.pinDataFragments);
        const bool showEcho  = shown(gs.allFragmentEchoes, gs.fragmentEchoes, gs.pinFragmentEchoes);
        const bool showAxiom = shown(gs.allDataAxioms,     gs.dataAxioms,     gs.pinDataAxioms);
        const bool showNull  = shown(gs.allNull,           gs.null,           gs.pinNull);

        if (showFrag || showEcho || showAxiom || showNull) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(Palette::IV_TextDim, "ASCENSION");
            ImGui::Spacing();

            currencyEntry("DATA FRAGMENTS",  gs.dataFragments,  zero, Palette::IV_Gold, &gs.pinDataFragments,  showFrag);
            currencyEntry("FRAGMENT ECHOES", gs.fragmentEchoes, zero, Palette::IV_Gold, &gs.pinFragmentEchoes, showEcho);
            currencyEntry("DATA AXIOMS",     gs.dataAxioms,     zero, Palette::IV_Gold, &gs.pinDataAxioms,     showAxiom);
            currencyEntry("NULL",            gs.null,           zero, Palette::IV_Gold, &gs.pinNull,           showNull);
        }

        const bool showHeat  = shown(gs.allHeat,  gs.heat,  gs.pinHeat);
        const bool showCache = shown(gs.allCache, gs.cache, gs.pinCache);
        if (showHeat || showCache) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(Palette::IV_TextDim, "OFFLINE");
            ImGui::Spacing();

            currencyEntry("HEAT",  gs.heat,  zero, Palette::IV_Red,  &gs.pinHeat,  showHeat);
            currencyEntry("CACHE", gs.cache, zero, Palette::IV_Cyan, &gs.pinCache, showCache);
        }

        const bool showSA = shown(gs.stockUnitsA,   gs.stockUnitsA,   gs.pinStockUnitsA);
        const bool showSB = shown(gs.stockUnitsB,   gs.stockUnitsB,   gs.pinStockUnitsB);
        const bool showSC = shown(gs.stockUnitsC,   gs.stockUnitsC,   gs.pinStockUnitsC);
        const bool showVC = shown(gs.voidcoinUnits, gs.voidcoinUnits, gs.pinVoidcoinUnits);
        if (showSA || showSB || showSC || showVC) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(Palette::IV_TextDim, "HOLDINGS");
            ImGui::Spacing();

            currencyEntry("STOCK A",  gs.stockUnitsA,   zero, Palette::IV_Green, &gs.pinStockUnitsA,   showSA);
            currencyEntry("STOCK B",  gs.stockUnitsB,   zero, Palette::IV_Red,   &gs.pinStockUnitsB,   showSB);
            currencyEntry("STOCK C",  gs.stockUnitsC,   zero, Palette::IV_Cyan,  &gs.pinStockUnitsC,   showSC);
            currencyEntry("VOIDCOIN", gs.voidcoinUnits, zero, Palette::IV_Gold,  &gs.pinVoidcoinUnits, showVC);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(Palette::IV_TextDim, "ALL-TIME");
        ImGui::Spacing();

        drawStatRow("Credits produced", gs.allCreditProduced);
        drawStatRow("Data produced",    gs.allDataProduced);
        drawStatRow("Info produced",    gs.allInformationProduced);
        drawStatRow("Entropy produced", gs.allEntropyProduced);

        if (gs.allDataFragments  > ErisNum::ErisNum(0.0))
            drawStatRow("Fragments produced", gs.allDataFragments);
        if (gs.allFragmentEchoes > ErisNum::ErisNum(0.0))
            drawStatRow("Echoes produced",    gs.allFragmentEchoes);
        if (gs.allDataAxioms     > ErisNum::ErisNum(0.0))
            drawStatRow("Axioms produced",    gs.allDataAxioms);
        if (gs.allNull           > ErisNum::ErisNum(0.0))
            drawStatRow("Null produced",      gs.allNull);

        if (gs.allHeat  > ErisNum::ErisNum(0.0))
            drawStatRow("Heat produced",  gs.allHeat);
        if (gs.allCache > ErisNum::ErisNum(0.0))
            drawStatRow("Cache produced", gs.allCache);

        if (gs.fragmentation > ErisNum::ErisNum(0.0))
            drawStatRow("Fragmentations",   gs.fragmentation);
        if (gs.singularity   > ErisNum::ErisNum(0.0))
            drawStatRow("Singularities",    gs.singularity);
        if (gs.convergence   > ErisNum::ErisNum(0.0))
            drawStatRow("Convergences",   gs.convergence);
        if (gs.nullification > ErisNum::ErisNum(0.0))
            drawStatRow("Nullifications", gs.nullification);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        size_t unlockedCount = 0;
        for (const Achievement& a : achievements)
            if (a.unlocked) ++unlockedCount;

        ImGui::TextColored(Palette::IV_Gold, "// ACHIEVEMENTS");
        ImGui::SameLine();
        char achCountBuf[32];
        std::snprintf(achCountBuf, sizeof(achCountBuf), "%zu / %zu",
                      unlockedCount, achievements.size());
        ImGui::TextColored(Palette::IV_TextDim, "%s", achCountBuf);
        ImGui::Spacing();

        const float panelW = ImGui::GetContentRegionAvail().x;
        const float rowH   = 60.f;  // name + desc, no separate progress bar row

        for (size_t i = 0; i < achievements.size(); ++i) {
            const Achievement& a    = achievements[i];
            const bool         done = a.unlocked;
            const float        prog = achievementProgress(achievements, gs, i);

            // Border: dim grey while locked, yellow once complete.
            ImGui::PushStyleColor(ImGuiCol_ChildBg, Palette::IV_Transparent);
            ImGui::PushStyleColor(ImGuiCol_Border,
                done ? ImVec4(Palette::IV_Gold.x, Palette::IV_Gold.y, Palette::IV_Gold.z, 1.f)
                     : ImVec4(0.30f, 0.30f, 0.30f, 0.5f));

            std::string childId = "ach_" + std::to_string(i);
            ImGui::BeginChild(childId.c_str(), {panelW, rowH},
                              ImGuiChildFlags_Borders);

            // Locked: a low-opacity fill spanning [0, prog] of the card width,
            // acting as the progress bar in place of a separate widget.
            // Complete: the whole card fills at the same low opacity, but in
            // the gold/yellow tone, so "fully lit yellow" reads as "done".
            {
                ImDrawList* dl       = ImGui::GetWindowDrawList();
                ImVec2      winPos   = ImGui::GetWindowPos();
                ImVec2      winSize  = ImGui::GetWindowSize();
                ImVec4      fillCol  = done
                    ? ImVec4(Palette::IV_Gold.x, Palette::IV_Gold.y, Palette::IV_Gold.z, 0.16f)
                    : ImVec4(Palette::IV_Cyan.x, Palette::IV_Cyan.y, Palette::IV_Cyan.z, 0.12f);
                float fillW = done ? winSize.x : winSize.x * prog;

                if (fillW > 0.f) {
                    dl->AddRectFilled(winPos, {winPos.x + fillW, winPos.y + winSize.y},
                                      ImGui::ColorConvertFloat4ToU32(fillCol));
                }
            }

            // Name
            ImGui::TextColored(done ? Palette::IV_Gold : Palette::IV_TextBright,
                               "%s", a.name.c_str());

            // Description
            ImGui::TextColored(done ? Palette::IV_TextMid : Palette::IV_TextDim,
                               "%s", a.desc.c_str());

            ImGui::EndChild();
            ImGui::PopStyleColor(2);
            ImGui::Spacing();
        }
    }

    // RESEARCH / GLITCHES appear once there is something to do there; until
    // then the panel is just the Support grid, as before.
    static std::string fmtClock(double secs) {
        const long s = static_cast<long>(std::max(0.0, secs) + 0.5);
        char b[24];
        if (s >= 3600) std::snprintf(b, sizeof(b), "%ld:%02ld:%02ld", s / 3600, (s / 60) % 60, s % 60);
        else           std::snprintf(b, sizeof(b), "%ld:%02ld", s / 60, s % 60);
        return b;
    }

    void drawUpgradesContent(GameState& gs) {
        if (!gs.researchUnlocked && !gs.glitchUnlocked) { drawSupportGrid(gs); return; }

        if (ImGui::BeginTabBar("##suptabs")) {
            if (ImGui::BeginTabItem("UPGRADES", nullptr, (supportTabRequest_ == 0) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                drawSupportGrid(gs);
                ImGui::EndTabItem();
            }
            if (gs.researchUnlocked &&
                ImGui::BeginTabItem("RESEARCH", nullptr, (supportTabRequest_ == 1) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                drawResearchContent(gs);
                ImGui::EndTabItem();
            }
            if (gs.glitchUnlocked &&
                ImGui::BeginTabItem("GLITCHES", nullptr, (supportTabRequest_ == 2) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                drawGlitchContent(gs);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        supportTabRequest_ = -1;
    }

    static ImVec4 accentForCost(RESOURCE::COST r) {
        switch (r) {
            case RESOURCE::COST::CREDITS:     return Palette::IV_Gold;
            case RESOURCE::COST::DATA:        return Palette::IV_Cyan;
            case RESOURCE::COST::INFORMATION: return Palette::IV_Purple;
            case RESOURCE::COST::ENTROPY:     return Palette::IV_Red;
        }
        return Palette::IV_Cyan;
    }
    static const char* nameForCost(RESOURCE::COST r) {
        switch (r) {
            case RESOURCE::COST::CREDITS:     return "CR";
            case RESOURCE::COST::DATA:        return "DATA";
            case RESOURCE::COST::INFORMATION: return "INFO";
            case RESOURCE::COST::ENTROPY:     return "ENTROPY";
        }
        return "";
    }

    void drawResearchContent(GameState& gs) {
        ImGui::TextColored(Palette::IV_TextDim, "One tech at a time. Permanent - Fragmentation keeps research.");
        if (gs.researchActive >= 0 && static_cast<size_t>(gs.researchActive) < researchTree.size()) {
            const ResearchNode& a = researchTree[static_cast<size_t>(gs.researchActive)];
            ImGui::TextColored(Palette::IV_Gold, "RESEARCHING  %s", a.name.c_str());
            char ov[48];
            std::snprintf(ov, sizeof(ov), "%.0f%%  -  %s left", researchFraction(gs) * 100.0, fmtClock(researchSecondsLeft(gs)).c_str());
            ImGui::ProgressBar(static_cast<float>(researchFraction(gs)), {-1.f, 0.f}, ov);
        } else {
            ImGui::TextColored(Palette::IV_TextDim, "Idle.");
        }
        ImGui::Separator();

        probeResearch_.valid = false;
        size_t doneCount = 0;
        for (size_t i = 0; i < researchTree.size(); ++i) {
            const ResearchNode& n = researchTree[i];
            if (n.done) { ++doneCount; continue; }
            if (!researchPrereqsMet(i)) continue;          // locked techs stay hidden
            const bool active = gs.researchActive == static_cast<int32_t>(i);
            const bool can    = canStartResearch(gs, i);
            const ImVec4 acc  = accentForCost(n.resource);

            ImGui::PushID(static_cast<int>(i));
            ImGui::TextColored(acc, "%s", n.name.c_str());
            ImGui::SameLine(0.f, 12.f);
            ImGui::TextColored(Palette::IV_TextDim, "%s %s  -  %s",
                nameForCost(n.resource),
                ErisNum::ErisNum(n.cost).toString(ErisNum::FormatMode::LatinShort, 2).c_str(),
                fmtClock(n.seconds / std::max(researchSpeed(gs), 1e-9)).c_str());
            ImGui::TextWrapped("%s", n.desc.c_str());
            if (active) {
                ImGui::TextColored(Palette::IV_Gold, "in progress");
            } else {
                if (!can) ImGui::BeginDisabled();
                if (ImGui::Button("START")) startResearch(gs, i);
                if (!probeResearch_.valid)
                    probeResearch_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
                if (!can) ImGui::EndDisabled();
            }
            ImGui::Separator();
            ImGui::PopID();
        }

        if (doneCount > 0 && ImGui::CollapsingHeader("COMPLETED")) {
            for (const ResearchNode& n : researchTree)
                if (n.done) {
                    ImGui::TextColored(Palette::IV_Green, "%s", n.name.c_str());
                    ImGui::SameLine(0.f, 10.f);
                    ImGui::TextColored(Palette::IV_TextDim, "%s", n.desc.c_str());
                }
        }
    }

    void drawGlitchContent(GameState& gs) {
        ImGui::TextColored(Palette::IV_TextDim, "Spend Entropy for a timed effect. Re-buying adds time (max x%.0f). Cleared by Fragmentation.", GlitchRules::MAX_STACK);
        ImGui::TextColored(Palette::IV_Red, "ENTROPY");
        ImGui::SameLine(110.f);
        ImGui::TextColored(Palette::IV_Red, "%s", gs.entropy.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::Separator();

        const double dm = researchGlitchDurMult();
        probeGlitch_.valid = false;
        for (size_t i = 0; i < glitches.size(); ++i) {
            const Glitch& g = glitches[i];
            const bool can  = canBuyGlitch(gs, i, dm);
            ImGui::PushID(static_cast<int>(i));
            ImGui::TextColored(Palette::IV_Red, "%s", g.name.c_str());
            ImGui::SameLine(0.f, 12.f);
            ImGui::TextColored(Palette::IV_TextDim, "%s ENTROPY  -  %s",
                ErisNum::ErisNum(g.entropyCost).toString(ErisNum::FormatMode::LatinShort, 2).c_str(),
                fmtClock(g.seconds * dm).c_str());
            ImGui::TextWrapped("%s", g.desc.c_str());
            if (!can) ImGui::BeginDisabled();
            if (ImGui::Button("BUY")) buyGlitch(gs, i, dm);
            if (!probeGlitch_.valid)
                probeGlitch_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
            if (!can) ImGui::EndDisabled();
            if (glitchActive(gs, i)) {
                ImGui::SameLine();
                const float frac = static_cast<float>(std::min(1.0, glitchRemaining(gs, i) / (g.seconds * dm)));
                ImGui::ProgressBar(frac, {-1.f, 0.f}, fmtClock(glitchRemaining(gs, i)).c_str());
            }
            ImGui::Separator();
            ImGui::PopID();
        }
    }

    // Small always-on readout (bottom-left of the main area) for running
    // research and Glitches. Hidden while the Fragment tree covers the area.
    void drawStatusOverlay(const GameState& gs) {
        if (treeOpen_) return;
        const bool res = gs.researchActive >= 0 && static_cast<size_t>(gs.researchActive) < researchTree.size();
        if (!res && !anyGlitchActive(gs)) return;

        ImGui::SetNextWindowPos({10.f, kScreenH - kBottomBarH - 10.f}, ImGuiCond_Always, {0.f, 1.f});
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGui::Begin("##statusov", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        if (res)
            ImGui::TextColored(Palette::IV_Gold, "RESEARCH  %s  %.0f%%  %s",
                researchTree[static_cast<size_t>(gs.researchActive)].name.c_str(),
                researchFraction(gs) * 100.0, fmtClock(researchSecondsLeft(gs)).c_str());
        for (size_t i = 0; i < glitches.size(); ++i)
            if (glitchActive(gs, i))
                ImGui::TextColored(Palette::IV_Red, "%s  %s", glitches[i].name.c_str(), fmtClock(glitchRemaining(gs, i)).c_str());
        ImGui::End();
    }

    void drawInfraContent(GameState& gs) {
        ensureInfraGrid(gs);
        const int  n   = gs.infraSize;
        const double sup = powerSupply(gs), dem = powerDemand(gs), eff = powerEfficiency(gs);
        const auto fmtCr = [](double v) { return ErisNum::ErisNum(v).toString(ErisNum::FormatMode::LatinShort, 2); };

        ImGui::TextColored(Palette::IV_Gold, "CREDITS  %s", gs.credits.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::TextColored(eff >= 1.0 ? Palette::IV_Green : Palette::IV_Red,
            "POWER  %.0f / %.0f   efficiency %.0f%%", sup, dem, eff * 100.0);
        if (eff < 1.0) ImGui::TextColored(Palette::IV_Red, "Underpowered: machine bonuses are scaled down. Build generators.");
        ImGui::Separator();

        probeInfraCells_.assign(static_cast<size_t>(n) * n, Probe{});
        probeInfraPal_.assign(infraKinds.size(), Probe{});
        probeInfraExpand_ = Probe{};

        ImGui::BeginChild("##infrapal", {290.f, 0.f}, ImGuiChildFlags_Borders);
        for (size_t k = 0; k < infraKinds.size(); ++k) {
            const InfraKind& ik = infraKinds[k];
            const double cost = infraCost(gs, k);
            const bool afford = gs.credits >= ErisNum::ErisNum(cost);
            ImGui::PushID(static_cast<int>(k));
            const bool sel = infraSel_ == static_cast<int>(k);
            ImGui::PushStyleColor(ImGuiCol_Button, sel ? Palette::IV_CyanDim : Palette::IV_Transparent);
            char label[96];
            std::snprintf(label, sizeof(label), "%s  x%zu##pal", ik.name.c_str(), infraCount(gs, k));
            if (ImGui::Button(label, {-1.f, 0.f})) infraSel_ = static_cast<int>(k);
            probeInfraPal_[k] = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
            ImGui::PopStyleColor();
            ImGui::TextColored(afford ? Palette::IV_Gold : Palette::IV_TextDim, "  %s CR", fmtCr(cost).c_str());
            if (ik.effect == InfraEffect::POWER) { ImGui::SameLine(); ImGui::TextColored(Palette::IV_Green, "+%.0f power", ik.value); }
            else                                  { ImGui::SameLine(); ImGui::TextColored(Palette::IV_TextDim, "-%.0f power", ik.power); }
            ImGui::TextColored(Palette::IV_TextDim, "  %s", ik.desc.c_str());
            ImGui::PopID();
        }
        ImGui::PushStyleColor(ImGuiCol_Button, infraSel_ == -1 ? Palette::IV_CyanDim : Palette::IV_Transparent);
        if (ImGui::Button("DEMOLISH (50% refund)##dem", {-1.f, 0.f})) infraSel_ = -1;
        ImGui::PopStyleColor();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##infragrid", {0.f, 0.f}, ImGuiChildFlags_Borders);
        const float avail = ImGui::GetContentRegionAvail().x;
        const float cell  = std::min(40.f, (avail - 4.f * (n - 1)) / n);
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                const size_t idx = static_cast<size_t>(r) * n + c;
                const uint8_t v  = gs.infraCells[idx];
                if (c) ImGui::SameLine(0.f, 4.f);
                ImGui::PushID(static_cast<int>(idx));
                const char* lab = v ? infraKinds[v - 1u].name.c_str() : "";
                char txt[8] = "";
                if (v) { txt[0] = lab[0]; txt[1] = '\0'; }
                ImGui::PushStyleColor(ImGuiCol_Button, v ? Palette::IV_CyanDim : Palette::IV_Transparent);
                if (ImGui::Button((std::string(txt) + "##cell").c_str(), {cell, cell})) {
                    if (infraSel_ == -1) demolishBuilding(gs, idx);
                    else if (!v)         placeBuilding(gs, idx, static_cast<size_t>(infraSel_));
                }
                ImGui::PopStyleColor();
                probeInfraCells_[idx] = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
                if (ImGui::IsItemHovered() && v) ImGui::SetTooltip("%s", lab);
                ImGui::PopID();
            }
        }
        ImGui::Spacing();
        if (canExpand(gs)) {
            const double ec = expandCost(gs);
            const bool can = gs.credits >= ErisNum::ErisNum(ec);
            if (!can) ImGui::BeginDisabled();
            char b[64];
            std::snprintf(b, sizeof(b), "EXPAND %dx%d  -  %s CR##expand", n + 1, n + 1, fmtCr(ec).c_str());
            if (ImGui::Button(b, {-1.f, 0.f})) expandGrid(gs);
            probeInfraExpand_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
            if (!can) ImGui::EndDisabled();
        } else {
            ImGui::TextColored(Palette::IV_TextDim, "Grid at maximum size.");
        }

        // Automation toggles appear once the building exists.
        if (infraHas(gs, InfraEffect::AUTO_SUPPORTS) || infraHas(gs, InfraEffect::AUTO_GLITCHES)) {
            ImGui::Separator();
            ImGui::TextColored(Palette::IV_Cyan, "AUTOMATION");
            if (infraHas(gs, InfraEffect::AUTO_SUPPORTS))
                ImGui::Checkbox("Auto-install Supports", &gs.autoSupports);
            if (infraHas(gs, InfraEffect::AUTO_GLITCHES)) {
                for (size_t i = 0; i < glitches.size(); ++i) {
                    bool on = Flags::get(gs.glitchAutoFlags, i);
                    const std::string l = "Keep " + glitches[i].name + " running##ag" + std::to_string(i);
                    if (ImGui::Checkbox(l.c_str(), &on)) Flags::set(gs.glitchAutoFlags, i, on);
                }
            }
        }
        ImGui::EndChild();
    }

    void drawSupportGrid(GameState& gs) {
        // Resolve the all-time total for each resource type.
        auto allTimeFor = [&](RESOURCE::COST r) -> const ErisNum::ErisNum& {
            switch (r) {
                case RESOURCE::COST::CREDITS:     return gs.allCreditProduced;
                case RESOURCE::COST::DATA:        return gs.allDataProduced;
                case RESOURCE::COST::INFORMATION: return gs.allInformationProduced;
                case RESOURCE::COST::ENTROPY:     return gs.allEntropyProduced;
            }
            return gs.allDataProduced;
        };

        auto poolFor = [&](RESOURCE::COST r) -> ErisNum::ErisNum& {
            switch (r) {
                case RESOURCE::COST::CREDITS:     return gs.credits;
                case RESOURCE::COST::DATA:        return gs.data;
                case RESOURCE::COST::INFORMATION: return gs.information;
                case RESOURCE::COST::ENTROPY:     return gs.entropy;
            }
            return gs.data;
        };

        auto accentFor = [](RESOURCE::COST r) -> ImVec4 {
            switch (r) {
                case RESOURCE::COST::CREDITS:     return Palette::IV_Gold;
                case RESOURCE::COST::DATA:        return Palette::IV_Cyan;
                case RESOURCE::COST::INFORMATION: return Palette::IV_Purple;
                case RESOURCE::COST::ENTROPY:     return Palette::IV_Red;
            }
            return Palette::IV_Cyan;
        };

        auto resLabelFor = [](RESOURCE::COST r) -> const char* {
            switch (r) {
                case RESOURCE::COST::CREDITS:     return "CR ";
                case RESOURCE::COST::DATA:        return "DATA ";
                case RESOURCE::COST::INFORMATION: return "INFO ";
                case RESOURCE::COST::ENTROPY:     return "ENTROPY ";
            }
            return "";
        };

        auto extraAccentFor = [](RESOURCE::EXTRA_COST r) -> ImVec4 {
            switch (r) {
                case RESOURCE::EXTRA_COST::CREDITS:     return Palette::IV_Gold;
                case RESOURCE::EXTRA_COST::DATA:        return Palette::IV_Cyan;
                case RESOURCE::EXTRA_COST::INFORMATION: return Palette::IV_Purple;
                case RESOURCE::EXTRA_COST::ENTROPY:     return Palette::IV_Red;
                default:                          return Palette::IV_TextDim;
            }
        };

        auto extraLabelFor = [](RESOURCE::EXTRA_COST r) -> const char* {
            switch (r) {
                case RESOURCE::EXTRA_COST::CREDITS:     return "CR ";
                case RESOURCE::EXTRA_COST::DATA:        return "DATA ";
                case RESOURCE::EXTRA_COST::INFORMATION: return "INFO ";
                case RESOURCE::EXTRA_COST::ENTROPY:     return "ENTROPY ";
                default:                          return "";
            }
        };

        auto boostLabelFor = [](RESOURCE::BOOST r) -> const char* {
            switch (r) {
                case RESOURCE::BOOST::DATA:        return "DATA ";
                case RESOURCE::BOOST::INFORMATION: return "INFO ";
                case RESOURCE::BOOST::ENTROPY:     return "ENTROPY ";
            }
            return "";
        };

        auto boostAccentFor = [](RESOURCE::BOOST r) -> ImVec4 {
            switch (r) {
                case RESOURCE::BOOST::DATA:        return Palette::IV_Cyan;
                case RESOURCE::BOOST::INFORMATION: return Palette::IV_Purple;
                case RESOURCE::BOOST::ENTROPY:     return Palette::IV_Red;
            }
            return Palette::IV_Cyan;
        };

        // Extra-resource affordability — shared with installSupport() so the
        // button state can never disagree with what the purchase will do.
        auto canAffordExtra = [&](const SupportData& u) -> bool {
            return canAffordSupportExtra(u, gs);
        };

        // Collect visible (unpurchased, threshold-met) upgrade indices
        std::vector<size_t> visible;
        for (size_t i = 0; i < supports.size(); ++i) {
            const SupportData& u = supports[i];
            if (u.installed) continue;
            // Visible if allTimeProd >= cost * 1.2
            ErisNum::ErisNum threshold = u.cost * ErisNum::ErisNum(1.2);
            if (allTimeFor(u.resource) < threshold) continue;
            visible.push_back(i);
        }

        if (visible.empty()) {
            ImGui::TextColored(Palette::IV_TextDim, "No upgrades available yet.");
            return;
        }

        // 4-wide grid
        constexpr int   kCols    = 4;
        const float     spacing  = ImGui::GetStyle().ItemSpacing.x;
        const float     cellW    = (ImGui::GetContentRegionAvail().x - spacing * (kCols - 1)) / kCols;
        const float     cellH    = 125.f; // taller to fit extra cost + boost line

        for (size_t vi = 0; vi < visible.size(); ++vi) {
            size_t       i      = visible[vi];
            SupportData& u      = supports[i];
            const bool   newRow = (vi % kCols == 0);

            const ErisNum::ErisNum& pool      = poolFor(u.resource);
            const bool canAffordPrimary       = pool >= u.cost;
            const bool canAffordAll           = canAffordPrimary && canAffordExtra(u);
            const ImVec4 accent               = accentFor(u.resource);
            const char*  resLabel             = resLabelFor(u.resource);

            if (!newRow) ImGui::SameLine(0.f, spacing);

            std::string childId = "##upg_" + std::to_string(i);
            ImVec4 borderCol = canAffordAll
                ? ImVec4(accent.x, accent.y, accent.z, 0.5f)
                : Palette::IV_Divider;

            ImGui::PushStyleColor(ImGuiCol_Border,  borderCol);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, canAffordAll
                ? ImVec4(accent.x, accent.y, accent.z, 0.05f)
                : Palette::IV_Black);

            ImGui::BeginChild(childId.c_str(), {cellW, cellH}, ImGuiChildFlags_Borders);

            ImGui::TextColored(
                canAffordAll ? Palette::IV_TextBright : Palette::IV_TextDim,
                "%s", u.name.c_str()
            );

            std::string costStr = u.cost.toString(ErisNum::FormatMode::LatinShort, 2)
                                + " " + resLabel;
            ImGui::TextColored(canAffordPrimary ? accent : Palette::IV_TextDim,
                               "%s", costStr.c_str());

            if (u.extra != RESOURCE::EXTRA_COST::NONE) {
                ImVec4      exAccent = extraAccentFor(u.extra);
                const char* exLabel  = extraLabelFor(u.extra);
                bool        exOk     = canAffordExtra(u);
                std::string exStr    = u.cost.toString(ErisNum::FormatMode::LatinShort, 2)
                                     + " " + exLabel;
                ImGui::TextColored(exOk ? exAccent : Palette::IV_TextDim,
                                   "%s", exStr.c_str());
            }

            {
                ImVec4      boostCol   = boostAccentFor(u.boost);
                const char* boostLabel = boostLabelFor(u.boost);
                char        multBuf[32];
                std::snprintf(multBuf, sizeof(multBuf), "x%.2f", static_cast<double>(u.boost_multiplier));
                ImGui::TextColored(boostCol, "%s", boostLabel);
                ImGui::SameLine(0.f, 2.f);
                ImGui::TextColored(Palette::IV_Gold, "%s", multBuf);
            }

            if (!canAffordAll) ImGui::BeginDisabled();
            std::string btnId = "INSTALL##" + std::to_string(i);
            ImGui::PushStyleColor(ImGuiCol_Button,        canAffordAll ? Palette::IV_CyanDim : Palette::IV_Transparent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);
            ImGui::SetCursorPosY(cellH + 5.f - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
            if (ImGui::Button(btnId.c_str(), {cellW - ImGui::GetStyle().WindowPadding.x * 2.f, 0}))
                installSupport(supports, gs, i);
            ImGui::PopStyleColor(3);
            if (!canAffordAll) ImGui::EndDisabled();

            ImGui::EndChild();
            ImGui::PopStyleColor(2);
        }
    }

    void drawMarketContent(GameState& gs) {
        const float panelW = ImGui::GetContentRegionAvail().x;
        const float halfW  = (panelW - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
        const float graphH = 130.f;
        const float ctrlH  = 90.f;


        // Credits are always visible, whichever sub-tab is open.
        ImGui::TextColored(Palette::IV_Gold, "CREDITS");
        ImGui::SameLine(110.f);
        ImGui::TextColored(Palette::IV_Gold, "%s", gs.credits.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::SameLine(0.f, 12.f);
        ImGui::TextColored(Palette::IV_TextDim, "+%s /s", rateCredits_.rate.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::Spacing();

        if (ImGui::BeginTabBar("##mkttabs")) {
            if (ImGui::BeginTabItem("TRADE", nullptr, (marketTabRequest_ == 0) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                // Helper: draw one exchange card
                auto drawExchange = [&](const char*    label,
                                         ImVec4         accent,
                                         MarketSeries&  series,
                                         float&         sellFrac,
                                         auto           stepFn,
                                         auto           sellFn,
                                         auto           previewFn)
                {
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, Palette::IV_Black);
                    ImGui::PushStyleColor(ImGuiCol_Border,
                        ImVec4(accent.x, accent.y, accent.z, 0.4f));
                    ImGui::BeginChild(label, {halfW, graphH + ctrlH}, ImGuiChildFlags_Borders);

                    // Name + current % + multiplier
                    float pct = series.displayPct();
                    ImVec4 pctCol = (series.current >= 0.5f) ? Palette::IV_Green : Palette::IV_Red;
                    ImGui::TextColored(accent, "%s", label);
                    ImGui::SameLine();
                    char pctBuf[32];
                    std::snprintf(pctBuf, sizeof(pctBuf), " +%.0f%%", pct);
                    ImGui::TextColored(pctCol, "%s", pctBuf);
                    ImGui::SameLine();
                    char multBuf[16];
                    std::snprintf(multBuf, sizeof(multBuf), "(x%.2f)", series.multiplier());
                    ImGui::TextColored(Palette::IV_TextDim, "%s", multBuf);

                    {
                        ImDrawList* dl  = ImGui::GetWindowDrawList();
                        ImVec2      pos = ImGui::GetCursorScreenPos();
                        float       w   = halfW - ImGui::GetStyle().WindowPadding.x * 2.f;
                        float       h   = graphH - ImGui::GetTextLineHeightWithSpacing() - 8.f;

                        dl->AddRectFilled({pos.x, pos.y}, {pos.x + w, pos.y + h},
                                          ImGui::ColorConvertFloat4ToU32({0.04f, 0.04f, 0.04f, 1.f}));

                        // Midpoint reference
                        float midY = pos.y + h * 0.5f;
                        dl->AddLine({pos.x, midY}, {pos.x + w, midY},
                                    ImGui::ColorConvertFloat4ToU32(Palette::IV_Divider));

                        const auto& hist  = series.history;
                        float       stepX = w / static_cast<float>(MARKET_HISTORY_LEN - 1);
                        for (size_t k = 0; k + 1 < MARKET_HISTORY_LEN; ++k) {
                            float x0 = pos.x + k       * stepX;
                            float x1 = pos.x + (k + 1) * stepX;
                            float y0 = pos.y + h * (1.f - hist[k]);
                            float y1 = pos.y + h * (1.f - hist[k + 1]);
                            float alpha = 0.3f + 0.7f * (static_cast<float>(k) / MARKET_HISTORY_LEN);
                            ImVec4 col  = {accent.x, accent.y, accent.z, alpha};
                            dl->AddLine({x0, y0}, {x1, y1},
                                        ImGui::ColorConvertFloat4ToU32(col), 1.5f);
                        }

                        // Live dot
                        float dotX = pos.x + w;
                        float dotY = pos.y + h * (1.f - series.current);
                        dl->AddCircleFilled({dotX, dotY}, 3.5f,
                                            ImGui::ColorConvertFloat4ToU32(accent));

                        ImGui::Dummy({w, h});
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::PushStyleColor(ImGuiCol_Button,        Palette::IV_Transparent);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);

                    std::string minusId = std::string("- ##m_") + label;
                    std::string plusId  = std::string("+ ##p_") + label;
                    std::string sellId  = std::string("SELL##s_") + label;

                    if (ImGui::SmallButton(minusId.c_str())) stepFn(-1);
                    ImGui::SameLine(0.f, 4.f);

                    char fracBuf[8];
                    std::snprintf(fracBuf, sizeof(fracBuf), "%2.0f%%", sellFrac * 100.f);
                    ImGui::TextColored(sellFrac > 0.f ? accent : Palette::IV_TextDim, "%-4s", fracBuf);
                    ImGui::SameLine(0.f, 4.f);

                    if (ImGui::SmallButton(plusId.c_str())) stepFn(+1);
                    ImGui::SameLine(0.f, 10.f);

                    double preview  = previewFn();
                    bool   canSell  = sellFrac > 0.f && preview > 0.0;
                    if (!canSell) ImGui::BeginDisabled();
                    ImGui::PushStyleColor(ImGuiCol_Button, canSell ? Palette::IV_CyanDim : Palette::IV_Transparent);
                    if (ImGui::SmallButton(sellId.c_str())) sellFn();
                    ImGui::PopStyleColor(1);
                    if (!canSell) ImGui::EndDisabled();

                    // Preview line — exactly what SELL would credit you right now
                    std::string sellStr = ErisNum::ErisNum(preview).toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(canSell ? Palette::IV_Green : Palette::IV_TextDim, "  -> +%s CR", sellStr.c_str());

                    ImGui::PopStyleColor(3);
                    ImGui::Spacing();

                    ImGui::EndChild();
                    ImGui::PopStyleColor(2);
                };

                drawExchange("DATA",  Palette::IV_Cyan,   market_.dataSeries,
                             market_.dataSellFrac,
                             [&](int d){ market_.stepDataFrac(d); },
                             [&](){ market_.sellData(gs); },
                             [&](){ return market_.previewSellData(gs); });

                ImGui::SameLine();

                drawExchange("INFO",  Palette::IV_Purple, market_.infoSeries,
                             market_.infoSellFrac,
                             [&](int d){ market_.stepInfoFrac(d); },
                             [&](){ market_.sellInfo(gs); },
                             [&](){ return market_.previewSellInfo(gs); });
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("STOCKS", nullptr, (marketTabRequest_ == 1) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(Palette::IV_Green, "// STOCKS");
                ImGui::SameLine();
                {
                    const ErisNum::ErisNum totalPnl   = CreditMarket::totalStockPnL(gs);
                    const bool              totalNeg   = totalPnl.isNegative();
                    std::string             totalPnlStr = totalPnl.toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(totalNeg ? Palette::IV_Red : Palette::IV_Green,
                        "NET %s%s CR", totalNeg ? "" : "+", totalPnlStr.c_str());
                }

                ImGui::TextColored(Palette::IV_TextDim,
                    "Price scale x%.2f, set from lifetime earnings at the start of each run.", market_.stockScale());

                const float stockGraphH = 92.f;
                const float stockCtrlH  = 136.f;
                const float stockCardH  = stockGraphH + stockCtrlH;

                static constexpr std::array<ImVec4, NUM_STOCK_MARKETS> kStockAccents = {
                    Palette::IV_Green, Palette::IV_Red, Palette::IV_Cyan, Palette::IV_Gold
                };

                // Helper: draw one stock/Voidcoin card — sparkline + price/owned +
                // a quantity stepper with BUY/SELL buttons (units, not a balance
                // fraction, since you're trading discrete shares for Credits).
                auto drawStockCard = [&](size_t idx) {
                    const char*       label  = kStockNames[idx];
                    ImVec4             accent = kStockAccents[idx];
                    MarketSeries&      series = market_.stockSeries[idx];
                    ErisNum::ErisNum&  units  = CreditMarket::stockUnitsFor(gs, idx);

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, Palette::IV_Black);
                    ImGui::PushStyleColor(ImGuiCol_Border,
                        ImVec4(accent.x, accent.y, accent.z, 0.4f));
                    ImGui::BeginChild(label, {halfW, stockCardH}, ImGuiChildFlags_Borders);

                    // Name + current % + multiplier
                    float  pct    = series.displayPct();
                    ImVec4 pctCol = (series.current >= 0.5f) ? Palette::IV_Green : Palette::IV_Red;
                    ImGui::TextColored(accent, "%s", label);
                    ImGui::SameLine();
                    char pctBuf[40];
                    std::snprintf(pctBuf, sizeof(pctBuf), " +%.0f%%", pct);
                    ImGui::TextColored(pctCol, "%s", pctBuf);
                    ImGui::SameLine();
                    char multBuf[32];
                    std::snprintf(multBuf, sizeof(multBuf), "(x%.2f)", series.multiplier());
                    ImGui::TextColored(Palette::IV_TextDim, "%s", multBuf);

                    {
                        ImDrawList* dl  = ImGui::GetWindowDrawList();
                        ImVec2      pos = ImGui::GetCursorScreenPos();
                        float       w   = halfW - ImGui::GetStyle().WindowPadding.x * 2.f;
                        float       h   = stockGraphH - ImGui::GetTextLineHeightWithSpacing() - 8.f;

                        dl->AddRectFilled({pos.x, pos.y}, {pos.x + w, pos.y + h},
                                          ImGui::ColorConvertFloat4ToU32({0.04f, 0.04f, 0.04f, 1.f}));

                        float midY = pos.y + h * 0.5f;
                        dl->AddLine({pos.x, midY}, {pos.x + w, midY},
                                    ImGui::ColorConvertFloat4ToU32(Palette::IV_Divider));

                        const auto& hist  = series.history;
                        float       stepX = w / static_cast<float>(MARKET_HISTORY_LEN - 1);
                        for (size_t k = 0; k + 1 < MARKET_HISTORY_LEN; ++k) {
                            float x0 = pos.x + k       * stepX;
                            float x1 = pos.x + (k + 1) * stepX;
                            float y0 = pos.y + h * (1.f - hist[k]);
                            float y1 = pos.y + h * (1.f - hist[k + 1]);
                            float alpha = 0.3f + 0.7f * (static_cast<float>(k) / MARKET_HISTORY_LEN);
                            ImVec4 col  = {accent.x, accent.y, accent.z, alpha};
                            dl->AddLine({x0, y0}, {x1, y1},
                                        ImGui::ColorConvertFloat4ToU32(col), 1.5f);
                        }

                        float dotX = pos.x + w;
                        float dotY = pos.y + h * (1.f - series.current);
                        dl->AddCircleFilled({dotX, dotY}, 3.5f,
                                            ImGui::ColorConvertFloat4ToU32(accent));

                        ImGui::Dummy({w, h});
                    }

                    ImGui::Spacing();
                    std::string priceStr = ErisNum::ErisNum(market_.stockPrice(idx))
                                                .toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(Palette::IV_TextDim, "P: %s CR", priceStr.c_str());
                    ImGui::SameLine();
                    std::string ownedStr = units.toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(accent, "OWN: %s", ownedStr.c_str());

                    const ErisNum::ErisNum& pnl    = CreditMarket::pnlFor(gs, idx);
                    const bool               pnlNeg = pnl.isNegative();
                    std::string pnlStr = pnl.toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(pnlNeg ? Palette::IV_Red : Palette::IV_Green,
                        "P/L: %s%s CR", pnlNeg ? "" : "+", pnlStr.c_str());

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::PushStyleColor(ImGuiCol_Button,        Palette::IV_Transparent);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);

                    std::string minusId = std::string("- ##qm_") + label;
                    std::string plusId  = std::string("+ ##qp_") + label;
                    std::string buyId   = std::string("BUY##b_")  + label;
                    std::string sellId  = std::string("SELL##s_") + label;

                    if (ImGui::SmallButton(minusId.c_str())) market_.stepTradeQty(idx, -1);
                    ImGui::SameLine(0.f, 4.f);

                    char qtyBuf[16];
                    std::snprintf(qtyBuf, sizeof(qtyBuf), "x%d", market_.stockTradeQty[idx]);
                    ImGui::TextColored(Palette::IV_TextBright, "%-5s", qtyBuf);
                    ImGui::SameLine(0.f, 4.f);

                    if (ImGui::SmallButton(plusId.c_str())) market_.stepTradeQty(idx, +1);
                    ImGui::SameLine(0.f, 10.f);

                    // Compute previews once — reused for button state, labels, and preview lines
                    const double buyCost     = market_.previewBuyStock(idx);
                    const double sellYield   = market_.previewSellStock(gs, idx);
                    const bool   canBuy      = gs.credits >= ErisNum::ErisNum(buyCost);
                    const bool   canSell     = units > ErisNum::ErisNum(0.0);

                    if (!canBuy) ImGui::BeginDisabled();
                    ImGui::PushStyleColor(ImGuiCol_Button,
                        canBuy ? ImVec4(accent.x, accent.y, accent.z, 0.18f) : Palette::IV_Transparent);
                    if (ImGui::SmallButton(buyId.c_str())) market_.buyStock(gs, idx);
                    ImGui::PopStyleColor(1);
                    if (!canBuy) ImGui::EndDisabled();
                    ImGui::SameLine(0.f, 4.f);

                    if (!canSell) ImGui::BeginDisabled();
                    ImGui::PushStyleColor(ImGuiCol_Button,
                        canSell ? ImVec4(accent.x, accent.y, accent.z, 0.18f) : Palette::IV_Transparent);
                    if (ImGui::SmallButton(sellId.c_str())) market_.sellStock(gs, idx);
                    ImGui::PopStyleColor(1);
                    if (!canSell) ImGui::EndDisabled();

                    // Preview lines — cost to buy / yield from sell at current price
                    std::string buyPreviewStr  = ErisNum::ErisNum(buyCost) .toString(ErisNum::FormatMode::LatinShort, 2);
                    std::string sellPreviewStr = ErisNum::ErisNum(sellYield).toString(ErisNum::FormatMode::LatinShort, 2);
                    ImGui::TextColored(canBuy  ? Palette::IV_Red   : Palette::IV_TextDim, "  BUY %s CR", buyPreviewStr.c_str());
                    ImGui::SameLine(0.f, 8.f);
                    ImGui::TextColored(canSell ? Palette::IV_Green : Palette::IV_TextDim, "SELL +%s CR",   sellPreviewStr.c_str());

                    ImGui::PopStyleColor(3);
                    ImGui::Spacing();

                    ImGui::EndChild();
                    ImGui::PopStyleColor(2);
                };

                drawStockCard(0);
                ImGui::SameLine();
                drawStockCard(1);

                drawStockCard(2);
                ImGui::SameLine();
                drawStockCard(3);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("MARKETPLACE", nullptr, (marketTabRequest_ == 2) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None)) {
                constexpr int   kMCols   = 4;
                const float     mSpacing = ImGui::GetStyle().ItemSpacing.x;
                const float     mCellW   = (panelW - mSpacing * (kMCols - 1)) / kMCols;
                const float     mCellH   = 115.f;

                auto perkTypeLabel = [](MarketPerkType t) -> const char* {
                    switch (t) {
                        case MarketPerkType::DATA_PROD:       return "DATA ";
                        case MarketPerkType::INFO_PROD:       return "INFO ";
                        case MarketPerkType::ENTROPY_PROD:    return "ENTROPY ";
                        case MarketPerkType::MARKET_VOL_CAP:  return "MKR CEIL ";
                        case MarketPerkType::DATA_SELL_VALUE: return "DATA VAL ";
                        case MarketPerkType::INFO_SELL_VALUE: return "INFO VAL ";
                    }
                    return "";
                };
                auto perkTypeAccent = [](MarketPerkType t) -> ImVec4 {
                    switch (t) {
                        case MarketPerkType::DATA_PROD:       return Palette::IV_Cyan;
                        case MarketPerkType::INFO_PROD:       return Palette::IV_Purple;
                        case MarketPerkType::ENTROPY_PROD:    return Palette::IV_Red;
                        case MarketPerkType::MARKET_VOL_CAP:  return Palette::IV_Green;
                        case MarketPerkType::DATA_SELL_VALUE: return Palette::IV_Gold;
                        case MarketPerkType::INFO_SELL_VALUE: return Palette::IV_Gold;
                    }
                    return Palette::IV_TextDim;
                };

                // One purchasable perk card.
                auto drawPerk = [&](size_t pi) {
                    MarketPerk&  p      = marketPerks[pi];
                    const bool   canBuy = gs.credits >= p.cost;
                    const ImVec4 accent = perkTypeAccent(p.type);

                    ImGui::PushStyleColor(ImGuiCol_Border,  canBuy ? ImVec4(accent.x, accent.y, accent.z, 0.5f) : Palette::IV_Divider);
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, canBuy ? ImVec4(accent.x, accent.y, accent.z, 0.05f) : Palette::IV_Black);

                    const std::string cid = "##mp_" + std::to_string(pi);
                    ImGui::BeginChild(cid.c_str(), {mCellW, mCellH}, ImGuiChildFlags_Borders);

                    ImGui::TextColored(canBuy ? Palette::IV_TextBright : Palette::IV_TextDim, "%s", p.name.c_str());
                    ImGui::TextColored(accent, "%s", perkTypeLabel(p.type));
                    ImGui::SameLine(0.f, 4.f);
                    ImGui::TextColored(Palette::IV_Gold, "x%.2f", static_cast<double>(p.multiplier));
                    ImGui::TextColored(canBuy ? Palette::IV_Gold : Palette::IV_TextDim,
                        "%s CR", p.cost.toString(ErisNum::FormatMode::LatinShort, 2).c_str());

                    ImGui::SetCursorPosY(mCellH + 5.f - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
                    if (!canBuy) ImGui::BeginDisabled();
                    ImGui::PushStyleColor(ImGuiCol_Button,        canBuy ? Palette::IV_CyanDim : Palette::IV_Transparent);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Palette::IV_BtnHover);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Palette::IV_BtnActive);
                    const std::string bid = "INSTALL##mp_" + std::to_string(pi);
                    if (ImGui::Button(bid.c_str(), {mCellW - ImGui::GetStyle().WindowPadding.x * 2.f, 0}))
                        buyMarketPerk(marketPerks, gs, pi);
                    if (!probePerk_.valid)
                        probePerk_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
                    ImGui::PopStyleColor(3);
                    if (!canBuy) ImGui::EndDisabled();

                    ImGui::EndChild();
                    ImGui::PopStyleColor(2);
                };

                probePerk_.valid = false;
                size_t shown = 0, owned = 0;
                for (size_t pi = 0; pi < marketPerks.size(); ++pi) {
                    if (marketPerks[pi].purchased) { ++owned; continue; }
                    if (shown % kMCols != 0) ImGui::SameLine(0.f, mSpacing);
                    drawPerk(pi);
                    ++shown;
                }
                if (shown == 0) ImGui::TextColored(Palette::IV_TextDim, "Every perk is installed.");

                // Installed perks fold away so the grid only shows what is still for sale.
                ImGui::Spacing();
                const std::string ownedHdr = "INSTALLED (" + std::to_string(owned) + "/" + std::to_string(marketPerks.size()) + ")";
                if (owned > 0 && ImGui::CollapsingHeader(ownedHdr.c_str())) {
                    for (const MarketPerk& p : marketPerks) {
                        if (!p.purchased) continue;
                        const ImVec4 accent = perkTypeAccent(p.type);
                        ImGui::TextColored(Palette::IV_TextMid, "%s", p.name.c_str());
                        ImGui::SameLine(200.f);
                        ImGui::TextColored(accent, "%s", perkTypeLabel(p.type));
                        ImGui::SameLine(0.f, 4.f);
                        ImGui::TextColored(Palette::IV_Gold, "x%.2f", static_cast<double>(p.multiplier));
                    }
                } else if (owned == 0) {
                    ImGui::TextColored(Palette::IV_TextDim, "INSTALLED (0/%zu)", marketPerks.size());
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        marketTabRequest_ = -1;
    }

    void drawFragmentContent(GameState& gs) {
        using EN = ErisNum::ErisNum;
        const EN zero(0.0);
        auto fmt = [](const EN& v) { return v.toString(ErisNum::FormatMode::LatinShort, 2); };

        const bool everFragmented = gs.fragmentation > zero;
        const std::string head = everFragmented
            ? "FRAGMENTATION #" + fmt(gs.fragmentation + EN(1.0))
            : std::string("FIRST FRAGMENTATION");
        ImGui::TextColored(Palette::IV_Gold, "%s", head.c_str());
        ImGui::Spacing();

        drawStatRow("Fragments held",     gs.dataFragments);
        drawStatRow("Lifetime Fragments", gs.allDataFragments);
        ImGui::TextColored(Palette::IV_TextMid, "Data / Info boost");
        ImGui::SameLine(160.f);
        ImGui::TextColored(Palette::IV_Gold, "x%.2f", GameMechanics::fragmentBoost(gs).toDouble());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(Palette::IV_TextDim, "CONSUMED ON FRAGMENTATION");
        ImGui::Spacing();

        auto row = [&](const char* label, const EN& held, double weight, const ImVec4& col) {
            ImGui::TextColored(col, "%s", label);
            ImGui::SameLine(110.f);
            ImGui::TextColored(Palette::IV_TextBright, "%s", fmt(held).c_str());
            ImGui::SameLine(220.f);
            ImGui::TextColored(Palette::IV_TextDim, "x%g", weight);
            ImGui::SameLine(280.f);
            ImGui::TextColored(Palette::IV_TextMid, "= %s", fmt(held * EN(weight)).c_str());
        };
        row("DATA",        gs.data,        1.0,                          Palette::IV_Cyan);
        row("INFORMATION", gs.information, Economy::FRAG_WEIGHT_INFO,    Palette::IV_Purple);
        row("ENTROPY",     gs.entropy,     Economy::FRAG_WEIGHT_ENTROPY, Palette::IV_Red);
        ImGui::TextColored(Palette::IV_Gold, "CREDITS");
        ImGui::SameLine(110.f);
        ImGui::TextColored(Palette::IV_TextBright, "%s", fmt(gs.credits).c_str());
        ImGui::SameLine(220.f);
        ImGui::TextColored(Palette::IV_TextDim, "wiped, worth nothing");

        const EN value = GameMechanics::fragmentValue(gs);
        const EN gain  = GameMechanics::fragmentGain(gs);
        const bool can = gain >= EN(1.0);

        ImGui::Spacing();
        ImGui::TextColored(Palette::IV_TextMid, "TOTAL VALUE");
        ImGui::SameLine(110.f);
        ImGui::TextColored(Palette::IV_TextBright, "%s", fmt(value).c_str());

        // Progress toward the next whole Fragment.
        {
            const EN target = GameMechanics::fragmentNextTarget(gs);
            const double frac = std::clamp(value.toDouble() / std::max(target.toDouble(), 1.0), 0.0, 1.0);
            ImVec2 pos = ImGui::GetCursorScreenPos();
            const float barW = ImGui::GetContentRegionAvail().x - 4.f;
            const float barH = 14.f;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRectFilled(pos, {pos.x + barW, pos.y + barH}, ImGui::ColorConvertFloat4ToU32({0.08f, 0.08f, 0.08f, 1.f}), 2.f);
            if (frac > 0.0)
                dl->AddRectFilled(pos, {pos.x + barW * static_cast<float>(frac), pos.y + barH},
                                  ImGui::ColorConvertFloat4ToU32(Palette::IV_Gold), 2.f);
            dl->AddRect(pos, {pos.x + barW, pos.y + barH}, ImGui::ColorConvertFloat4ToU32(Palette::IV_Divider), 2.f);
            ImGui::Dummy({barW, barH});
            ImGui::TextColored(Palette::IV_TextDim, "next Fragment at value %s", fmt(target).c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (can) ImGui::TextColored(Palette::IV_Gold, "YIELD  +%s FRAGMENTS", fmt(gain).c_str());
        else     ImGui::TextColored(Palette::IV_TextDim, "YIELD  nothing yet");
        ImGui::Spacing();

        probeFragment_.valid = false;
        probeConfirm_.valid  = false;
        if (!can) confirmFragment_ = false;

        if (!can) ImGui::BeginDisabled();
        if (!confirmFragment_) {
            ImGui::PushStyleColor(ImGuiCol_Button, can ? Palette::IV_PurpleDim : Palette::IV_Transparent);
            if (ImGui::Button("FRAGMENT", {160.f, 0.f})) confirmFragment_ = true;
            ImGui::PopStyleColor();
            probeFragment_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
        } else {
            ImGui::TextColored(Palette::IV_Red, "This wipes your run. Continue?");
            ImGui::PushStyleColor(ImGuiCol_Button, Palette::IV_RedDim);
            if (ImGui::Button("CONFIRM", {160.f, 0.f})) {
                if (GameMechanics::fragment(gs)) treeOpen_ = true;   // show the tree on first reset
                confirmFragment_ = false;
            }
            ImGui::PopStyleColor();
            probeConfirm_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true };
            ImGui::SameLine();
            if (ImGui::Button("CANCEL", {100.f, 0.f})) confirmFragment_ = false;
        }
        if (!can) ImGui::EndDisabled();

        if (everFragmented) {
            ImGui::SameLine(0.f, 16.f);
            if (ImGui::Button(treeOpen_ ? "CLOSE TREE  [T]" : "OPEN TREE  [T]", {160.f, 0.f}))
                treeOpen_ = !treeOpen_;
        } else {
            ImGui::TextColored(Palette::IV_TextDim, "The Fragment tree unlocks with your first Fragmentation.");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextColored(Palette::IV_TextDim, "WIPED");
        ImGui::TextWrapped("Data, Information, Entropy, Credits, Marketplace perks, stock and Voidcoin holdings, "
                           "and Supports (the first %zu are kept by the tree).", treeKeepSupports());
        ImGui::Spacing();
        ImGui::TextColored(Palette::IV_TextDim, "KEPT");
        ImGui::TextWrapped("Fragments, the Fragment tree, achievements, lifetime stats, void rate, pins, unlocks, "
                           "Heat and Cache.");
    }

    void drawTree(GameState& gs) {
        if (!treeOpen_ || !(gs.fragmentation > ErisNum::ErisNum(0.0))) return;

        const float w = kSpiralBound;
        const float h = kScreenH - kBottomBarH;

        ImGui::SetNextWindowPos({0.f, 0.f});
        ImGui::SetNextWindowSize({w, h});
        ImGui::SetNextWindowBgAlpha(0.74f);   // spiral stays faintly visible behind

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar     |
            ImGuiWindowFlags_NoResize       |
            ImGuiWindowFlags_NoMove         |
            ImGuiWindowFlags_NoScrollbar    |
            ImGuiWindowFlags_NoSavedSettings|
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("##fragtree", nullptr, flags);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 wp = ImGui::GetWindowPos();

        ImGui::SetCursorPos({12.f, 10.f});
        ImGui::TextColored(Palette::IV_Gold, "// FRAGMENT TREE");
        ImGui::SameLine(0.f, 16.f);
        ImGui::TextColored(Palette::IV_TextMid, "Fragments: %s",
            gs.dataFragments.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::SetCursorPos({w - 44.f, 8.f});
        if (ImGui::SmallButton(" X ##closetree")) treeOpen_ = false;

        auto centre = [&](const TreeNode& n) { return ImVec2{wp.x + n.x * w, wp.y + n.y * h}; };
        constexpr float kR = 22.f;

        // Edges first, so nodes sit on top.
        for (size_t i = 0; i < fragmentTree.size(); ++i) {
            for (size_t r : fragmentTree[i].requires_) {
                const bool lit = fragmentTree[r].purchased != 0;
                dl->AddLine(centre(fragmentTree[r]), centre(fragmentTree[i]),
                            ImGui::ColorConvertFloat4ToU32(lit ? Palette::IV_Gold : ImVec4(0.25f, 0.25f, 0.25f, 1.f)),
                            lit ? 3.f : 2.f);
            }
        }

        for (size_t i = 0; i < fragmentTree.size(); ++i) {
            TreeNode& n   = fragmentTree[i];
            const ImVec2 c = centre(n);
            const bool owned  = n.purchased != 0;
            const bool avail  = treePrereqsMet(i) && !owned;
            const bool afford = canBuyTreeNode(gs, i);

            ImVec4 ring = owned ? Palette::IV_Gold : afford ? Palette::IV_Cyan : avail ? Palette::IV_TextMid : Palette::IV_TextDim;
            ImVec4 fill = owned ? ImVec4(Palette::IV_Gold.x, Palette::IV_Gold.y, Palette::IV_Gold.z, 0.35f)
                        : afford ? Palette::IV_CyanDim : ImVec4(0.04f, 0.04f, 0.04f, 1.f);

            dl->AddCircleFilled(c, kR, ImGui::ColorConvertFloat4ToU32(fill));
            dl->AddCircle(c, kR, ImGui::ColorConvertFloat4ToU32(ring), 0, 2.f);

            // Cost inside the node (a check mark once owned)
            char costBuf[24];
            if (owned) std::snprintf(costBuf, sizeof(costBuf), "OK");
            else       std::snprintf(costBuf, sizeof(costBuf), "%g", n.cost);
            const ImVec2 cs = ImGui::CalcTextSize(costBuf);
            dl->AddText({c.x - cs.x * 0.5f, c.y - cs.y * 0.5f}, ImGui::ColorConvertFloat4ToU32(ring), costBuf);

            // Name underneath
            const ImVec2 ns = ImGui::CalcTextSize(n.name.c_str());
            dl->AddText({c.x - ns.x * 0.5f, c.y + kR + 4.f},
                        ImGui::ColorConvertFloat4ToU32(owned ? Palette::IV_Gold : avail ? Palette::IV_TextBright : Palette::IV_TextDim),
                        n.name.c_str());

            ImGui::SetCursorScreenPos({c.x - kR, c.y - kR});
            const std::string id = "##node_" + std::to_string(i);
            if (ImGui::InvisibleButton(id.c_str(), {kR * 2.f, kR * 2.f}) && afford)
                buyTreeNode(gs, i);

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(Palette::IV_Gold, "%s", n.name.c_str());
                ImGui::TextUnformatted(n.desc.c_str());
                ImGui::Spacing();
                if (owned) {
                    ImGui::TextColored(Palette::IV_Green, "Owned");
                } else {
                    ImGui::TextColored(afford ? Palette::IV_Cyan : Palette::IV_TextMid, "Cost: %g Fragments", n.cost);
                    if (!treePrereqsMet(i)) {
                        std::string need = "Requires: ";
                        bool first = true;
                        for (size_t r : n.requires_) {
                            if (fragmentTree[r].purchased) continue;
                            if (!first) need += ", ";
                            need += fragmentTree[r].name; first = false;
                        }
                        ImGui::TextColored(Palette::IV_Red, "%s", need.c_str());
                    }
                }
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
    }

    void drawCurrencyRow(const char* label,
                         const ErisNum::ErisNum& value,
                         const ErisNum::ErisNum& rate,
                         const ImVec4& accent) const
    {
        // Label
        ImGui::TextColored(Palette::IV_TextMid, "%s", label);
        ImGui::SameLine(160.f);
        // Value
        ImGui::TextColored(accent, "%s", value.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        // Rate on next line, indented
        ImGui::TextColored(Palette::IV_TextDim, "  +%s /s", rate.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
        ImGui::Spacing();
    }

    void drawStatRow(const char* label, const ErisNum::ErisNum& value) const {
        ImGui::TextColored(Palette::IV_TextMid, "%s", label);
        ImGui::SameLine(160.f);
        ImGui::TextColored(Palette::IV_TextBright, "%s", value.toString(ErisNum::FormatMode::LatinShort, 2).c_str());
    }

    void drawBottomBar(const GameState& gs) {
        ImGui::SetNextWindowPos({0.f, kScreenH - kBottomBarH});
        ImGui::SetNextWindowSize({kScreenW - kTabW, kBottomBarH});
        ImGui::SetNextWindowBgAlpha(0.92f);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar     |
            ImGuiWindowFlags_NoResize       |
            ImGuiWindowFlags_NoMove         |
            ImGuiWindowFlags_NoScrollbar    |
            ImGuiWindowFlags_NoSavedSettings|
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("##bottombar", nullptr, flags);
        ImGui::SetCursorPosY((kBottomBarH - ImGui::GetTextLineHeight()) * 0.5f - 1.f);

        // If pinned entries would run past the window edge, stop and show "..."
        // instead of letting them clip off-screen.
        bool        truncated = false;
        const float limitX    = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x;

        auto entry = [&](const char* lbl, const ErisNum::ErisNum& val, const ImVec4& col, bool pinned) {
            if (!pinned || truncated) return;
            const std::string v = val.toString(ErisNum::FormatMode::LatinShort, 2);
            const float w = ImGui::CalcTextSize(lbl).x + ImGui::GetStyle().ItemSpacing.x
                          + ImGui::CalcTextSize(v.c_str()).x;
            if (ImGui::GetCursorPosX() + w > limitX - 24.f) {
                ImGui::TextColored(Palette::IV_TextDim, "...");
                truncated = true;
                return;
            }
            ImGui::TextColored(Palette::IV_TextDim, "%s", lbl);
            ImGui::SameLine();
            ImGui::TextColored(col, "%s", v.c_str());
            ImGui::SameLine(0.f, 24.f);
        };

        entry("CREDITS", gs.credits,     Palette::IV_Gold,   gs.pinCredits);
        entry("DATA",   gs.data,        Palette::IV_Cyan,   gs.pinData);
        entry("INFO",   gs.information, Palette::IV_Purple, gs.pinInformation);
        entry("ENTROPY",gs.entropy,     Palette::IV_Red,    gs.pinEntropy);
        entry("FRAGS",  gs.dataFragments,  Palette::IV_Gold, gs.pinDataFragments);
        entry("ECHOES", gs.fragmentEchoes, Palette::IV_Gold, gs.pinFragmentEchoes);
        entry("AXIOMS", gs.dataAxioms,     Palette::IV_Gold, gs.pinDataAxioms);
        entry("NULL",   gs.null,           Palette::IV_Gold, gs.pinNull);
        entry("HEAT",   gs.heat,           Palette::IV_Red,  gs.pinHeat);
        entry("CACHE",  gs.cache,          Palette::IV_Cyan, gs.pinCache);
        entry("STOCK A",  gs.stockUnitsA,   Palette::IV_Green, gs.pinStockUnitsA);
        entry("STOCK B",  gs.stockUnitsB,   Palette::IV_Red,   gs.pinStockUnitsB);
        entry("STOCK C",  gs.stockUnitsC,   Palette::IV_Cyan,  gs.pinStockUnitsC);
        entry("VOIDCOIN", gs.voidcoinUnits, Palette::IV_Gold,  gs.pinVoidcoinUnits);

        ImGui::End();
    }

    void drawDividerSFML(sf::RenderTarget& target) const {
        // Vertical line between spiral area and tab strip
        sf::RectangleShape line({1.f, kScreenH - kBottomBarH});
        line.setPosition({kPanelXClosed, 0.f});
        line.setFillColor(Palette::Divider);
        target.draw(line);

        // Bottom bar top edge
        sf::RectangleShape hline({kScreenW, 1.f});
        hline.setPosition({0.f, kScreenH - kBottomBarH});
        hline.setFillColor(Palette::Divider);
        target.draw(hline);
    }
};