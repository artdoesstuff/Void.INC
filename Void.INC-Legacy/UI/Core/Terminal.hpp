#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Misc/Lore.hpp"
#include "Download.hpp"

inline int hotfixPage = 0;
const int HF_PER_PAGE = 40;

inline int logicPage = 0;
const int LOGIC_PER_PAGE = 20;

inline void backgroundDeco(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f size, bool buy) {
    sf::Color traceColor = buy ? sf::Color(243, 238, 225, 50) : sf::Color(140, 140, 140, 30);

    sf::VertexArray traces(sf::PrimitiveType::Lines);

    auto addVertex = [&](sf::Vector2f vPos, sf::Color vColor) {
        traces.append(sf::Vertex{ vPos, vColor });
        };

    float l = 15.f;

    addVertex(pos, traceColor);
    addVertex(pos + sf::Vector2f(l, 0.f), traceColor);
    addVertex(pos, traceColor);
    addVertex(pos + sf::Vector2f(0.f, l), traceColor);

    addVertex(pos + size, traceColor);
    addVertex(pos + size - sf::Vector2f(l, 0.f), traceColor);
    addVertex(pos + size, traceColor);
    addVertex(pos + size - sf::Vector2f(0.f, l), traceColor);

    sf::Vector2f traceStart = pos + sf::Vector2f(size.x * 0.75f, 0.f);

    addVertex(traceStart, traceColor);
    addVertex(traceStart + sf::Vector2f(35.f, 20.f), traceColor);

    addVertex(traceStart + sf::Vector2f(35.f, 20.f), traceColor);
    addVertex(traceStart + sf::Vector2f(35.f, size.y), traceColor);

    window.draw(traces);

    sf::CircleShape pad(2.f);
    pad.setFillColor(traceColor);

    for (int i = 0; i < 3; ++i) {
        pad.setPosition(pos + sf::Vector2f(-1.f, 20.f + (i * 15.f)));
        window.draw(pad);
    }
}

inline void drawTerminalModule(sf::RenderWindow& window, std::string title, float animation, std::function<void(sf::Vector2f)> contentDraw) {
    if (animation <= 0.f) return;

    sf::Vector2u winSize = window.getSize();
    float width = winSize.x * 0.8f;
    float height = winSize.y * 0.7f;

    float targetY = (winSize.y - height) / 2.f;
    float currentY = winSize.y - (height * animation) - targetY;

    sf::RectangleShape overlay({ (float)winSize.x, (float)winSize.y });
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(180 * animation)));
    window.draw(overlay);

    sf::RectangleShape frame({ width, height });
    frame.setPosition({ (winSize.x - width) / 2.f, currentY });
    frame.setFillColor(sf::Color(10, 10, 10));
    frame.setOutlineColor(sf::Color(50, 50, 50, static_cast<std::uint8_t>(255 * animation)));
    frame.setOutlineThickness(1.f);
    window.draw(frame);

    sf::RectangleShape titleBar({ width, 30.f });
    titleBar.setPosition(frame.getPosition());
    titleBar.setFillColor(sf::Color(40, 40, 40));
    window.draw(titleBar);

    sf::Text t(jetBrainsMono, "> " + title, 14);
    t.setPosition(titleBar.getPosition() + sf::Vector2f(10.f, 5.f));
    t.setFillColor(sf::Color(243, 238, 225));
    window.draw(t);

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::Text c(jetBrainsMono, "- X", 14);
    c.setPosition({ titleBar.getPosition().x + (width * 0.97f), titleBar.getPosition().y + 5.f});
    c.setFillColor(sf::Color(243, 238, 225));
    bool canClose = c.getGlobalBounds().contains(mousePos);
    window.draw(c);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canClose) {
        activeTab = Tab::NONE;
        playSFX("button");
    }

    contentDraw(frame.getPosition() + sf::Vector2f(20.f, 50.f));
}

