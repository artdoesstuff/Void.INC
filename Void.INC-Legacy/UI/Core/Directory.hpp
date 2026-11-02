#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Bits/Format.hpp"

inline void positionTreeNodes(sf::Vector2u windowSize) {
    if (dirTree.size() < 9) return;

    sf::Vector2f startPos(windowSize.x / 4.f, windowSize.y / 2.f + 60.f);
    dirTree[0].pos = startPos; // 0

    dirTree[1].pos = dirTree[0].pos + sf::Vector2f(180.f, 0.f); // 1
    dirTree[12].pos = dirTree[1].pos + sf::Vector2f(0.f, -180.f); // 1_1
    dirTree[13].pos = dirTree[1].pos + sf::Vector2f(0.f, 180.f); // 1_2

    dirTree[2].pos = dirTree[1].pos + sf::Vector2f(180.f, 0.f); // 2
    dirTree[20].pos = dirTree[2].pos + sf::Vector2f(0.f, -180.f); // 2_1
	dirTree[27].pos = dirTree[2].pos + sf::Vector2f(0.f, 180.f); // 2_2

    dirTree[9].pos = dirTree[2].pos + sf::Vector2f(180.f, 0.f); // 3
    dirTree[3].pos = dirTree[9].pos + sf::Vector2f(0.f, -180.f); // 3_1
    dirTree[4].pos = dirTree[9].pos + sf::Vector2f(0.f, 180.f); // 3_2

    dirTree[10].pos = dirTree[9].pos + sf::Vector2f(180.f, 0.f); // 4
    dirTree[5].pos = dirTree[10].pos + sf::Vector2f(0.f, -180.f); // 4_1
    dirTree[6].pos = dirTree[10].pos + sf::Vector2f(0.f, 180.f); // 4_2

    dirTree[11].pos = dirTree[10].pos + sf::Vector2f(180.f, 0.f); // 5
    dirTree[7].pos = dirTree[11].pos + sf::Vector2f(0.f, -180.f); // 5_1
    dirTree[8].pos = dirTree[11].pos + sf::Vector2f(0.f, 180.f); // 5_2

	dirTree[16].pos = dirTree[11].pos + sf::Vector2f(180.f, 0.f); // 6
	dirTree[28].pos = dirTree[16].pos + sf::Vector2f(0.f, -180.f); // 6_1
	dirTree[26].pos = dirTree[16].pos + sf::Vector2f(0.f, 180.f); // 6_2

	dirTree[17].pos = dirTree[16].pos + sf::Vector2f(180.f, 0.f); // 7
    dirTree[18].pos = dirTree[17].pos + sf::Vector2f(0.f, -180.f); // 7_1
    dirTree[19].pos = dirTree[17].pos + sf::Vector2f(0.f, 180.f); // 7_2

    dirTree[14].pos = dirTree[0].pos + sf::Vector2f(-180.f, 0.f); // !
    dirTree[21].pos = dirTree[14].pos + sf::Vector2f(0.f, -180.f); // !!

    dirTree[15].pos = dirTree[14].pos + sf::Vector2f(-180.f, 0.f); // @

    dirTree[22].pos = dirTree[0].pos + sf::Vector2f(0.f, -180.f); // A

    dirTree[23].pos = dirTree[22].pos + sf::Vector2f(0.f, -180.f); // B
    dirTree[25].pos = dirTree[23].pos + sf::Vector2f(180.f, 0.f); // B_1

    dirTree[24].pos = dirTree[23].pos + sf::Vector2f(0.f, -180.f); // C
	dirTree[29].pos = dirTree[24].pos + sf::Vector2f(180.f, 0.f); // C_1
	dirTree[30].pos = dirTree[29].pos + sf::Vector2f(180.f, 0.f); // C_2
	dirTree[31].pos = dirTree[30].pos + sf::Vector2f(180.f, 0.f); // C_3
	dirTree[32].pos = dirTree[31].pos + sf::Vector2f(180.f, 0.f); // C_4

    for (auto& node : dirTree) {
        node.nodeCircle.setPosition(node.pos);
    }
}

inline void drawTreeLines(sf::RenderWindow& window) {
    auto drawCable = [&](int parentIdx, int childIdx, bool isOffline = false) {
        if (!dirTree[parentIdx].patched) return;

        sf::Vector2f start = dirTree[parentIdx].pos;
        sf::Vector2f end = dirTree[childIdx].pos;

        sf::Vector2f direction = end - start;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.f / pi;

        sf::RectangleShape cable({ distance, 3.f });
        cable.setOrigin({ 0.f, 1.5f });
        cable.setPosition(start);
        cable.setRotation(sf::degrees(angle));

        if (isOffline) cable.setFillColor(sf::Color(100, 100, 100, 200));
        else {
            if (dirTree[childIdx].patched) {
                cable.setFillColor(sf::Color(0, 255, 150, 200));
            }
            else {
                cable.setFillColor(sf::Color(0, 255, 150, 50));
            }
        }
        window.draw(cable);
    };

    drawCable(0, 1); // 0 to 1
    drawCable(0, 14); // 0 to !
    drawCable(0, 22); // 0 to A

    drawCable(1, 2); // 1 to 2
    drawCable(1, 12, dirTree[1].disabled); // 1 to 1_1
    drawCable(1, 13, dirTree[1].disabled); // 1 to 1_2

    drawCable(2, 9); // 2 to 3
    drawCable(2, 20); // 2 to 2_1
    drawCable(2, 27); // 2 to 2_1

    drawCable(9, 10); // 3 to 4
    drawCable(9, 3); // 3 to 3_1
    drawCable(9, 4, dirTree[4].disabled); // 3 to 3_2

    drawCable(10, 11); // 4 to 5
    drawCable(10, 5); // 4 to 4_1
    drawCable(10, 6); // 4 to 4_2

    drawCable(11, 16); // 5 to 6
    drawCable(11, 7); // 5 to 5_1
    drawCable(11, 8); // 5 to 5_2

    drawCable(16, 17); // 6 to 7
    drawCable(16, 28); // 6 to 6_1
    drawCable(16, 26); // 6 to 6_2

    drawCable(17, 18); // 7 to 7_1
    drawCable(17, 19, dirTree[19].disabled); // 7 to 7_2

    drawCable(14, 21); // ! to !!
    drawCable(14, 15); // ! to @

    drawCable(22, 23); // A to B

    drawCable(23, 24); // B to C
    drawCable(23, 25); // B to B_1

    drawCable(24, 29); // C to C_1
    drawCable(29, 30); // C_1 to C_2
    drawCable(30, 31); // C_2 to C_3
    drawCable(31, 32); // C_3 to C_4
}

