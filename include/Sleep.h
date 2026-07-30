/**
 * @file Sleep.h
 * @brief Defines the Sleep class for tracking player fatigue level.
 *
 * @author Group 46
 */

#pragma once

/**
 * @class Sleep
 * @brief Tracks the player's current energy (sleep) level.
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
    explicit Sleep(int maxLevel = 100);

    /** @brief Decreases sleep level by amount, clamped to 0. */
    void decrease(int amount = 1);

    /** @brief Fully restores the sleep level to maxLevel. */
    void sleep();

    /** @brief Returns true when sleep level has reached zero. */
    bool isEmpty()     const;

    /** @brief Returns the current sleep level. */
    int  getLevel()    const;

    /** @brief Returns the sleep capacity. */
    int  getMaxLevel() const;

    /** @brief Sets the sleep level, clamped to [0, maxLevel]. */
    void setLevel(int l);
};
