/**
 * @file Health.h
 * @brief Defines the Health class for managing entity hit points.
 *
 * Health encapsulates an entity's current and maximum HP, clamping all
 * modifications so the value never goes below 0 or above maxHp.
 *
 * @author Group 46
 */

#pragma once
#include <algorithm>

/**
 * @class Health
 * @brief Manages an entity's hit-point pool.
 *
 * Provides clamped increase/decrease operations and a dead-state query.
 * Used by Entity as a composition member rather than direct int fields so
 * the clamping logic is centralised in one place.
 *
 * @author Group 46
 */
class Health {
private:
    int hp;     ///< Current hit points.
    int maxHp;  ///< Maximum hit points; hp is always clamped to [0, maxHp].

public:
    /**
     * @brief Constructs a Health instance at full capacity.
     * @param maxHp Maximum (and initial) hit-point value. Defaults to 100.
     */
    Health(int maxHp = 100) : hp(maxHp), maxHp(maxHp) {}

    /**
     * @brief Reduces current HP by the given amount, clamped to 0.
     * @param amount Number of hit points to remove (must be positive).
     */
    void decrease(int amount) { hp = std::max(0, hp - amount); }

    /**
     * @brief Restores HP by the given amount, clamped to maxHp.
     * @param amount Number of hit points to restore (must be positive).
     */
    void increase(int amount) { hp = std::min(maxHp, hp + amount); }

    /**
     * @brief Returns true when current HP has reached zero.
     * @return True if the entity is dead, false otherwise.
     */
    bool isDead()   const { return hp <= 0; }

    /**
     * @brief Returns the current hit-point value.
     * @return Current HP in the range [0, maxHp].
     */
    int  getHp()    const { return hp; }

    /**
     * @brief Returns the maximum hit-point cap.
     * @return Maximum HP value set at construction.
     */
    int  getMaxHp() const { return maxHp; }

    /**
     * @brief Directly sets HP to an arbitrary value, clamped to [0, maxHp].
     * @param h The desired HP value before clamping.
     */
    void setHp(int h) { hp = std::clamp(h, 0, maxHp); }
};
