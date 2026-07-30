/**
 * @file DayCycle.cpp
 * @brief Implements the DayCycle class, which manages in-game time progression and sky visual effects.
 *
 * This file defines the logic for advancing the day/night cycle, determining
 * the current phase of the day, generating readable phase names, interpolating
 * sky colors, and computing the on-screen positions of the sun and moon.
 *
 * It also provides a darkness value used to render the night overlay so that
 * the world gradually becomes darker in the evening and brighter near dawn.
 *
 * @author Group 46
 */
#include "DayCycle.h"
#include <cmath>
#include <algorithm>

/**
 * @brief Advances the in-game day cycle by the given time amount.
 *
 * The dayTime value is stored as a normalized value in the range [0, 1),
 * where 0 represents the start of the day and values closer to 1 represent
 * the end of the day. Once dayTime reaches or exceeds 1, it wraps back around
 * and the day count increases by one.
 *
 * @param dt The elapsed real time in seconds since the last frame update.
 */
void DayCycle::update(float dt) {
    dayTime += dt / DAY_DURATION;
    if (dayTime >= 1.f) {
        dayTime -= 1.f;
        ++dayCount;
    }
}

/**
 * @brief Skips the cycle forward to the start of the next morning.
 *
 * This is mainly used when the player sleeps. If the current phase is night,
 * the function also advances the day counter before resetting the time to
 * the morning start point.
 */
void DayCycle::skipToMorning() {
    if (isNight()) ++dayCount;
    dayTime = MORNING_START;
}

/**
 * @brief Determines the current phase of the day.
 *
 * The function compares the current normalized time value against the
 * configured phase boundaries to determine whether it is morning, noon,
 * evening, or night.
 *
 * @return The current TimeOfDay phase.
 */
TimeOfDay DayCycle::getPhase() const {
    if (dayTime < NOON_START)    return TimeOfDay::Morning;
    if (dayTime < EVENING_START) return TimeOfDay::Noon;
    if (dayTime < NIGHT_START)   return TimeOfDay::Evening;
    return TimeOfDay::Night;
}

/**
 * @brief Checks whether the current phase is night.
 *
 * @return True if the current time is within the night phase, or false otherwise.
 */
bool DayCycle::isNight() const { 
    return dayTime >= NIGHT_START; 
}

/**
 * @brief Checks whether the current phase is morning.
 *
 * @return True if the current time is within the morning phase, or false otherwise.
 */
bool DayCycle::isMorning() const { 
    return dayTime < NOON_START; 
}

/**
 * @brief Returns the current normalized time within the day cycle.
 *
 * The returned value is in the range [0, 1), where:
 * - 0.0 is the start of the day
 * - 0.5 is around the middle of the day
 * - values near 1.0 are close to the next day
 *
 * @return The current normalized day time.
 */
float DayCycle::getDayTime() const { 
    return dayTime; 
}

/**
 * @brief Returns the current in-game day count.
 *
 * @return The current day number.
 */
int DayCycle::getDayCount() const { 
    return dayCount; 
}

/**
 * @brief Sets the normalized day time directly.
 *
 * This function is mainly useful for loading game state or testing.
 *
 * @param t The new normalized day time value.
 */
void DayCycle::setDayTime(float t) { 
    dayTime = t; 
}

/**
 * @brief Sets the day count directly.
 *
 * This function is mainly useful for loading saved game data.
 *
 * @param c The new day count value.
 */
void DayCycle::setDayCount(int c) { 
    dayCount = c; 
}

/**
 * @brief Returns the current phase name as a readable string.
 *
 * This is useful for displaying the current phase in the user interface.
 *
 * @return A string representing the current phase of the day.
 */
std::string DayCycle::getPhaseName() const {
    switch (getPhase()) {
        case TimeOfDay::Morning: return "Morning";
        case TimeOfDay::Noon:    return "Noon";
        case TimeOfDay::Evening: return "Evening";
        case TimeOfDay::Night:   return "Night";
        default:                 return "Day";
    }
}

/**
 * @brief Linearly interpolates between two colors.
 *
 * This helper function blends color a into color b using the interpolation
 * factor t. The value of t is clamped to the range [0, 1] to avoid invalid
 * results.
 *
 * @param a The starting color.
 * @param b The ending color.
 * @param t The interpolation factor, where 0 means fully a and 1 means fully b.
 * @return The interpolated color.
 */