inline void drawDirTreeUI(sf::RenderWindow& window) {
    drawTreeLines(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    int hoveredIdx = -1;

    auto isVisible = [&](int idx) -> bool {
        if (idx == 0) return true;

        if (idx == 1 || idx == 14 || idx == 22) return dirTree[0].patched; // Unlock 1, !, and A
        if (idx == 2 || idx == 12 || idx == 13) return dirTree[1].patched; // Unlock 2, 1_1, and 1_2
        if (idx == 9 || idx == 20 || idx == 27) return dirTree[2].patched; // Unlock 3, 2_1, and 2_2
		if (idx == 3 || idx == 4 || idx == 10) return dirTree[9].patched; // Unlock 4, 3_1, and 3_2
		if (idx == 5 || idx == 6 || idx == 11) return dirTree[10].patched; // Unlock 5, 4_1, and 4_2
        if (idx == 7 || idx == 8 || idx == 16) return dirTree[11].patched; // Unlock 6, 5_1, and 5_2
		if (idx == 17 || idx == 26 || idx == 28) return dirTree[16].patched; // Unlock 7, 6_2, and 6_1
		if (idx == 18 || idx == 19) return dirTree[17].patched; // Unlock 7_1 and 7_2
        if (idx == 15 || idx == 21) return dirTree[14].patched; // Unlock @
        if (idx == 23) return dirTree[22].patched; // Unlock B
        if (idx == 24 || idx == 25) return dirTree[23].patched; // Unlock C and B_1
        if (idx == 29) return dirTree[24].patched; // Unlock C_1
        if (idx == 30) return dirTree[29].patched; // Unlock C_2
        if (idx == 31) return dirTree[30].patched; // Unlock C_3
        if (idx == 32) return dirTree[31].patched; // Unlock C_4
        return false;
    };

    for (int i = 0; i < dirTree.size(); ++i) {
        std::string basic = dirTree[i].name + ".sys";
        std::string status;

        if (!isVisible(i)) continue;

        sf::Vector2f diff = mousePos - dirTree[i].pos;
        if ((diff.x * diff.x + diff.y * diff.y) < (30.f * 30.f)) {
            hoveredIdx = i;
            dirTree[i].nodeCircle.setOutlineThickness(4.f);
        }
        else {
            dirTree[i].nodeCircle.setOutlineThickness(2.f);
        }

        if (isVisible(i)) {
            if (dirTree[i].disabled) {
                dirTree[i].nodeCircle.setFillColor(sf::Color(20, 20, 20));
                dirTree[i].nodeCircle.setOutlineColor(sf::Color(100, 100, 100));
                status = "[ OFFLINE ]";
                
            }
            else if (dirTree[i].patched) {
                dirTree[i].nodeCircle.setFillColor(sf::Color(5, 10, 5));
                dirTree[i].nodeCircle.setOutlineColor(sf::Color(0, 255, 150));
                status = "[ ONLINE ]";
            }
            else {
                dirTree[i].nodeCircle.setFillColor(sf::Color(0, 0, 0));
                dirTree[i].nodeCircle.setOutlineColor(sf::Color(50, 100, 75));
                status = "-" + format(dirTree[i].bytes, true) + " Bytes";
            }

            window.draw(dirTree[i].nodeCircle);

            sf::Text label(jetBrainsMono, basic, 12);
            label.setOrigin({ label.getLocalBounds().size.x / 2.f, 0.f });
            label.setPosition(dirTree[i].pos + sf::Vector2f(0.f, 35.f));
            window.draw(label);

		    sf::Text statusText(jetBrainsMono, status, 12);
		    statusText.setOrigin({ statusText.getLocalBounds().size.x / 2.f, 0.f });
		    statusText.setPosition(dirTree[i].pos + sf::Vector2f(0.f, 50.f));
		    window.draw(statusText);
        }
    }

    if (hoveredIdx != -1) {
        sf::Text desc(jetBrainsMono, dirTree[hoveredIdx].desc, 13);
        desc.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = desc.getLocalBounds();
        sf::RectangleShape box({ textBounds.size.x + 20.f, textBounds.size.y + 25.f });
        box.setFillColor(sf::Color(5, 10, 5, 230));
        box.setOutlineColor(sf::Color(100, 100, 100));
        box.setOutlineThickness(1.f);

        sf::Vector2f tPos = mousePos + sf::Vector2f(15.f, 15.f);

        if (tPos.x + box.getSize().x > window.getSize().x) {
            tPos.x -= (box.getSize().x + 20.f);
        }

        box.setPosition(tPos);
        desc.setPosition(tPos + sf::Vector2f(10.f, 5.f));

        window.draw(box);
        window.draw(desc);
    }
}