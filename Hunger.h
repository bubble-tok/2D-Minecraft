/**
 * @file Hunger.h
 * @brief Defines the Hunger class for tracking player satiation level.
 *
 * Hunger drains over time as the player moves and exists, and is restored
 * by consuming food items. When the level reaches zero, the player begins
 * losing HP from starvation.
 *
 * @author Group 46
 */

#pragma once
#include <algorithm>

/**
 * @class Hunger
 * @brief Tracks the player's current hunger (satiation) level.
 *
 * The hunger level starts at maxLevel and decreases each tick. Food items
 * call increase() to restore it. The value is always clamped to [0, maxLevel].
 *
 * @author Group 46
 */
class Hunger {
private:
    int level;    ///< Current hunger level; 0 means the player is starving.
    int maxLevel; ///< Maximum hunger capacity.

public:
    /**
     * @brief Constructs a Hunger instance at full capacity.
     * @param maxLevel Maximum (and initial) hunger level. Defaults to 100.
     */
    Hunger(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /**
     * @brief Decreases hunger by the given amount, clamped to 0.
     *
     * Called each game tick — faster when the player is moving.
     *
     * @param amount Points of hunger to remove. Defaults to 1.
     */
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /**
     * @brief Increases hunger by the given amount, clamped to maxLevel.
     *
     * Called when the player eats food.
     *
     * @param amount Points of hunger to restore.
     */
    void increase(int amount) { level = std::min(maxLevel, level + amount); }

    /**
     * @brief Returns true when the hunger level has reached zero.
     * @return True if the player is starving, false otherwise.
     */
    bool isEmpty()     const { return level <= 0; }

    /**
     * @brief Returns the current hunger level.
     * @return Current level in the range [0, maxLevel].
     */
    int  getLevel()    const { return level; }

    /**
     * @brief Returns the hunger capacity.
     * @return Maximum hunger level set at construction.
     */
    int  getMaxLevel() const { return maxLevel; }

    /**
     * @brief Directly sets the hunger level, clamped to [0, maxLevel].
     * @param l The desired level before clamping.
     */
    void setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
};
