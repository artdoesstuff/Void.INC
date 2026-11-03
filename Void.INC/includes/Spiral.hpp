#pragma once

/**
 * @file Spiral.hpp
 * @brief Void.INC — the spiral renderer.
 *
 * 16-arm logarithmic spiral drawn as line segments.
 * Colour is a single fixed white that fades to transparent toward the arms'
 * outer tips.  Speed and scale are driven by allDataProduced (total Data ever
 * produced), giving the spiral a sense of weight as the run progresses.
 *
 * Usage:
 *   Spiral spiral;
 *   // each frame:
 *   updateSpiral(spiral, centre, elapsed, scale, allDataProduced);
 *   window.draw(spiral.spiral);
 */

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <algorithm>

inline constexpr float pi = 3.14159265358979323846f;

struct Spiral {
    sf::VertexArray spiral;
};

/**
 * Rebuild the spiral geometry for this frame.
 *
 * @param spiral        The Spiral to update (vertex array is resized on first call).
 * @param centre        Screen-space centre of the spiral.
 * @param time          Monotonically increasing elapsed time (seconds).
 * @param currentScale  Uniform scale applied to the arm radius [0.5, 1.0 typical].
 * @param allDataProduced       Total Data ever produced — drives rotation speed.
 */
inline void updateSpiral(Spiral& spiral,
                       sf::Vector2f centre,
                       float        time,
                       float        currentScale,
                       long double  allDataProduced,
                       sf::Color    tint = sf::Color(255, 255, 255))
{
    constexpr int    numArms        = 16;
    constexpr size_t segmentsPerArm = 50;
    constexpr size_t verticesPerArm = segmentsPerArm * 2;
    constexpr size_t totalVertices  = verticesPerArm * numArms;

    if (spiral.spiral.getVertexCount() != totalVertices) {
        spiral.spiral.resize(totalVertices);
        spiral.spiral.setPrimitiveType(sf::PrimitiveType::Lines);
    }

    // Rotation speed scales softly with log10(allDataProduced)
    float logVal       = (allDataProduced > 1.0L) ? static_cast<float>(std::log10(allDataProduced)) : 0.f;
    float rotationSpeed = 0.4f + logVal * 0.02f;

    // Single spiral colour — white by default, or a Merchant cosmetic tint
    const sf::Color spiralColour = tint;

    for (int arm = 0; arm < numArms; ++arm) {
        float armOffset = (arm * 2.f * pi) / static_cast<float>(numArms);

        for (size_t s = 0; s < segmentsPerArm; ++s) {
            size_t idx = arm * verticesPerArm + s * 2;

            for (int j = 0; j < 2; ++j) {
                float ratio  = (static_cast<float>(s) + static_cast<float>(j))
                               / static_cast<float>(segmentsPerArm);
                float angle  = ratio * 5.f - time * rotationSpeed + armOffset;
                float radius = ratio * 350.f * currentScale;

                spiral.spiral[idx + j].position = {
                    centre.x + radius * std::cos(angle),
                    centre.y + radius * std::sin(angle)
                };

                // Fade out toward the tip; bright near the core
                float alpha = std::pow(std::sin(ratio * pi), 1.2f);
                spiral.spiral[idx + j].color   = spiralColour;
                spiral.spiral[idx + j].color.a = static_cast<std::uint8_t>(160.f * alpha);
            }
        }
    }
}