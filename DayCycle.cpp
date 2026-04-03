/**
 * @file DayCycle.cpp
 * @brief Implements day/night cycle progression and sky visual helpers.
 *
 * @author Group 46
 */
#include "DayCycle.h"
#include <cmath>
#include <algorithm>

void DayCycle::update(float dt) {
    dayTime += dt / DAY_DURATION;
    if (dayTime >= 1.f) {
        dayTime -= 1.f;
        ++dayCount;
    }
}

void DayCycle::skipToMorning() {
    if (isNight()) ++dayCount;
    dayTime = MORNING_START;
}

TimeOfDay DayCycle::getPhase() const {
    if (dayTime < NOON_START)    return TimeOfDay::Morning;
    if (dayTime < EVENING_START) return TimeOfDay::Noon;
    if (dayTime < NIGHT_START)   return TimeOfDay::Evening;
    return TimeOfDay::Night;
}

bool DayCycle::isNight()   const { return dayTime >= NIGHT_START; }
bool DayCycle::isMorning() const { return dayTime < NOON_START; }

float DayCycle::getDayTime()  const { return dayTime; }
int   DayCycle::getDayCount() const { return dayCount; }

void DayCycle::setDayTime(float t) { dayTime  = t; }
void DayCycle::setDayCount(int c)  { dayCount = c; }

std::string DayCycle::getPhaseName() const {
    switch (getPhase()) {
        case TimeOfDay::Morning: return "Morning";
        case TimeOfDay::Noon:    return "Noon";
        case TimeOfDay::Evening: return "Evening";
        case TimeOfDay::Night:   return "Night";
        default:                 return "Day";
    }
}

sf::Color DayCycle::lerpColor(sf::Color a, sf::Color b, float t) {
    t = std::max(0.f, std::min(1.f, t));
    return sf::Color(
        (sf::Uint8)(a.r + (int)((b.r - a.r) * t)),
        (sf::Uint8)(a.g + (int)((b.g - a.g) * t)),
        (sf::Uint8)(a.b + (int)((b.b - a.b) * t)),
        255
    );
}

sf::Color DayCycle::getSkyColor() const {
    // Key sky colours at each phase boundary
    static const sf::Color MORNING_SKY(255, 160,  90); // orange-pink sunrise
    static const sf::Color NOON_SKY   (100, 170, 255); // bright blue
    static const sf::Color EVENING_SKY(255, 120,  40); // golden-orange sunset
    static const sf::Color NIGHT_SKY  ( 10,  15,  60); // deep navy

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
        // Night fades back toward a pre-dawn morning at the very end
        return lerpColor(NIGHT_SKY, MORNING_SKY,
                         (dayTime - NIGHT_START) / (1.f - NIGHT_START));
    }
}

sf::Vector2f DayCycle::getSunPosition(float screenW, float screenH) const {
    if (isNight()) return {-1.f, -1.f};
    // Normalise dayTime [0, NIGHT_START) → t in [0, 1)
    float t = dayTime / NIGHT_START;
    float x = t * screenW;
    // Sine arc: highest at t=0.5 (noon)
    float y = screenH * 0.12f + screenH * 0.25f * (1.f - std::sin(t * 3.14159f));
    return {x, y};
}

sf::Vector2f DayCycle::getMoonPosition(float screenW, float screenH) const {
    if (!isNight()) return {-1.f, -1.f};
    // Normalise dayTime [NIGHT_START, 1.0) → t in [0, 1)
    float t = (dayTime - NIGHT_START) / (1.f - NIGHT_START);
    float x = t * screenW;
    float y = screenH * 0.12f + screenH * 0.25f * (1.f - std::sin(t * 3.14159f));
    return {x, y};
}

float DayCycle::getNightDarkness() const {
    if (dayTime < EVENING_START) return 0.f;
    if (dayTime < NIGHT_START) {
        // Ramp up through evening
        return (dayTime - EVENING_START) / (NIGHT_START - EVENING_START);
    }
    // Full night — begin fading in the last 10% for a pre-dawn glow
    static constexpr float PREDAWN = 0.9f;
    if (dayTime < PREDAWN) return 1.f;
    return 1.f - (dayTime - PREDAWN) / (1.f - PREDAWN);
}
