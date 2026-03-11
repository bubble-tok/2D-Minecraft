#pragma once
#include <algorithm>

/**
 * @class Sleep
 * @brief Manages the player's sleep (fatigue) system.
 *
 * The Sleep class tracks how rested the player is. The sleep level
 * gradually decreases over time, representing fatigue. When the
 * player sleeps, the sleep level is fully restored. If the level
 * reaches zero, gameplay effects such as reduced movement speed
 * may occur.
 */
class Sleep {
private:
    int level;    ///< Current sleep level (how rested the player is)
    int maxLevel; ///< Maximum sleep level

public:

    /**
     * @brief Constructs a Sleep object.
     *
     * Initializes both the current sleep level and maximum sleep
     * level to the same value.
     *
     * @param maxLevel Maximum sleep value (default = 100)
     */
    Sleep(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /**
     * @brief Decreases the sleep level over time.
     *
     * This function simulates fatigue building up as time passes.
     * The level will never drop below zero.
     *
     * @param amount Amount of sleep to decrease (default = 1)
     */
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /**
     * @brief Restores sleep level to maximum.
     *
     * Called when the player sleeps, fully restoring the fatigue meter.
     */
    void sleep() { level = maxLevel; }

    /**
     * @brief Checks whether the player is fully exhausted.
     *
     * @return True if sleep level is zero or below
     */
    bool isEmpty() const { return level <= 0; }

    /**
     * @brief Gets the current sleep level.
     *
     * @return Current sleep value
     */
    int getLevel() const { return level; }

    /**
     * @brief Gets the maximum sleep level.
     *
     * @return Maximum sleep value
     */
    int getMaxLevel() const { return maxLevel; }

    /**
     * @brief Sets the sleep level.
     *
     * The value is clamped between 0 and maxLevel to prevent invalid values.
     *
     * @param l New sleep level
     */
    void setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
};