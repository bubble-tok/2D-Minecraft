/**
 * @file Hunger.h
 * @brief Defines the Hunger class used to manage hunger mechanics.
 */

#pragma once
#include <algorithm>

/**
 * @class Hunger
 * @brief Manages hunger levels for an entity such as the player.
 *
 * The Hunger class tracks the current hunger level and the maximum hunger.
 * Hunger naturally decreases over time and increases when food is consumed.
 */
class Hunger {
private:
    int level;     ///< Current hunger level
    int maxLevel;  ///< Maximum hunger level allowed

public:

    /**
     * @brief Constructs a Hunger object.
     *
     * Initializes both the current hunger and maximum hunger to the same value.
     *
     * @param maxLevel Maximum hunger value (default = 100)
     */
    Hunger(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /**
     * @brief Decreases hunger over time.
     *
     * This function is typically called each game tick to simulate
     * natural hunger loss.
     *
     * @param amount Amount of hunger to decrease (default = 1)
     */
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /**
     * @brief Increases hunger when food is consumed.
     *
     * Hunger will not exceed the maximum allowed value.
     *
     * @param amount Amount of hunger restored
     */
    void increase(int amount) { level = std::min(maxLevel, level + amount); }

    /**
     * @brief Checks if hunger has been depleted.
     *
     * @return True if hunger is zero or below
     */
    bool isEmpty() const { return level <= 0; }

    /**
     * @brief Gets the current hunger level.
     *
     * @return Current hunger value
     */
    int getLevel() const { return level; }

    /**
     * @brief Gets the maximum hunger level.
     *
     * @return Maximum hunger value
     */
    int getMaxLevel() const { return maxLevel; }

    /**
     * @brief Sets the hunger level directly.
     *
     * The value is clamped between 0 and maxLevel to ensure
     * it remains within valid bounds.
     *
     * @param l New hunger level
     */
    void setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
};