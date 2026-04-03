/**
 * @file Hunger.h
 * @brief Defines the Hunger class for tracking player satiation level.
 *
 * @author Group 46
 */

#pragma once

/**
 * @class Hunger
 * @brief Tracks the player's current hunger level.
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
    explicit Hunger(int maxLevel = 100);

    /** @brief Decreases hunger by amount, clamped to 0. */
    void decrease(int amount = 1);

    /** @brief Increases hunger by amount, clamped to maxLevel. */
    void increase(int amount);

    /** @brief Returns true when hunger level has reached zero. */
    bool isEmpty()     const;

    /** @brief Returns the current hunger level. */
    int  getLevel()    const;

    /** @brief Returns the hunger capacity. */
    int  getMaxLevel() const;

    /** @brief Sets the hunger level, clamped to [0, maxLevel]. */
    void setLevel(int l);
};
