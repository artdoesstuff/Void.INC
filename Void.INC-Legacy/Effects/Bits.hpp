#pragma once

#include "../Misc/Globals/GIncludes.hpp"

struct BitParticle {
    sf::Text bit;
    sf::Vector2f pos;
    float speed;
    float spiral;
    int type;
    float verticalDir;

    BitParticle(sf::Font& font, sf::Vector2f startPos, int t)
        : bit(font, ""), pos(startPos), type(t) {
        bit.setString(rand() % 2 == 0 ? "0" : "1");
        bit.setCharacterSize(14);
        bit.setFillColor(sf::Color(243, 238, 225, 40));
        speed = 80.f + (rand() % 120);
        spiral = 0.5f + (static_cast<float>(rand() % 100) / 100.f);
        verticalDir = (rand() % 2 == 0) ? 1.f : -1.f;
    }
};

inline std::vector<BitParticle> dataStream;
inline sf::Clock spawnClock;

inline void updateStream(sf::RenderWindow& window, sf::Vector2f centre, float dt, int set) {
    if (spawnClock.getElapsedTime().asSeconds() > 0.02f) {
        int type = (rand() % 10 < 3) ? 0 : 1;
        bool isSpiralType = (type == 0 && (set == 0 || set == 2));

        sf::Vector2f spawnPos;
        if (isSpiralType) {
            int edge = rand() % 4;
            if (edge == 0) spawnPos = { (float)(rand() % window.getSize().x), -50.f };
            else if (edge == 1) spawnPos = { (float)(rand() % window.getSize().x), (float)window.getSize().y + 50.f };
            else if (edge == 2) spawnPos = { -50.f, (float)(rand() % window.getSize().y) };
            else spawnPos = { (float)window.getSize().x + 50.f, (float)(rand() % window.getSize().y) };
        }
        else {
            spawnPos = { (float)(rand() % window.getSize().x), (rand() % 2 == 0) ? -20.f : (float)window.getSize().y + 20.f };
        }

        dataStream.emplace_back(jetBrainsMono, spawnPos, type);
        spawnClock.restart();
    }

    for (auto it = dataStream.begin(); it != dataStream.end();) {
        bool isSpiralType = (it->type == 0 && (set == 0 || set == 2));

        if (isSpiralType) {
            sf::Vector2f dir = centre - it->pos;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist < 80.f) { it = dataStream.erase(it); continue; }

            dir /= dist;
            sf::Vector2f perp(-dir.y, dir.x);
            float pull = it->speed * (1.f + (600.f / (dist + 1.f)));

            it->pos += dir * pull * dt;
            it->pos += perp * (pull * it->spiral) * dt;

            it->bit.setFillColor(sf::Color(243, 238, 225, (std::uint8_t)std::clamp(dist / 2.0f, 0.f, 150.f)));
        }
        else if (set == 1 || set == 2) {
            it->pos.y += it->verticalDir * it->speed * dt;

            if (it->pos.y < -100.f || it->pos.y > window.getSize().y + 100.f) {
                it = dataStream.erase(it);
                continue;
            }

            it->bit.setFillColor(sf::Color(243, 238, 225, 30));
        }

        it->bit.setPosition(it->pos);
        window.draw(it->bit);
        ++it;
    }
}