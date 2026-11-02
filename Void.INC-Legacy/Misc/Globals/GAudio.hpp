#pragma once

#include "GIncludes.hpp"
#include "GVariables.hpp"

inline std::map<std::string, sf::SoundBuffer> soundBuffers;
inline std::map<std::string, sf::Sound> sounds;
inline std::map<std::string, std::unique_ptr<sf::Music>> music;
inline std::map<std::string, std::unique_ptr<sf::Music>> ambience;

float bVolSFX = 30.f;
float bVolMusic = 30.f;

inline void initAudio() {
    std::vector<std::pair<std::string, std::string>> sfxToLoad = {
        {"button", "Assets/Audio/SFX/button_clicks.wav"},
        {"heartbeat", "Assets/Audio/SFX/heartbeat.wav"},
        {"installed", "Assets/Audio/SFX/logic_hotfix_purchase.wav"},
        {"patched", "Assets/Audio/SFX/patch_purchase.wav"}
    };

    std::vector<std::pair<std::string, std::string>> musicToLoad = {
        {"menu", "Assets/Audio/Music/menu.ogg"},
        {"main", "Assets/Audio/Music/main.ogg"},
        {"directory", "Assets/Audio/Music/directory.ogg"},
        {"kernel", "Assets/Audio/Music/kernel.ogg"},
        // On a technicality, just for loading screens
        {"loading", "Assets/Audio/Ambience/white_noise.ogg"}
    };

    std::vector<std::pair<std::string, std::string>> ambienceToLoad = {
        {"ambience_0", "Assets/Audio/Ambience/ambience_0.ogg"},
        {"ambience_1", "Assets/Audio/Ambience/ambience_1.ogg"},
        {"ambience_2", "Assets/Audio/Ambience/ambience_2.ogg"},
        {"ambience_3", "Assets/Audio/Ambience/ambience_3.ogg"},
        {"ambience_4", "Assets/Audio/Ambience/ambience_4.ogg"},
        {"ambience_5", "Assets/Audio/Ambience/ambience_5.ogg"},
        {"dread_0", "Assets/Audio/Ambience/dread_0.ogg"},
        {"dread_1", "Assets/Audio/Ambience/dread_1.ogg"},
        {"heartbeat", "Assets/Audio/Ambience/heartbeat.ogg"},
        {"screams", "Assets/Audio/Ambience/screams.ogg"}
    };

    for (const auto& [name, path] : sfxToLoad) {
        if (soundBuffers[name].loadFromFile(path)) {
            sounds.emplace(name, sf::Sound(soundBuffers[name]));
        }
    }

    for (const auto& [name, path] : musicToLoad) {
        auto track = std::make_unique<sf::Music>();
        if (track->openFromFile(path)) {
            track->setLooping(true);
            track->setVolume(bVolMusic);
            music[name] = std::move(track);
        }
    }

    for (const auto& [name, path] : ambienceToLoad) {
        auto track = std::make_unique<sf::Music>();
        if (track->openFromFile(path)) {
            track->setVolume(bVolMusic);
            ambience[name] = std::move(track);
        }
    }
}

inline void ambienceRandom() {
    if (ambienceClock.getElapsedTime().asSeconds() >= 60.f) {

        if (!ambience.empty()) {
            static std::mt19937 rand(std::random_device{}());
            std::uniform_int_distribution<size_t> dist(0, ambience.size() - 1);

            auto it = ambience.begin();
            std::advance(it, dist(rand));

            it->second->play();
        }

        ambienceClock.restart();
    }
}

inline void updateVolume() {
    float sfxVol = (muteAll || muteSFX) ? 0.f : bVolSFX;
    float musicVol = (muteAll || muteAmbience) ? 0.f : bVolMusic;

    for (auto& [name, sound] : sounds) {
        sound.setVolume(sfxVol);
    }

    for (auto& [name, track] : music) {
        track->setVolume(musicVol);
    }

    for (auto& [name, track] : ambience) {
        track->setVolume(musicVol);
    }
}

inline void playSFX(const std::string& name) {
    if (sounds.find(name) != sounds.end()) {
        sounds.at(name).play();
    }
}

inline void playMusic(const std::string& name) {
    if (music.count(name)) {
        if (music[name]->getStatus() == sf::SoundSource::Status::Playing) {
            return;
        }

        for (auto& [key, track] : music) {
            track->stop();
        }

        music[name]->play();
    }
}