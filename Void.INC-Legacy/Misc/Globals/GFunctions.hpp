#pragma once
#include "GIncludes.hpp"
#include "GVariables.hpp"

inline sf::Clock cursorClock;
inline std::string getCursor() {
    return (cursorClock.getElapsedTime().asMilliseconds() % 1000 < 500) ? "_" : " ";
}

inline void centreText(sf::Text& text, sf::Vector2f targetPos) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
    text.setPosition(targetPos);
}

inline void drawTabBox(sf::RenderWindow& window, sf::Vector2f boxSize, std::string title, std::string text, std::function<void(sf::Vector2f innerPos)> drawContent, std::function<void()> onClose) {
    sf::Vector2f centre(window.getSize().x / 2.f, window.getSize().y / 2.f);
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

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

    sf::Text t(jetBrainsMono, title + getCursor(), 14);
    t.setPosition({ titleBar.getPosition().x - (boxSize.x / 2.f) + 10.f, titleBar.getPosition().y + 5.f });
    t.setFillColor(sf::Color(243, 238, 225));

    sf::Text c(jetBrainsMono, "- X", 14);
    c.setPosition({ titleBar.getPosition().x + ((boxSize.x / 2.f) * 0.85f), titleBar.getPosition().y + 5.f });
    c.setFillColor(sf::Color(243, 238, 225));

    window.draw(box);
    window.draw(titleBar);
    window.draw(t);
    window.draw(c);

    std::string body =
        text + getCursor();

    sf::Text w(jetBrainsMono, body, 16);
    w.setOrigin({ w.getGlobalBounds().size.x / 2.f, w.getGlobalBounds().size.y / 2.f - 10.f });
    w.setPosition(centre);
    w.setFillColor(sf::Color(200, 200, 200));

    window.draw(w);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && c.getGlobalBounds().contains(mousePos) && cooldown.getElapsedTime().asMilliseconds() > 200)) {
        onClose();
        playSFX("button");
        cooldown.restart();
       }

    drawContent({ titleBar.getPosition().x - (boxSize.x / 2.f) + 20.f, titleBar.getPosition().y + 45.f });
}

template<typename T>
T lerp(T a, T b, T t) {
    return a + t * (b - a);
}