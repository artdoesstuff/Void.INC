#include "Misc/Globals/GAudio.hpp"
#include "Misc/Globals/GFunctions.hpp"
#include "Misc/Globals/GIncludes.hpp"
#include "Misc/Globals/GVariables.hpp"

#include "Bits/Format.hpp"
#include "Bits/Offline.hpp"
#include "Bits/Update.hpp"

#include "Corruption/Corruption.hpp"

#include "Effects/Bits.hpp"

#include "Hardware/Hotfixes.hpp"
#include "Hardware/Logic.hpp"

#include "Initialisation/Initialisation.hpp"
#include "Initialisation/Re-initialisation.hpp"

#include "UI/Core/Directory.hpp"
#include "UI/Core/Download.hpp"
#include "UI/Core/Terminal.hpp"
#include "UI/Core/Kernel.hpp"
#include "UI/Extra/Loading.hpp"
#include "UI/Extra/Offline.hpp"
#include "UI/Extra/Options.hpp"
#include "UI/Extra/Star.hpp"
#include "UI/Extra/Start.hpp"

#include "UserData/Local/Loading.hpp"
#include "UserData/Local/Options.hpp"
#include "UserData/Local/Saving.hpp"
#include "UserData/Local/Version.hpp"

int main() {
	if (std::filesystem::exists("updater.exe")) std::system("updater.exe");
	if (!std::filesystem::exists("updater.exe")) std::cout << "updater.exe not detected, skipping..." << std::endl;

	sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Void.INC | " + voidVersion, sf::State::Fullscreen);
	window.setFramerateLimit(60);
	window.setIcon(sf::Image("icon.png"));

	sf::Vector2f centre = { window.getSize().x / 2.f, window.getSize().y / 2.f };
	sf::Vector2f sPos = { window.getSize().x * 0.7f, window.getSize().y / 2.f };

	time_t timeEnd = 0;

	sf::RectangleShape clickRect;
	clickRect.setSize(sf::Vector2f(256, 256));
	clickRect.setOrigin(sf::Vector2f(clickRect.getSize().x / 2, clickRect.getSize().y / 2));
	clickRect.setPosition(centre);
	clickRect.setOutlineColor(sf::Color::Black);
	clickRect.setOutlineThickness(5);

	Star star;
	star.star = sf::VertexArray(sf::PrimitiveType::LineStrip, 4000);
	float starScale = 1.f;

	Cog cog;

	sf::VertexArray lines(sf::PrimitiveType::Lines);
	for (int i = 0; i < window.getSize().y; i += 4) {
		lines.append(sf::Vertex{ sf::Vector2f(0.f, static_cast<float>(i)), sf::Color(255, 255, 255, 40) });
		lines.append(sf::Vertex{ sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(i)), sf::Color(255, 255, 255, 40)});
	}

	sf::Text sessionText(jetBrainsMono, "", 14);
	sessionText.setFillColor(sf::Color(100, 100, 100, 150));

	sf::Text bitsText(jetBrainsMono);
	bitsText.setCharacterSize(36);
	bitsText.setFillColor(sf::Color(243,238,225));

	sf::Text bitsTextRed(jetBrainsMono);
	bitsTextRed.setCharacterSize(36);
	bitsTextRed.setFillColor(sf::Color(255, 0, 0, 100));

	sf::Text bitsTextCyan(jetBrainsMono);
	bitsTextCyan.setCharacterSize(36);
	bitsTextCyan.setFillColor(sf::Color(0, 255, 255, 100));

	sf::Text bitsPerSecondText(jetBrainsMono);
	bitsPerSecondText.setCharacterSize(20);
	bitsPerSecondText.setFillColor(sf::Color(243, 238, 225));

	sf::Text bytesText(jetBrainsMono);
	bytesText.setCharacterSize(36);
	bytesText.setFillColor(sf::Color(0, 255, 150));

	sf::Text malbitsText(jetBrainsMono);
	malbitsText.setCharacterSize(20);
	malbitsText.setFillColor(sf::Color(255, 50, 0));

	sf::Text malbytesText(jetBrainsMono);
	malbytesText.setCharacterSize(36);
	malbytesText.setFillColor(sf::Color(255, 50, 50));

	initLogicGates();
	initHotfixes();
	initDirTree();
	initKernelTree();
	initAudio();
	positionTreeNodes(window.getSize());
	positionKernelNodes(window.getSize());
	load(timeEnd, bits, bytes, allBits, allClickedBits, allBytes, bitsPerSecond, hotfixMult, timesInitialised, malbits, malbytes, allMalbits, allMalbytes, timesCorrupted, currentCorruption, logicGateList, hotfixList, dirTree, kernelTree);
	loadOptions(renderEffects, quickStart, muteAll, muteSFX, muteAmbience);
	offline(timeEnd, bits, allBits, bitsPerSecond, hotfixMult);
	
	while (window.isOpen()) {
		float deltaTime = deltaClock.restart().asSeconds();
		float elapsedTime = elapsedClock.getElapsedTime().asSeconds();

		updateBits(deltaTime);

		starScale += (1.f - starScale) * 0.1f;
		updateStar(star, centre, elapsedTime, starScale, allBits);

		bitsText.setString("-" + format(bits) + " Bits");
		sf::Vector2f bitPos(clickRect.getPosition().x, clickRect.getPosition().y + 400);
		centreText(bitsText, bitPos);

		bitsPerSecondText.setString("-" + format(realBitsPerSecond, true) + " Bits per Second");
		centreText(bitsPerSecondText, { clickRect.getPosition().x, clickRect.getPosition().y + 440 });

		bytesText.setString("-" + format(bytes, true) + " Bytes");
		centreText(bytesText, { clickRect.getPosition().x, clickRect.getPosition().y + 400 });

		if (allBits >= 1e30) {
			malbitsText.setString("-" + format(malbits) + " Malbits");
			centreText(malbitsText, { clickRect.getPosition().x, clickRect.getPosition().y + 470 });
		}

		malbytesText.setString("-" + format(malbytes, true) + " Malbytes");
		centreText(malbytesText, { clickRect.getPosition().x, clickRect.getPosition().y + 400 });

		bool isCorrupted = std::any_of(kernelTree.begin(), kernelTree.end(), [](const auto& n) { return n.overwritten; });

		sf::RenderStates states;
		states.blendMode = sf::BlendAdd;

		updateVolume();

		window.clear(sf::Color::Black);

		sf::Time sessionTime = sessionClock.getElapsedTime();
		int seconds = static_cast<int>(sessionTime.asSeconds());
		int hh = seconds / 3600;
		int mm = (seconds % 3600) / 60;
		int ss = seconds % 60;

		char timerBuf[16];
		sprintf(timerBuf, "[ %02d:%02d:%02d ]", hh, mm, ss);

		sessionText.setString(timerBuf);

		sessionText.setPosition({ window.getSize().x - 120.f, window.getSize().y - 30.f });
		window.draw(sessionText);

		if (showStart && !quickStart) {
			playMusic("menu");
			drawStartUI(window, states, star, elapsedTime, deltaTime, sPos, centre);
		}

		if (start && !quickStart) {
			if (currentStartStep == StartState::IDLE) currentStartStep = StartState::TRANSITION;
			timer += deltaTime;
			canClickStart = false;

			switch (currentStartStep) {
			case StartState::TRANSITION: {
				float t = std::min(1.f, timer / 1.0f);
				float ease = t * t * (0.5f * t);
				sPos = sPos + (centre - sPos) * ease;

				if (timer >= 1.0f) {
					sPos = centre;
					showStart = false;
					showOffline = true;
					start = false;
					canClick = true;
					timer = 0.0f;
					currentStartStep = StartState::IDLE;
				}
				break;
			}
			}
		}

		if (!showStart || quickStart) {
			ambienceRandom();
			if (quickStart) {
				showStart = false;
				canClick = true;
			}
			if (!reinitialisation && !initialisation && !corrupting) {
				playMusic("main");
				if (isCorrupted && renderEffects) {
					float offset = 2.0f + std::sin(elapsedTime * 10.f) * 2.0f;
					if (std::rand() % 20 == 0) offset *= 3.0f;
					centreText(bitsTextRed, bitPos - sf::Vector2f(offset, 0.f));
					centreText(bitsTextCyan, bitPos + sf::Vector2f(offset, 0.f));
					window.draw(bitsTextRed);
					window.draw(bitsTextCyan);
				}
				window.draw(bitsText);
				window.draw(bitsPerSecondText);
				window.draw(malbitsText);

				if (renderEffects) {
					updateStream(window, centre, deltaTime, 2);
					for (auto& d : dataStream) {
						window.draw(d.bit);
					}
				}

				updateLogicGateUI(window, allBits);

				window.draw(star.star, states);

				drawTerminalUI(window, bits, allBits, deltaTime);

				if (showOffline) drawOfflineUI(window, centre);
				if (showReinitialisationPopup) drawReinitialisationPopup(window, reinitialisation, centre);
			}
		}

		if (reinitialisation) {
			if (currentReinitStep == ReinitState::IDLE) {
				currentReinitStep = ReinitState::VORTEX_EXPANSION;
				playMusic("loading");
				canClick = false;
			}
			timer += deltaTime;
			sf::View shakeView = window.getDefaultView();

			switch (currentReinitStep) {
				case ReinitState::VORTEX_EXPANSION: {
					starScale += (20.f - starScale) * 0.05f;

					float intensity = (starScale / 20.f) * 15.f;
					float offsetX = (std::rand() % 100 - 50) / 50.f * intensity;
					float offsetY = (std::rand() % 100 - 50) / 50.f * intensity;
					shakeView.move({ offsetX, offsetY });

					window.setView(shakeView);
					window.draw(star.star, states);

					if (timer >= 0.5f) currentReinitStep = ReinitState::VORTEX_SHRINK;
					break;
				}

				case ReinitState::VORTEX_SHRINK: {
					starScale += (0.f - starScale) * 0.15f;
					shakeView.move({ (std::rand() % 10 - 5) / 2.f, (std::rand() % 10 - 5) / 2.f });

					window.setView(shakeView);
					window.draw(star.star, states);

					if (timer >= 0.7f) {
						starScale = -10.f;
						currentReinitStep = ReinitState::LOADING_BAR;
					}
					break;
				}

				case ReinitState::LOADING_BAR:
					loadingProgress = (timer - 0.7f) / 5.0f;
					drawLoadingUI(window, loadingProgress);

					if (loadingProgress >= 1.0f) {
						loadingProgress = 1.0f;
						currentReinitStep = ReinitState::DIR;
						playMusic("directory");
					}
					break;

				case ReinitState::DIR:
					window.setView(window.getDefaultView());
					loadingProgress = 0.0f;
					timer = 0.0f;
					canClickInit = true;

					ambienceRandom();

					if (renderEffects) {
						updateStream(window, centre, deltaTime, 1);
						for (auto& d : dataStream) {
							window.draw(d.bit);
						}
					}

					drawTreeLines(window);
					drawDirTreeUI(window);

					drawTerminalUI(window, bits, allBits, deltaTime);

					if (showCorruptPopup) drawCorruptPopup(window, corrupting, centre);

					window.draw(bytesText);
					break;
			}
		}

		if (initialisation) {
			if (currentInitStep == InitState::IDLE) {
				currentInitStep = InitState::LOADING_BAR;
				currentCorruptStep = CorruptState::IDLE;
				playMusic("loading");

				loadingProgress = 0.0f;
				timer = 0.0f;

				corrupting = false;
				canClickCorrupt = false;
			}
			timer += deltaTime;
			sf::View shakeView = window.getDefaultView();

			switch (currentInitStep) {
				case InitState::LOADING_BAR: {
					loadingProgress = timer / 3.0f;
					drawLoadingUI(window, loadingProgress);

					if (loadingProgress >= 1.0f) {
						loadingProgress = 1.0f;
						currentInitStep = InitState::VORTEX_EXPANSION;
						starScale = 0.1f;
					}
					break;
				}

				case InitState::VORTEX_EXPANSION: {
					starScale += (1.f - starScale) * 0.1f;

					float intensity = (1.f - starScale) * 20.f;
					float offsetX = (std::rand() % 100 - 50) / 50.f * intensity;
					float offsetY = (std::rand() % 100 - 50) / 50.f * intensity;
					shakeView.move({ offsetX, offsetY });
					window.setView(shakeView);

					window.draw(star.star, states);

					if (timer >= 3.5f) {
						window.setView(window.getDefaultView());
						loadingProgress = 0.0f;
						timer = 0.0f;
						canClick = true;

						starScale = 1.f;

						currentInitStep = InitState::IDLE;

						initialisation = false;
					}
					break;
				}
			}
		}

		if (corrupting) {
			if (currentCorruptStep == CorruptState::IDLE) {
				currentCorruptStep = CorruptState::CORRUPT;
				playMusic("loading");
			}
			timer += deltaTime;

			switch (currentCorruptStep) {
				case CorruptState::CORRUPT:
					if (!snapshotTaken) {
						screenSnapshot.resize(window.getSize());
						screenSnapshot.update(window);
						snapshotTaken = true;
					}
					{
						int slices = 30;
						float sliceH = static_cast<float>(window.getSize().y) / slices;

						for (int i = 0; i < slices; ++i) {
							sf::Sprite slice(screenSnapshot);
							sf::IntRect area({0, static_cast<int>(i * sliceH)}, {static_cast<int>(window.getSize().x), static_cast<int>(sliceH)});
							slice.setTextureRect(area);

							float offset = (std::rand() % 100 - 50) * (timer / 2.0f);
							slice.setPosition({ offset, i * sliceH });

							if (i % 4 == 0) slice.setColor(sf::Color(255, 100, 100, 180));
							if (i % 7 == 0) slice.setColor(sf::Color(100, 255, 255, 180));

							window.draw(slice);
						}
					}

					if (timer > 2.0f) {
						currentCorruptStep = CorruptState::LOADING_BAR;
						snapshotTaken = false;
					}
					break;

				case CorruptState::LOADING_BAR:
					loadingProgress = timer / 5.0f;
					drawLoadingUI(window, loadingProgress);

					if (loadingProgress >= 1.0f) {
						loadingProgress = 1.0f;
						currentCorruptStep = CorruptState::KERNEL;
						playMusic("kernel");
					}
					break;

				case CorruptState::KERNEL:
					drawTerminalUI(window, bits, allBits, deltaTime);

					drawKernelLines(window);
					drawKernelUI(window);

					window.draw(malbytesText);
					break;
			}
		}
		
		if (!showStart) {
			updateCog(cog, window, deltaTime, showOptions);
			window.draw(cog.cog);
		}
		if (showOptions) drawOptionsUI(window, showOptions, centre);

		window.draw(lines);

		if (isCorrupted && renderEffects && (std::rand() % 500 == 0)) {
			sf::View glitchView = window.getDefaultView();

			float intensity = 5.0f;
			glitchView.move({ (std::rand() % 10 - 5.f) * intensity, (std::rand() % 4 - 2.f) * intensity });

			sf::RectangleShape glitchOverlay(sf::Vector2f(window.getSize()));
			glitchOverlay.setFillColor(sf::Color(255, 0, 0, 40));

			window.setView(glitchView);
			window.draw(glitchOverlay);
		}
		else window.setView(window.getDefaultView());

		window.display();

		while (const std::optional gameEvent = window.pollEvent()) {
			sf::Vector2f winSize = (sf::Vector2f)window.getSize();
			if (gameEvent->is<sf::Event::Closed>()) {
				save(time(nullptr), bits, bytes, allBits, allClickedBits, allBytes, bitsPerSecond, hotfixMult, timesInitialised, malbits, malbytes, allMalbits, allMalbytes, timesCorrupted, currentCorruption, logicGateList, hotfixList, dirTree, kernelTree);
				saveOptions(renderEffects, quickStart, muteAll, muteSFX, muteAmbience);
				versionSave(voidVersion);
				window.close();
			}

			if (gameEvent->is<sf::Event::TextEntered>()) {
				const auto& textEvent = gameEvent->getIf<sf::Event::TextEntered>();

				if (activeTab == Tab::LOGS && !logsUnlocked) {
					if (textEvent->unicode == '\b') {
						if (!logInput.empty()) logInput.pop_back();
						playSFX("button");
					}
					else if (textEvent->unicode < 128 && textEvent->unicode != '\r' && textEvent->unicode != '\n') {
						logInput += static_cast<char>(textEvent->unicode);
						playSFX("button");
					}
				}
			}

			if (gameEvent->is<sf::Event::MouseButtonPressed>()) {
				const auto& mouseEvent = gameEvent->getIf<sf::Event::MouseButtonPressed>();
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

				if (mouseEvent->button == sf::Mouse::Button::Left && canClick) {
					if (clickRect.getGlobalBounds().contains(mousePos)) {
						long double bitsClicked = bitsPerClick * clickMultiplier * (1 + realBitsPerSecond * 0.05f);
						starScale = 1.1f; bits += bitsClicked; allBits += bitsClicked; allClickedBits += bitsClicked;
						playSFX("heartbeat");
					}

					if (mousePos.y > winSize.y - 60.f) {
						if (sf::FloatRect({ 10, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::LOGIC) ? Tab::NONE : Tab::LOGIC; playSFX("button");
						}
						else if (sf::FloatRect({ 200, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::HOTFIX) ? Tab::NONE : Tab::HOTFIX; playSFX("button");
						}
						else if (sf::FloatRect({ 390, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::STATS) ? Tab::NONE : Tab::STATS; playSFX("button");
						}
						else if (sf::FloatRect({ 580, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::REINIT) ? Tab::NONE : Tab::REINIT; playSFX("button");
						}
						else if (sf::FloatRect({ 770, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::LOGS) ? Tab::NONE : Tab::LOGS; playSFX("button");
						}
					}

					if (activeTab == Tab::LOGIC) {
						for (auto& lg : logicGateList) {
							if (lg.costType == currentLogicMode && lg.rect.getGlobalBounds().contains(mousePos)) {
								long double& activeCurrency = (lg.costType == Currency::BIT) ? bits : malbits;
								auto [totalCost, amount] = getDownload(lg, activeCurrency, currentBuy);
								if (activeCurrency >= totalCost && amount > 0) {
									activeCurrency -= totalCost;
									lg.ver += amount;
									bitsPerSecond += (lg.bps * amount);
									lg.currentCost = lg.baseCost * std::pow(logicGateInflation, lg.ver) * costMult;
									playSFX("installed");
								}
							}
						}
						showReinitialisationPopup = false;
					}
					else if (activeTab == Tab::HOTFIX) {
						size_t startIdx = hotfixPage * HF_PER_PAGE;
						size_t endIdx = std::min(startIdx + HF_PER_PAGE, hotfixList.size());
						for (size_t i = startIdx; i < endIdx; ++i) {
							auto& hf = hotfixList[i];
							if (hf.written == 0 && hf.rect.getGlobalBounds().contains(mousePos)) {
								if (bits >= hf.bits) {
									bits -= hf.bits;
									hf.written = 1;
									hotfixMult += hf.bitMult;
									playSFX("installed");
								}
							}
						}
						showReinitialisationPopup = false;
					}
					else if (activeTab == Tab::STATS) showReinitialisationPopup = false;
					else if (activeTab == Tab::REINIT) showReinitialisationPopup = true;
					else if (activeTab == Tab::LOGS) showReinitialisationPopup = false;
					else if (activeTab == Tab::NONE) showReinitialisationPopup = false;
				}

				if (mouseEvent->button == sf::Mouse::Button::Left && canClickInit) {
					if (mousePos.y > winSize.y - 60.f) {
						if (sf::FloatRect({ 10, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::INIT) ? Tab::NONE : Tab::INIT; playSFX("button");
						}
						if (sf::FloatRect({ 200, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::CORRUPT) ? Tab::NONE : Tab::CORRUPT; playSFX("button");
						}
					}
					
					if (activeTab == Tab::INIT) {
						currentReinitStep = ReinitState::IDLE;
						reinitialisation = false;
						initialisation = true;
						canClickInit = false;
						showCorruptPopup = false;
					}

					else if (activeTab == Tab::CORRUPT) {
						canClickInit = false;
						showCorruptPopup = true;
					}

					for (size_t i = 0; i < dirTree.size(); ++i) {
						auto& patch = dirTree[i];

						sf::Vector2f diff = mousePos - patch.pos;
						float distanceSquared = (diff.x * diff.x) + (diff.y * diff.y);

						if (patch.patched == 0 && !patch.disabled && distanceSquared <= (30.f * 30.f)) {
							if (bytes >= patch.bytes) {
								bytes -= patch.bytes;
								patch.patched = 1;
								playSFX("patched");

								if (patch.name == "Patch_0") bitsToBytesRate = 1e-6L;
								else if (patch.name == "Patch_3_1") bitsToBytesRate = 3e-7L;
								else if (patch.name == "Patch_6") bitsToBytesRate = 5e-7L;
							}
						}
					}
				}

				if (mouseEvent->button == sf::Mouse::Button::Left && canClickCorrupt) {
					if (mousePos.y > winSize.y - 60.f) {
						if (sf::FloatRect({ 10, winSize.y - 50.f }, { 180, 40 }).contains(mousePos)) {
							activeTab = (activeTab == Tab::REBOOT) ? Tab::NONE : Tab::REBOOT; playSFX("button");
						}
					}

					if (activeTab == Tab::REBOOT) {
						initialisation = true;
					}

					for (size_t i = 0; i < kernelTree.size(); ++i) {
						auto& overwrites = kernelTree[i];

						sf::Vector2f diff = mousePos - overwrites.pos;
						float distanceSquared = (diff.x * diff.x) + (diff.y * diff.y);

						if (overwrites.overwritten == 0 && distanceSquared <= (30.f * 30.f)) {
							if (malbytes >= overwrites.malbytes) {
								malbytes -= overwrites.malbytes;
								overwrites.overwritten = 1;
								playSFX("patched");
							}
						}
					}
				}
			}
		}
	}
}