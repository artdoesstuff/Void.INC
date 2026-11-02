#pragma once

#include "../../Misc/Globals/GIncludes.hpp"

struct Star {
    sf::VertexArray star;
};

inline sf::Color lerpColour(sf::Color a, sf::Color b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return sf::Color(
        static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<std::uint8_t>(a.b + (b.b - a.b) * t)
    );
}

inline void updateStar(Star& star, sf::Vector2f centre, float time, float currentScale, long double allBits) {
    constexpr int numArms = 16;
    constexpr size_t segmentsPerArm = 50;
    constexpr size_t verticesPerArm = segmentsPerArm * 2;
    constexpr size_t totalVertices = verticesPerArm * numArms;

    if (star.star.getVertexCount() != totalVertices) {
        star.star.resize(totalVertices);
        star.star.setPrimitiveType(sf::PrimitiveType::Lines);
    }

    float logVal = (allBits > 1.0L) ? static_cast<float>(std::log10(allBits)) : 0.f;
    float rotationSpeed = 0.4f + (logVal * 0.02f);

    sf::Color starColour;
    if (logVal <= 9.f)  starColour = sf::Color(255, 0, 0);
    else if (logVal <= 18.f) starColour = lerpColour(sf::Color(255, 0, 0),   sf::Color(255, 255, 0),   (logVal - 9.f)  / 9.f);
    else if (logVal <= 45.f) starColour = lerpColour(sf::Color(255, 255, 0), sf::Color(255, 255, 255), (logVal - 18.f) / 27.f);
    else starColour = lerpColour(sf::Color(255, 255, 255), sf::Color(0, 100, 255), (logVal - 45.f) / 54.f);

    for (int arm = 0; arm < numArms; ++arm) {
        float armOffset = (arm * 2.f * pi) / numArms;

        for (size_t s = 0; s < segmentsPerArm; ++s) {
            size_t idx = (arm * verticesPerArm) + (s * 2);

            for (int j = 0; j < 2; ++j) {
                float ratio = (s + j) / static_cast<float>(segmentsPerArm);
                float angle = (ratio * 5.f) - (time * rotationSpeed) + armOffset;
                float radius = ratio * 350.f * currentScale;

                star.star[idx + j].position = {
                    centre.x + (radius * std::cos(angle)),
                    centre.y + (radius * std::sin(angle))
                };

                float alphaFade = std::pow(std::sin(ratio * pi), 1.2f);
                star.star[idx + j].color = starColour;
                star.star[idx + j].color.a = static_cast<std::uint8_t>(160 * alphaFade);
            }
        }
    }
}