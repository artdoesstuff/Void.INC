#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cmath>

#include "includes/GameState.hpp"
#include "includes/GameMechanics.hpp"
#include "includes/Spiral.hpp"
#include "includes/Hud.hpp"
#include "includes/Supports.hpp"
#include "includes/Marketplace.hpp"
#include "includes/Achievements.hpp"
#include "includes/FragmentTree.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode({1280u, 720u}),
        "VOID.INC - 2.0 Rewrite",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    GameState gs = GameMechanics::loadOrNew("save.dat");
    unpackSupport(supports, gs.supportFlags);
    unpackMarket(marketPerks, gs.marketFlags);
    unpackAchievements(achievements, gs.achievementFlags);
    unpackTree(fragmentTree, gs.fragmentTreeFlags);
    unpackResearch(researchTree, gs.researchFlags);
    GameMechanics::setVoidPercent(gs, gs.voidRate);   // re-clamp now that tree bonuses are known

    // ---- Spiral -----------------------------------------------------------
    Spiral spiral;
    sf::Vector2f spiralCentre(320.f, 360.f);

    // ---- ImGui + HUD ------------------------------------------------------
    if (!ImGui::SFML::Init(window)) return 1;
    applyTerminalStyle();
    Hud hud(gs);

    // ---- Timers -----------------------------------------------------------
    sf::Clock deltaClock;
    float     autoSaveTimer = 0.f;
    float     elapsed       = 0.f;
    bool      running       = true;   // cleared by Close / Escape; teardown happens once, after the loop

    // ---- Main loop --------------------------------------------------------
    while (window.isOpen() && running) {
        // -- Events ---------------------------------------------------------
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                running = false;

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    case sf::Keyboard::Key::V:
                        GameMechanics::setVoidPercent(gs, gs.voidRate + 0.05);
                        break;
                    case sf::Keyboard::Key::C:
                        GameMechanics::setVoidPercent(gs, gs.voidRate - 0.05);
                        break;
                    case sf::Keyboard::Key::T:
                        hud.toggleTree(gs);
                        break;
                    case sf::Keyboard::Key::F5:
                        GameMechanics::save(gs);
                        break;
                    case sf::Keyboard::Key::Escape:
                        running = false;
                        break;
                    default: break;
                }
            }
        }

        // Stop before touching ImGui again this frame (Shutdown happens below).
        if (!running) break;

        // -- Delta time -----------------------------------------------------
        const sf::Time sfDt    = deltaClock.restart();
        const float    dt      = std::min(sfDt.asSeconds(), 0.1f);

        // -- Tick -----------------------------------------------------------
        GameMechanics::tick(gs, static_cast<double>(dt));
        GameMechanics::updateUnlocks(gs);
        checkAchievements(achievements, gs);
        hud.update(gs, dt);

        autoSaveTimer += dt;
        if (autoSaveTimer >= GameMechanics::AUTO_SAVE_INTERVAL) {
            GameMechanics::save(gs);
            autoSaveTimer = 0.f;
        }

        // -- Spiral update --------------------------------------------------
        elapsed += dt;
        float logVal      = GameMechanics::dataLog10Float(gs);
        float targetScale = 0.8f + std::min(logVal / 99.f, 1.f) * 0.2f;
        long double allDataLD = static_cast<long double>(gs.allDataProduced.toDouble());
        updateSpiral(spiral, spiralCentre, elapsed, targetScale, allDataLD);

        // -- ImGui frame ----------------------------------------------------
        ImGui::SFML::Update(window, sfDt);
        hud.drawImGui(gs);

        // -- Render ---------------------------------------------------------
        window.clear(sf::Color::Black);
        window.draw(spiral.spiral);
        ImGui::SFML::Render(window);
        hud.drawSFML(window);
        window.display();
    }

    // ---- Teardown (once) ----------------------------------------------------
    GameMechanics::save(gs);
    ImGui::SFML::Shutdown();
    if (window.isOpen()) window.close();

    return 0;
}