inline void drawTerminalUI(sf::RenderWindow& window, long double& bits, long double allBits, float dt) {
    sf::Vector2u winSize = window.getSize();
    float moduleWidth = winSize.x * 0.8f;
    float moduleHeight = winSize.y * 0.7f;

    if (activeTab != Tab::NONE) tabProgress = std::min(1.f, tabProgress + dt * 5.f);
    else tabProgress = std::max(0.f, tabProgress - dt * 5.f);

    auto drawTabButton = [&](std::string label, float x, Tab type) {
        sf::RectangleShape btn({ 180.f, 40.f });
        btn.setPosition({ x, winSize.y - 50.f });
        btn.setFillColor(activeTab == type ? sf::Color(40, 40, 40, 150) : sf::Color(15, 15, 15, 200));
        btn.setOutlineColor(sf::Color(50, 50, 50, 100));
        btn.setOutlineThickness(1.f);
        window.draw(btn);

        sf::Text txt(jetBrainsMono, label, 13);
        txt.setPosition(btn.getPosition() + sf::Vector2f(15.f, 10.f));
        window.draw(txt);
    };

    if (!reinitialisation && !corrupting) {
        drawTabButton("logic.bat", 10.f, Tab::LOGIC);
        drawTabButton("hotfixes.bat", 200.f, Tab::HOTFIX);
        drawTabButton("stats.bat", 390.f, Tab::STATS);
        if (allBits >= 5000000.0L) drawTabButton("reinit.bat", 580.f, Tab::REINIT);
        if (allBits >= 1e45L) drawTabButton("logs.bat", 770.f, Tab::LOGS);
    }

    if (reinitialisation) {
        drawTabButton("init.bat", 10.f, Tab::INIT);
        if (allBits >= 1e63) drawTabButton("not_sus.bat", 200.f, Tab::CORRUPT);
    }

    if (corrupting) drawTabButton("reboot.bat", 10.f, Tab::REBOOT);

    if (activeTab == Tab::LOGIC) {
        drawTerminalModule(window, (currentLogicMode == Currency::BIT ? "void://hardware/logic.bat" : "void://hardware/mal_logic.bat") + getCursor(), tabProgress, [&](sf::Vector2f start) {
            float boxW = std::round((moduleWidth - 100.f) / 5.f);
            float boxH = std::round((moduleHeight - 140.f) / 4.f);
            float pad = 10.f;
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            std::vector<Logic*> filtered;
            for (auto& lg : logicGateList) {
                if (lg.costType == currentLogicMode) filtered.push_back(&lg);
            }

            size_t startIdx = logicPage * LOGIC_PER_PAGE;
            size_t endIdx = std::min(startIdx + LOGIC_PER_PAGE, filtered.size());

            for (size_t i = startIdx; i < endIdx; ++i) {
                auto& lg = *filtered[i];
                int localIdx = static_cast<int>(i - startIdx);
                int col = localIdx % 5;
                int row = localIdx / 5;

                sf::Vector2f pos(
                    std::round(start.x + col * (boxW + pad)),
                    std::round(start.y + row * (boxH + pad))
                );

                bool isHovered = sf::FloatRect(pos, {boxW, boxH}).contains(mousePos);
                long double currency = (currentLogicMode == Currency::BIT) ? bits : malbits;
                auto [totalCost, amount] = getDownload(lg, currency, currentBuy);
                bool canAfford = currency >= totalCost;

                lg.rect.setSize({ boxW, boxH });
                lg.rect.setPosition(pos);
                lg.rect.setFillColor(isHovered ? sf::Color(20, 20, 20) : sf::Color::Black);
                lg.rect.setOutlineThickness(1.f);
                lg.rect.setOutlineColor(canAfford ? sf::Color(100, 100, 100) : sf::Color(50, 50, 50));
                if (isHovered) lg.rect.setOutlineColor(sf::Color(243, 238, 225));

                window.draw(lg.rect);
                backgroundDeco(window, pos, { boxW, boxH }, canAfford);

                for (int p = 0; p < 4; ++p) {
                    sf::RectangleShape pin({ 4.f, 2.f });
                    pin.setFillColor(sf::Color(243, 238, 225, 80));
                    float pinY = std::round(pos.y + 15.f + (p * 15.f));
                    pin.setPosition({ std::round(pos.x - 2.f), pinY });
                    window.draw(pin);
                    pin.setPosition({ std::round(pos.x + boxW - 2.f), pinY });
                    window.draw(pin);
                }

                sf::Text n(jetBrainsMono, lg.name + "_" + std::to_string(lg.ver) + ".bin", 12);
                n.setPosition({ std::round(pos.x + 12.f), std::round(pos.y + 10.f) });
                n.setFillColor(sf::Color(243, 238, 225));
                window.draw(n);

                std::string label = (currentLogicMode == Currency::BIT) ? "_bits.tmp" : "_malbits.tmp";
                sf::Text c(jetBrainsMono, "-" + format(totalCost) + label + " x" + std::to_string(amount), 10);
                c.setPosition({ std::round(pos.x + 12.f), std::round(pos.y + boxH - 22.f) });
                c.setFillColor(canAfford ? sf::Color(243, 238, 225, 200) : sf::Color(140, 140, 140));
                window.draw(c);
            }

            float footerY = std::round(start.y + moduleHeight - 90.f);
            float centerX = std::round(start.x + moduleWidth / 2.f);
            int totalPages = std::max(1, (int)std::ceil((float)filtered.size() / LOGIC_PER_PAGE));

            auto drawNav = [&](const std::string& str, float xOffset, int target) {
                sf::Text t(jetBrainsMono, str, 14);
                t.setOrigin({ std::round(t.getLocalBounds().size.x / 2.f), 0.f });
                t.setPosition({ std::round(centerX + xOffset), footerY });
                bool hovered = t.getGlobalBounds().contains(mousePos);
                t.setFillColor(hovered ? sf::Color::White : sf::Color(100, 100, 100));
                window.draw(t);

                if (hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
                    logicPage = std::clamp(target, 0, totalPages - 1);
                    playSFX("button");
                    cooldown.restart();
                }
            };

            drawNav("<<<", -100.f, logicPage - 5);
            drawNav("<", -65.f, logicPage - 1);
            drawNav(">", 65.f, logicPage + 1);
            drawNav(">>>", 100.f, logicPage + 5);

            sf::Text pgNum(jetBrainsMono, "PAGE " + std::to_string(logicPage + 1) + "/" + std::to_string(totalPages), 12);
            pgNum.setOrigin({ std::round(pgNum.getLocalBounds().size.x / 2.f), 0.f });
            pgNum.setPosition({ centerX, footerY });
            window.draw(pgNum);

            auto drawModeBtn = [&](std::string label, float xOffset, Currency mode) {
                sf::RectangleShape b({ 120.f, 25.f });
                sf::Vector2f bPos(std::round(start.x + xOffset), footerY);
                b.setPosition(bPos);
                bool hovered = b.getGlobalBounds().contains(mousePos);
                bool active = currentLogicMode == mode;

                b.setFillColor(active ? sf::Color(243, 238, 225) : sf::Color::Black);
                b.setOutlineColor(sf::Color(243, 238, 225, 150));
                b.setOutlineThickness(1.f);
                window.draw(b);

                sf::Text mt(jetBrainsMono, label, 10);
                mt.setOrigin({ std::round(mt.getLocalBounds().size.x / 2.f), 0.f });
                mt.setPosition({ std::round(bPos.x + 60.f), std::round(bPos.y + 5.f) });
                mt.setFillColor(active ? sf::Color::Black : (hovered ? sf::Color::White : sf::Color(150, 150, 150)));
                window.draw(mt);

                if (hovered && !active && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
                    currentLogicMode = mode;
                    logicPage = 0;
                    playSFX("button");
                    cooldown.restart();
                }
            };

            drawModeBtn("STANDARD", 0.f, Currency::BIT);
            drawModeBtn("MALICIOUS", 130.f, Currency::MALBIT);

            float btnX = start.x + moduleWidth - 360.f;
            std::vector<std::pair<std::string, Download>> modes = {
                {"x1", Download::X1}, {"x5", Download::X5}, {"x10", Download::X10},
                {"x50", Download::X50}, {"x100", Download::X100}, {"MAX", Download::MAX}
            };

            for (auto& m : modes) {
                sf::RectangleShape b({ 45.f, 25.f });
                sf::Vector2f bPos(std::round(btnX), footerY);
                b.setPosition(bPos);
                bool hovered = b.getGlobalBounds().contains(mousePos);
                bool active = currentBuy == m.second;

                b.setFillColor(active ? sf::Color(60, 60, 60) : sf::Color::Black);
                b.setOutlineColor(active ? sf::Color(243, 238, 225) : sf::Color(60, 60, 60));
                b.setOutlineThickness(1.f);
                window.draw(b);

                sf::Text bt(jetBrainsMono, m.first, 10);
                bt.setOrigin({ std::round(bt.getLocalBounds().size.x / 2.f), 0.f });
                bt.setPosition({ std::round(bPos.x + 22.f), std::round(bPos.y + 5.f) });
                bt.setFillColor(active ? sf::Color::White : sf::Color(120, 120, 120));
                window.draw(bt);

                if (hovered && !active && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
                    currentBuy = m.second;
                    playSFX("button");
                    cooldown.restart();
                }
                btnX += 51.f;
            }
        });
    }

    else if (activeTab == Tab::HOTFIX) {
        drawTerminalModule(window, "void://hardware/hotfixes.bat" + getCursor(), tabProgress, [&](sf::Vector2f start) {
            float size = (moduleWidth - 140.f) / 10.f;
            float pad = 10.f;
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            size_t startIdx = hotfixPage * HF_PER_PAGE;
            size_t endIdx = std::min(startIdx + HF_PER_PAGE, hotfixList.size());

            for (size_t i = startIdx; i < endIdx; ++i) {
                auto& hf = hotfixList[i];
                int localIdx = static_cast<int>(i - startIdx);

                sf::Vector2f pos(
                    std::round(start.x + (localIdx % 10) * (size + pad)),
                    std::round(start.y + (localIdx / 10) * (size + pad))
                );

                bool isHovered = sf::FloatRect(pos, { size, size }).contains(mousePos);
                bool canAfford = bits >= hf.bits;

                hf.rect.setSize({ std::round(size), std::round(size) });
                hf.rect.setPosition(pos);
                hf.rect.setFillColor(isHovered ? sf::Color(25, 25, 25) : sf::Color(5, 5, 5));
                hf.rect.setOutlineThickness(1.f);
                hf.rect.setOutlineColor(hf.written ? sf::Color(60, 60, 60) : (isHovered ? sf::Color(243, 238, 225) : sf::Color(40, 40, 40)));
                window.draw(hf.rect);

                backgroundDeco(window, pos, { size, size }, canAfford && !hf.written);

                sf::Text n(jetBrainsMono, hf.name + (hf.written ? ".bin" : ".pkg\n-" + format(hf.bits) + "_bits.tmp"), 9);
                n.setPosition({ std::round(pos.x + 6.f), std::round(pos.y + 6.f) });
                n.setFillColor(hf.written ? sf::Color(100, 100, 100) : sf::Color(243, 238, 225));
                window.draw(n);

                sf::RectangleShape status({ size - 12.f, 2.f });
                status.setPosition({ std::round(pos.x + 6.f), std::round(pos.y + size - 8.f) });
                if (hf.written) status.setFillColor(sf::Color(100, 255, 100, 150));
                else status.setFillColor(canAfford ? sf::Color(243, 238, 225, 80) : sf::Color(255, 50, 50, 40));
                window.draw(status);
            }

            float navY = std::round(start.y + moduleHeight - 90.f);
            float centerX = std::round(start.x + moduleWidth / 2.f);
            int totalPages = std::max(1, (int)std::ceil((float)hotfixList.size() / HF_PER_PAGE));

            auto drawNavBtn = [&](std::string label, float xOffset, int pageTarget) {
                sf::Text t(jetBrainsMono, label, 12);
                sf::FloatRect bounds = t.getLocalBounds();
                t.setOrigin({ std::round(bounds.size.x / 2.f), 0.f });
                t.setPosition({ std::round(centerX + xOffset), navY });

                bool hovered = t.getGlobalBounds().contains(mousePos);
                t.setFillColor(hovered ? sf::Color::White : sf::Color(100, 100, 100));
                window.draw(t);

                if (hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 150) {
                    hotfixPage = std::clamp(pageTarget, 0, totalPages - 1);
                    playSFX("button");
                    cooldown.restart();
                }
            };

            drawNavBtn("<<<", -110.f, hotfixPage - 5);
            drawNavBtn("<", -70.f, hotfixPage - 1);
            drawNavBtn(">", 70.f, hotfixPage + 1);
            drawNavBtn(">>>", 110.f, hotfixPage + 5);

            sf::Text pgNum(jetBrainsMono, "SEC_LOAD: " + std::to_string(hotfixPage + 1) + " / " + std::to_string(totalPages), 12);
            pgNum.setOrigin({ std::round(pgNum.getLocalBounds().size.x / 2.f), 0.f });
            pgNum.setPosition({ centerX, navY });
            pgNum.setFillColor(sf::Color(243, 238, 225));
            window.draw(pgNum);

            sf::RectangleShape progressBack({ 200.f, 2.f });
            progressBack.setPosition( {std::round(centerX - 100.f), navY + 20.f} );
            progressBack.setFillColor(sf::Color(50, 50, 50));
            window.draw(progressBack);

            sf::RectangleShape progressFill({ (200.f / totalPages) * (hotfixPage + 1), 2.f });
            progressFill.setPosition(progressBack.getPosition());
            progressFill.setFillColor(sf::Color(243, 238, 225, 150));
            window.draw(progressFill);

            int affordable = 0;
            for (const auto& hf : hotfixList) { if (!hf.written && bits >= hf.bits) affordable++; }

            if (affordable > 0) {
                sf::RectangleShape bBtn({ 210.f, 30.f });
                sf::Vector2f bPos(std::round(start.x), std::round(start.y + moduleHeight - 95.f));
                bBtn.setPosition(bPos);
                bool bHover = bBtn.getGlobalBounds().contains(mousePos);

                bBtn.setFillColor(bHover ? sf::Color(243, 238, 225) : sf::Color::Black);
                bBtn.setOutlineColor(sf::Color(243, 238, 225));
                bBtn.setOutlineThickness(1.f);
                window.draw(bBtn);

                sf::Text bTxt(jetBrainsMono, "> INSTALL_ALL (" + std::to_string(affordable) + ")", 11);
                bTxt.setPosition( {std::round(bPos.x + 10.f), std::round(bPos.y + 7.f)} );
                bTxt.setFillColor(bHover ? sf::Color::Black : sf::Color(243, 238, 225));
                window.draw(bTxt);

                if (bHover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
                    for (auto& hf : hotfixList) {
                        if (!hf.written && bits >= hf.bits) {
                            bits -= hf.bits;
                            hf.written = 1;
                            hotfixMult += hf.bitMult;
                        }
                    }
                    playSFX("button");
                    cooldown.restart();
                }
            }
        });
    }

    else if (activeTab == Tab::STATS) {
        drawTerminalModule(window, "void://hardware/stats.bat" + getCursor(), tabProgress, [&](sf::Vector2f start) {
            const sf::Color cream(243, 238, 225);
            const sf::Color muted(100, 100, 100);
            const sf::Color accent(140, 255, 160);
            const sf::Color accentMal(255, 100, 100);

            float colW = (moduleWidth - 80.f) / 2.f;
            float colR = start.x + colW + 40.f;
            float rowH = 18.f;

            auto drawSectionHeader = [&](sf::Vector2f pos, float width, const std::string& label) {
                sf::Text hdr(jetBrainsMono, "[ " + label + " ]", 11);
                hdr.setPosition(pos);
                hdr.setFillColor(muted);
                window.draw(hdr);

                float lineX = pos.x + hdr.getLocalBounds().size.x + 8.f;
                sf::RectangleShape rule({ width - hdr.getLocalBounds().size.x - 8.f, 1.f });
                rule.setPosition({ lineX, pos.y + 8.f });
                rule.setFillColor(sf::Color(50, 50, 50));
                window.draw(rule);
            };

            auto drawStat = [&](sf::Vector2f pos, float colWidth, const std::string& key, const std::string& val, sf::Color valColor = sf::Color(243, 238, 225)) {
                sf::Text k(jetBrainsMono, key, 12);
                k.setPosition(pos);
                k.setFillColor(muted);
                window.draw(k);

                sf::Text v(jetBrainsMono, val, 12);
                v.setPosition({ pos.x + colWidth - v.getLocalBounds().size.x - 4.f, pos.y });
                v.setFillColor(valColor);
                window.draw(v);

                sf::RectangleShape sep({ colWidth, 1.f });
                sep.setPosition({ pos.x, pos.y + 15.f });
                sep.setFillColor(sf::Color(30, 30, 30));
                window.draw(sep);
            };

            auto drawBar = [&](sf::Vector2f pos, float width, float ratio, sf::Color fillColor) {
                sf::RectangleShape back({ width, 4.f });
                back.setPosition(pos);
                back.setFillColor(sf::Color(30, 30, 30));
                window.draw(back);

                float filled = std::clamp(ratio, 0.f, 1.f) * width;
                if (filled > 0.f) {
                    sf::RectangleShape fill({ filled, 4.f });
                    fill.setPosition(pos);
                    fill.setFillColor(fillColor);
                    window.draw(fill);
                }
            };

            sf::Vector2f lPos = start;

            drawSectionHeader(lPos, colW, "ACCUMULATION");
            lPos.y += 22.f;

            drawStat(lPos, colW, "> all_bits.tmp",        format(allBits) + " b",   accent);
            lPos.y += rowH;
            drawStat(lPos, colW, "> all_malbits.tmp",     format(allMalbits) + " b", accentMal);
            lPos.y += rowH;
            drawStat(lPos, colW, "> clicked_bits.tmp",    format(allClickedBits) + " b", cream);
            lPos.y += rowH;
            drawStat(lPos, colW, "> all_bytes.tmp",       format(allBytes) + " B",   cream);
            lPos.y += rowH;
            drawStat(lPos, colW, "> all_malbytes.tmp",    format(allMalbytes) + " B", cream);
            lPos.y += rowH + 10.f;

            drawSectionHeader(lPos, colW, "HOTFIX COVERAGE");
            lPos.y += 20.f;

            int totalHF = hotfixList.size();
            int writtenHF = 0;
            for (const auto& hf : hotfixList) if (hf.written) writtenHF++;
            float hfRatio = totalHF > 0 ? writtenHF / totalHF : 0.f;

            drawBar({ lPos.x, lPos.y }, colW, hfRatio, sf::Color(243, 238, 225, 160));
            lPos.y += 10.f;
            sf::Text hfTxt(jetBrainsMono, std::to_string(writtenHF) + " / " + std::to_string(totalHF) + " hotfixes installed", 10);
            hfTxt.setPosition(lPos);
            hfTxt.setFillColor(muted);
            window.draw(hfTxt);
            lPos.y += rowH + 10.f;

            sf::Vector2f rPos = { colR, start.y };

            drawSectionHeader(rPos, colW, "SYSTEM HISTORY");
            rPos.y += 22.f;

            drawStat(rPos, colW, "> times_init.tmp",     std::to_string(timesInitialised), cream);
            rPos.y += rowH;
            drawStat(rPos, colW, "> times_crpt.tmp",     std::to_string(timesCorrupted), accentMal);
            rPos.y += rowH;
            drawStat(rPos, colW, "> hotfix_mult.tmp",    "x" + format(hotfixMult), cream);
            rPos.y += rowH + 10.f;

            drawSectionHeader(rPos, colW, "REINIT THRESHOLD");
            rPos.y += 20.f;
            float reinitRatio = (float)std::min(1.0L, allBits / 5000000.0L);
            drawBar({ rPos.x, rPos.y }, colW, reinitRatio, sf::Color(180, 180, 255, 160));
            rPos.y += 10.f;
            sf::Text reinitTxt(jetBrainsMono,
                allBits >= 5000000.0L ? "THRESHOLD REACHED" : format(allBits) + " / 5,000,000 b",
                10);
            reinitTxt.setPosition(rPos);
            reinitTxt.setFillColor(allBits >= 5000000.0L ? sf::Color(180, 180, 255) : muted);
            window.draw(reinitTxt);
            rPos.y += rowH + 10.f;

            drawSectionHeader(rPos, colW, "LOG ACCESS");
            rPos.y += 20.f;
            float logsRatio = (float)std::min(1.0L, allBits / 1e45L);
            drawBar({ rPos.x, rPos.y }, colW, logsRatio, sf::Color(255, 200, 80, 140));
            rPos.y += 10.f;
            sf::Text logsTxt(jetBrainsMono,
                allBits >= 1e45L ? "OMEGA-LEVEL UNLOCKED" : "LOCKED — ACCUMULATE MORE",
                10);
            logsTxt.setPosition(rPos);
            logsTxt.setFillColor(allBits >= 1e45L ? sf::Color(255, 200, 80) : muted);
            window.draw(logsTxt);
            rPos.y += rowH + 10.f;

            drawSectionHeader(rPos, colW, "CORRUPTION ACCESS");
            rPos.y += 20.f;
            float corruptRatio = (float)std::min(1.0L, allBits / 1e63L);
            drawBar({ rPos.x, rPos.y }, colW, corruptRatio, sf::Color(255, 80, 80, 120));
            rPos.y += 10.f;
            sf::Text corruptTxt(jetBrainsMono,
                allBits >= 1e63L ? "NOT_SUS.BAT AVAILABLE" : "LOCKED — ACCUMULATE MORE",
                10);
            corruptTxt.setPosition(rPos);
            corruptTxt.setFillColor(allBits >= 1e63L ? sf::Color(255, 80, 80) : muted);
            window.draw(corruptTxt);

            float footerY = start.y + moduleHeight - 70.f;
            sf::RectangleShape footerLine({ moduleWidth - 40.f, 1.f });
            footerLine.setPosition({ start.x, footerY });
            footerLine.setFillColor(sf::Color(40, 40, 40));
            window.draw(footerLine);

            sf::Text ver(jetBrainsMono, voidVersion + "  //  PROC: stats.bat  //  STATUS: NOMINAL", 10);
            ver.setPosition({ start.x, footerY + 8.f });
            ver.setFillColor(sf::Color(60, 60, 60));
            window.draw(ver);
        });
    }

    else if (activeTab == Tab::LOGS) {
        drawTerminalModule(window, "void://restricted/logs.bat" + getCursor(), tabProgress, [&](sf::Vector2f start) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (!logsUnlocked) {
                sf::Text t(jetBrainsMono, "OMEGA-LEVEL ACCESS CODE REQUIRED:\n> " + logInput + getCursor(), 56);
				t.setOrigin({ t.getGlobalBounds().size.x / 2.f, 0.f });
                t.setPosition(start + sf::Vector2f(moduleWidth / 2.f, moduleHeight * 0.35f));
                t.setFillColor(sf::Color(243, 238, 225));
                window.draw(t);

                if (logInput == "050126") {
                    logsUnlocked = true;
                    logInput = "";
                }
            }
            else {
                float sidebarWidth = 200.f;

                for (size_t i = 0; i < loreEntries.size(); ++i) {
                    sf::Text logTitle(jetBrainsMono, "> " + loreEntries[i].title, 13);
                    logTitle.setPosition(start + sf::Vector2f(-5.f, 20.f + (i * 25.f)));

                    bool isHovered = sf::FloatRect(logTitle.getPosition(), { sidebarWidth, 20.f }).contains(mousePos);

                    if (selectedLog == i) logTitle.setFillColor(sf::Color(243, 238, 225));
                    else if (isHovered) logTitle.setFillColor(sf::Color::White);
                    else logTitle.setFillColor(sf::Color(100, 100, 100));

                    if (isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && cooldown.getElapsedTime().asMilliseconds() > 200) {
                        if (selectedLog != i) playSFX("button");
                        selectedLog = i;
                    }
                    window.draw(logTitle);
                }

                sf::RectangleShape line({ 1.f, moduleHeight - 100.f });
                line.setPosition(start + sf::Vector2f(sidebarWidth, 10.f));
                line.setFillColor(sf::Color(50, 50, 50));
                window.draw(line);

                auto& entry = loreEntries[selectedLog];

                sf::Text h(jetBrainsMono, entry.title + " - " + entry.cycle, 14);
                h.setPosition(start + sf::Vector2f(sidebarWidth + 20.f, 20.f));
                h.setFillColor(sf::Color(243, 238, 225));
                window.draw(h);

                sf::Text b(jetBrainsMono, entry.content, 12);
                b.setPosition(start + sf::Vector2f(sidebarWidth + 20.f, 40.f));
                b.setFillColor(sf::Color(180, 180, 180));
                window.draw(b);
            }
            });
    }
}