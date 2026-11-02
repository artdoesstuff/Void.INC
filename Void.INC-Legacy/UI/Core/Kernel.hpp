#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

inline void positionKernelNodes(sf::Vector2u windowSize) {
    if (kernelTree.empty()) return;

    sf::Vector2f startPos(windowSize.x / 2.f, windowSize.y / 2.f);

    kernelTree[0].pos = startPos;

    kernelTree[1].pos = kernelTree[0].pos + sf::Vector2f(-180.f, -180.f);
    kernelTree[2].pos = kernelTree[0].pos + sf::Vector2f(180.f, -180.f);
    kernelTree[3].pos = kernelTree[0].pos + sf::Vector2f(-180.f, 180.f);
    kernelTree[4].pos = kernelTree[0].pos + sf::Vector2f(180.f, 180.f);

    for (auto& node : kernelTree) {
        node.nodeCircle.setPosition(node.pos);
    }
}

inline void drawKernelLines(sf::RenderWindow& window) {
    auto drawCable = [&](int parentIdx, int childIdx) {
        if (!kernelTree[parentIdx].overwritten) return;

        sf::Vector2f start = kernelTree[parentIdx].pos;
        sf::Vector2f end = kernelTree[childIdx].pos;
        sf::Vector2f dir = end - start;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

        sf::RectangleShape cable({ dist, 3.f });
        cable.setOrigin({ 0.f, 1.5f });
        cable.setPosition(start);
        cable.setRotation(sf::degrees(angle));

        if (kernelTree[childIdx].overwritten) cable.setFillColor(sf::Color(255, 0, 0, 200));
        else cable.setFillColor(sf::Color(100, 0, 0, 80));

        window.draw(cable);
    };

    drawCable(0, 1);
    drawCable(0, 2);
    drawCable(0, 3);
    drawCable(0, 4);
}

inline void drawKernelUI(sf::RenderWindow& window) {
    drawKernelLines(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    int hoveredIdx = -1;

    auto isVisible = [&](int idx) -> bool {
        if (idx == 0) return true;

        if (idx == 1 || idx == 2 || idx == 3 || idx == 4) return kernelTree[0].overwritten;
        return false;
        };

    for (int i = 0; i < kernelTree.size(); ++i) {
        if (!isVisible(i)) continue;

        sf::Vector2f diff = mousePos - kernelTree[i].pos;
        if ((diff.x * diff.x + diff.y * diff.y) < (30.f * 30.f)) {
            hoveredIdx = i;
            kernelTree[i].nodeCircle.setOutlineThickness(4.f);
        } else {
            kernelTree[i].nodeCircle.setOutlineThickness(2.f);
        }

        if (isVisible(i)) {
            if (kernelTree[i].overwritten) {
                kernelTree[i].nodeCircle.setFillColor(sf::Color(60, 0, 0));
                kernelTree[i].nodeCircle.setOutlineColor(sf::Color(255, 50, 50));
            }
            else if (malbytes >= kernelTree[i].malbytes) {
                kernelTree[i].nodeCircle.setFillColor(sf::Color(20, 5, 5));
                kernelTree[i].nodeCircle.setOutlineColor(sf::Color(150, 0, 0));
            }
            else {
                kernelTree[i].nodeCircle.setFillColor(sf::Color(5, 5, 5));
                kernelTree[i].nodeCircle.setOutlineColor(sf::Color(60, 60, 60));
            }
        }

        window.draw(kernelTree[i].nodeCircle);

        sf::Text label(jetBrainsMono, kernelTree[i].name + ".krnl", 12);
        label.setOrigin({ label.getLocalBounds().size.x / 2.f, 0.f });
        label.setPosition(kernelTree[i].pos + sf::Vector2f(0.f, 35.f));
        window.draw(label);

        std::string status = kernelTree[i].overwritten ? "[ OVERWRITTEN ]" : "-" + format(kernelTree[i].malbytes) + " Malbytes";
        sf::Text stat(jetBrainsMono, status, 11);
        stat.setOrigin({ stat.getLocalBounds().size.x / 2.f, 0.f });
        stat.setPosition(kernelTree[i].pos + sf::Vector2f(0.f, 50.f));
        stat.setFillColor(kernelTree[i].overwritten ? sf::Color(255, 50, 50) : sf::Color(150, 150, 150));
        window.draw(stat);
    }

    if (hoveredIdx != -1) {
        sf::Text desc(jetBrainsMono, kernelTree[hoveredIdx].desc, 13);
        sf::FloatRect bounds = desc.getLocalBounds();
        sf::RectangleShape box({ bounds.size.x + 20.f, bounds.size.y + 25.f });
        box.setFillColor(sf::Color(15, 0, 0, 240));
        box.setOutlineColor(sf::Color(255, 0, 0));
        box.setOutlineThickness(1.f);

        sf::Vector2f tPos = mousePos + sf::Vector2f(15.f, 15.f);
        box.setPosition(tPos);
        desc.setPosition(tPos + sf::Vector2f(10.f, 5.f));

        window.draw(box);
        window.draw(desc);
    }
}