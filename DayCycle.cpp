/**
 * @file DayCycle.cpp
 * @brief Implements DayCycle time progression, sky colour, and celestial positions.
 *
 * @author Group 46
 */
#include "DayCycle.h"
#include <cmath>
#include <algorithm>

sf::Color DayCycle::lerpColor(sf::Color a, sf::Color b, float t) {
    // Clamp t to [0,1] to prevent overshoot at phase boundaries
    t = std::max(0.f, std::min(1.f, t));
    return sf::Color(
        (sf::Uint8)(a.r + (b.r - a.r) * t),
        (sf::Uint8)(a.g + (b.g - a.g) * t),
        (sf::Uint8)(a.b + (b.b - a.b) * t),
        255);
}

void DayCycle::update(float dt) {
    // Advance dayTime by the fraction of a day that dt represents
    dayTime += dt / DAY_DURATION;
    if (dayTime >= 1.f) {
        dayTime -= 1.f; // wrap back to start of new day
        ++dayCount;
    }
}

void DayCycle::skipToMorning() {
    // If currently in Night, the next morning is a new day
    if (dayTime >= NIGHT_START) ++dayCount;
    dayTime = MORNING_START + 0.01f; // small offset to avoid exact boundary
}

TimeOfDay DayCycle::getPhase() const {
    if (dayTime < NOON_START)    return TimeOfDay::Morning;
    if (dayTime < EVENING_START) return TimeOfDay::Noon;
    if (dayTime < NIGHT_START)   return TimeOfDay::Evening;
    return TimeOfDay::Night;
}

bool DayCycle::isNight()   const { return getPhase() == TimeOfDay::Night; }
bool DayCycle::isMorning() const { return getPhase() == TimeOfDay::Morning; }

std::string DayCycle::getPhaseName() const {
    switch (getPhase()) {
        case TimeOfDay::Morning: return "Morning";
        case TimeOfDay::Noon:    return "Noon";
        case TimeOfDay::Evening: return "Evening";
        case TimeOfDay::Night:   return "Night";
    }
    return "";
}

sf::Color DayCycle::getSkyColor() const {
    // Key colours for each phase of day
    static const sf::Color NIGHT_COLOR  ( 10,  10,  35); // deep navy
    static const sf::Color MORNING_COLOR(255, 160,  80); // warm orange
    static const sf::Color NOON_COLOR   (100, 180, 255); // bright blue
    static const sf::Color EVENING_COLOR(220, 100,  40); // burnt orange

    float t = dayTime;

    if (t < NOON_START) {
        // Morning: blend night → morning → noon in two sub-segments
        float f = t / NOON_START;
        if (f < 0.3f) return lerpColor(NIGHT_COLOR, MORNING_COLOR, f / 0.3f);
        return lerpColor(MORNING_COLOR, NOON_COLOR, (f - 0.3f) / 0.7f);
    }
    if (t < EVENING_START) return NOON_COLOR; // solid blue midday
    if (t < NIGHT_START) {
        // Evening: noon → orange sunset → night
        float f = (t - EVENING_START) / (NIGHT_START - EVENING_START);
        if (f < 0.5f) return lerpColor(NOON_COLOR, EVENING_COLOR, f / 0.5f);
        return lerpColor(EVENING_COLOR, NIGHT_COLOR, (f - 0.5f) / 0.5f);
    }
    return NIGHT_COLOR;
}

sf::Vector2f DayCycle::getSunPosition(float screenW, float screenH) const {
    if (isNight()) return {-1.f, -1.f}; // sun is below the horizon at night

    // Map dayTime [0, NIGHT_START] → progress [0, 1] across the sky
    float progress = dayTime / NIGHT_START;
    float x = screenW * progress;
    // Sine arc: rises from horizon, peaks at noon, sets at horizon
    float arc = std::sin(progress * 3.14159f);
    float y = screenH * 0.12f + (1.f - arc) * screenH * 0.10f;
    return {x, y};
}

sf::Vector2f DayCycle::getMoonPosition(float screenW, float screenH) const {
    if (!isNight()) return {-1.f, -1.f}; // moon only visible at night

    // Map Night sub-range → progress [0, 1]
    float progress = (dayTime - NIGHT_START) / (1.f - NIGHT_START);
    float x = screenW * progress;
    float arc = std::sin(progress * 3.14159f);
    float y = screenH * 0.10f + (1.f - arc) * screenH * 0.08f;
    return {x, y};
}

float DayCycle::getNightDarkness() const {
    if (dayTime < EVENING_START) return 0.f; // full daylight — no overlay
    if (dayTime < NIGHT_START)
        // Gradually darken during evening transition
        return (dayTime - EVENING_START) / (NIGHT_START - EVENING_START);
    if (dayTime > 0.9f)
        // Gradually brighten in the last 10% of the night (pre-dawn)
        return 1.f - (dayTime - 0.9f) / 0.1f;
    return 1.f; // full darkness during deep night
}