sf::Color DayCycle::lerpColor(sf::Color a, sf::Color b, float t) {
    t = std::max(0.f, std::min(1.f, t));
    return sf::Color(
        (sf::Uint8)(a.r + (int)((b.r - a.r) * t)),
        (sf::Uint8)(a.g + (int)((b.g - a.g) * t)),
        (sf::Uint8)(a.b + (int)((b.b - a.b) * t)),
        255
    );
}

/**
 * @brief Computes the current sky color based on the day cycle.
 *
 * The sky color changes smoothly throughout the day by interpolating between
 * preset colors for morning, noon, evening, and night.
 *
 * @return The current sky color.
 */
sf::Color DayCycle::getSkyColor() const {
    // Key sky colors at the major phase boundaries.
    static const sf::Color MORNING_SKY(255, 160,  90); // Orange-pink sunrise
    static const sf::Color NOON_SKY   (100, 170, 255); // Bright blue daytime sky
    static const sf::Color EVENING_SKY(255, 120,  40); // Golden-orange sunset
    static const sf::Color NIGHT_SKY  ( 10,  15,  60); // Deep navy night sky

    if (dayTime < NOON_START) {
        return lerpColor(MORNING_SKY, NOON_SKY,
                         dayTime / NOON_START);
    } else if (dayTime < EVENING_START) {
        return lerpColor(NOON_SKY, EVENING_SKY,
                         (dayTime - NOON_START) / (EVENING_START - NOON_START));
    } else if (dayTime < NIGHT_START) {
        return lerpColor(EVENING_SKY, NIGHT_SKY,
                         (dayTime - EVENING_START) / (NIGHT_START - EVENING_START));
    } else {
        // During late night, blend gradually back toward the morning sky.
        return lerpColor(NIGHT_SKY, MORNING_SKY,
                         (dayTime - NIGHT_START) / (1.f - NIGHT_START));
    }
}

/**
 * @brief Calculates the screen position of the sun.
 *
 * The sun is visible only during the daytime. Its horizontal position moves
 * from left to right across the screen, while its vertical position follows
 * a sine-wave arc so that it appears highest around noon.
 *
 * @param screenW The width of the screen in pixels.
 * @param screenH The height of the screen in pixels.
 * @return The sun's position as an sf::Vector2f, or (-1, -1) if it is night.
 */
sf::Vector2f DayCycle::getSunPosition(float screenW, float screenH) const {
    if (isNight()) return {-1.f, -1.f};

    float t = dayTime / NIGHT_START;
    float x = t * screenW;
    float y = screenH * 0.12f + screenH * 0.25f * (1.f - std::sin(t * 3.14159f));

    return {x, y};
}

/**
 * @brief Calculates the screen position of the moon.
 *
 * The moon is visible only during the night. Its movement follows a similar
 * left-to-right arc to the sun, but only during the night segment of the cycle.
 *
 * @param screenW The width of the screen in pixels.
 * @param screenH The height of the screen in pixels.
 * @return The moon's position as an sf::Vector2f, or (-1, -1) if it is not night.
 */
sf::Vector2f DayCycle::getMoonPosition(float screenW, float screenH) const {
    if (!isNight()) return {-1.f, -1.f};

    float t = (dayTime - NIGHT_START) / (1.f - NIGHT_START);
    float x = t * screenW;
    float y = screenH * 0.12f + screenH * 0.25f * (1.f - std::sin(t * 3.14159f));

    return {x, y};
}

/**
 * @brief Computes the darkness strength used for the night overlay.
 *
 * The darkness stays at 0 during the day, increases gradually during the
 * evening, stays fully dark through most of the night, and then fades again
 * near dawn.
 *
 * @return A value between 0 and 1 representing the current darkness level.
 */
float DayCycle::getNightDarkness() const {
    if (dayTime < EVENING_START) return 0.f;

    if (dayTime < NIGHT_START) {
        // Gradually darken during the evening transition.
        return (dayTime - EVENING_START) / (NIGHT_START - EVENING_START);
    }

    // During late night, begin fading darkness near dawn.
    static constexpr float PREDAWN = 0.9f;
    if (dayTime < PREDAWN) return 1.f;

    return 1.f - (dayTime - PREDAWN) / (1.f - PREDAWN);
}