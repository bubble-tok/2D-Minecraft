/**
 * @file Sleep.h
 * @brief Defines the Sleep class for tracking player fatigue level.
 *
 * Sleep drains faster at night and when the player is moving. When fully
 * depleted it slows the player and triggers a warning. The player can
 * restore it by sleeping near a placed Bed during Night phase.
 *
 * @author Group 46
 */

#pragma once
#include <algorithm>

/**
 * @class Sleep
 * @brief Tracks the player's current energy (sleep) level.
 *
 * Operates identically to Hunger but represents fatigue instead of
 * satiation. When isEmpty() is true the player's movement speed is halved.
 *
 * @author Group 46
 */
class Sleep {
private:
    int level;    ///< Current sleep level; 0 means the player is exhausted.
    int maxLevel; ///< Maximum sleep capacity.

public:
    /**
     * @brief Constructs a Sleep instance at full capacity.
     * @param maxLevel Maximum (and initial) sleep level. Defaults to 100.
     */
    Sleep(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /**
     * @brief Decreases sleep level by the given amount, clamped to 0.
     *
     * Called each game tick — twice as fast during Night phase.
     *
     * @param amount Points of sleep to remove. Defaults to 1.
     */
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /**
     * @brief Fully restores the sleep level to maxLevel.
     *
     * Called when the player sleeps in a Bed during Night.
     */
    void sleep() { level = maxLevel; }

    /**
     * @brief Returns true when sleep level has reached zero.
     * @return True if the player is exhausted, false otherwise.
     */
    bool isEmpty()     const { return level <= 0; }

    /**
     * @brief Returns the current sleep level.
     * @return Current level in the range [0, maxLevel].
     */
    int  getLevel()    const { return level; }

    /**
     * @brief Returns the sleep capacity.
     * @return Maximum sleep level set at construction.
     */
    int  getMaxLevel() const { return maxLevel; }

    /**
     * @brief Directly sets the sleep level, clamped to [0, maxLevel].
     * @param l The desired level before clamping.
     */
    void setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
};
