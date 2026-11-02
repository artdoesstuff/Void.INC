#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Misc/Globals/GFunctions.hpp"
#include "../../Misc/Globals/GVariables.hpp"

struct Cog {
    sf::VertexArray cog;
    float rotation = 0.f;
    float currentRotationSpeed = 0.f;
    sf::FloatRect bounds;

    Cog() {
        cog.setPrimitiveType(sf::PrimitiveType::TriangleFan);
    }
};

inline void updateCog(Cog& cog, sf::RenderWindow& window, float dt, bool& showOptions) {
    const int segments = 16;
    const float outerRadius = 25.f;
    const float innerRadius = 18.f;
    const float margin = 40.f;

    sf::Vector2f winSize = sf::Vector2f(window.getSize());
    sf::Vector2f topRightPos = { winSize.x - margin, margin };

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    cog.bounds = sf::FloatRect({ topRightPos.x - outerRadius, topRightPos.y - outerRadius }, { outerRadius * 2.f, outerRadius * 2.f });

    bool cHover = cog.bounds.contains(mousePos);

    float targetSpeed = (cHover || showOptions) ? 150.f : 0.f;
    cog.currentRotationSpeed = lerp(cog.currentRotationSpeed, targetSpeed, std::clamp(dt * 4.f, 0.f, 1.f));
    cog.rotation += cog.currentRotationSpeed * dt;

    cog.cog.setPrimitiveType(sf::PrimitiveType::TriangleFan);
    cog.cog.clear();

    cog.cog.append(sf::Vertex{ topRightPos, sf::Color::Black });

    for (int i = 0; i <= segments; ++i) {
        float angle = (i * 2.f * pi / segments) + (cog.rotation * pi / 180.f);
        float r = (i % 2 == 0) ? outerRadius : innerRadius;

        sf::Vector2f vertexPos = topRightPos + sf::Vector2f(std::cos(angle) * r, std::sin(angle) * r);

        sf::Color colour = cHover ? sf::Color(100, 100, 100) : sf::Color(40, 40, 40);

        cog.cog.append(sf::Vertex{ vertexPos, colour });
    }

    if ((cHover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) && cooldown.getElapsedTime().asMilliseconds() > 200) {
        showOptions = true;
        canClickOptions = true;
        cooldown.restart();
        playSFX("button");

		if (showStart) canClickStart = false;
        if (!showStart) canClick = false;
        if (reinitialisation) canClickInit = false;
    }
}

struct ConfigOption {
    std::string option;
    bool* toggle;
};

inline void drawOptionsUI(sf::RenderWindow& window, bool& showOptions, sf::Vector2f& centre) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    static std::vector<ConfigOption> options = {
        {"RENDER_FX", &renderEffects},
        {"QUICK_START", &quickStart},
        {"MUTE_ALL", &muteAll},
        {"MUTE_SFX", &muteSFX},
        {"MUTE_AMB", &muteAmbience}
    };

    float linePadding = 30.f;
    sf::Vector2f boxSize(550.f, 90.f + (options.size() * linePadding));

    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    sf::RectangleShape box(boxSize);
    box.setOrigin(boxSize / 2.f);
    box.setPosition(centre);
    box.setFillColor(sf::Color(10, 10, 10));
    box.setOutlineColor(sf::Color(50, 50, 50));
    box.setOutlineThickness(1);

    sf::RectangleShape titleBar({ boxSize.x, 30.f });
    titleBar.setOrigin({ boxSize.x / 2.f, 0.f });
    titleBar.setPosition({ centre.x, centre.y - (boxSize.y / 2.f) });
    titleBar.setFillColor(sf::Color(40, 40, 40));

    sf::Text t(jetBrainsMono, "> void://hardware/options.ini" + getCursor(), 14);
    t.setPosition({ titleBar.getPosition().x - (boxSize.x / 2.f) + 10.f, titleBar.getPosition().y + 5.f });
    t.setFillColor(sf::Color(243, 238, 225));

    sf::Text c(jetBrainsMono, "- X", 14);
    c.setPosition({ titleBar.getPosition().x + ((boxSize.x / 2.f) * 0.85f), titleBar.getPosition().y + 5.f });
    c.setFillColor(sf::Color(243, 238, 225));

    window.draw(box);
    window.draw(titleBar);
    window.draw(t);
    window.draw(c);

    float startY = titleBar.getPosition().y + 45.f;

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Vector2f linePos = { titleBar.getPosition().x - (boxSize.x / 2.f) + 20.f, startY + (i * linePadding) };
        sf::Text optText(jetBrainsMono, options[i].option + ".sys", 13);
        optText.setPosition(linePos);

        bool isHovered = optText.getGlobalBounds().contains(mousePos);
        bool isActive = *options[i].toggle;

        optText.setFillColor(isHovered ? sf::Color(243, 238, 225) : (isActive ? sf::Color(140, 140, 140) : sf::Color(80, 80, 80)));

        if (isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canClickOptions && cooldown.getElapsedTime().asMilliseconds() > 200) {
            *options[i].toggle = !(*options[i].toggle);
            playSFX("button");
            cooldown.restart();
        }
        window.draw(optText);

        sf::Text indicator(jetBrainsMono, isActive ? "[x]" : "[ ]", 13);
        indicator.setPosition({ titleBar.getPosition().x + (boxSize.x / 2.f) - 60.f, linePos.y });
        indicator.setFillColor(optText.getFillColor());
        window.draw(indicator);
    }

    float cyclerY = startY + (options.size() * linePadding);
    sf::Vector2f cyclerPos = { titleBar.getPosition().x - (boxSize.x / 2.f) + 20.f, cyclerY };

    int percent = static_cast<int>(getCorruptionValue(currentCorruption) * 100);
    sf::Text cyclerText(jetBrainsMono, "OFFLINE_CORRUPTION = " + std::to_string(percent) + "%", 13);
    cyclerText.setPosition(cyclerPos);

    bool cyclerHover = cyclerText.getGlobalBounds().contains(mousePos);
    cyclerText.setFillColor(cyclerHover ? sf::Color(243, 238, 225) : sf::Color(80, 80, 80));

    if (cyclerHover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canClickOptions && cooldown.getElapsedTime().asMilliseconds() > 200) {
        int next = (static_cast<int>(currentCorruption) + 1) % static_cast<int>(CorruptionLevel::COUNT);
        currentCorruption = static_cast<CorruptionLevel>(next);

        corruptOffline = getCorruptionValue(currentCorruption);
        offlineMultiplier = 1.f - corruptOffline;

        playSFX("button");
        cooldown.restart();
    }
    window.draw(cyclerText);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && c.getGlobalBounds().contains(mousePos)) {
        showOptions = false;
        canClickOptions = false;
        playSFX("button");

        if (showStart) canClickStart = true;
        if (!showStart) canClick = true;
        if (reinitialisation) canClickInit = true;
    }
}