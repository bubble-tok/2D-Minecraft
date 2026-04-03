/**
 * @file Health.h
 * @brief Defines the Health class for managing entity hit points.
 *
 * @author Group 46
 */

#pragma once

/**
 * @class Health
 * @brief Manages an entity's hit-point pool.
 *
 * Provides clamped increase/decrease operations and a dead-state query.
 *
 * @author Group 46
 */
class Health {
private:
    int hp;    ///< Current hit points.
    int maxHp; ///< Maximum hit points; hp is always clamped to [0, maxHp].

public:
    /**
     * @brief Constructs a Health instance at full capacity.
     * @param maxHp Maximum (and initial) hit-point value. Defaults to 100.
     */
    explicit Health(int maxHp = 100);

    /** @brief Reduces current HP by the given amount, minimum 0. */
    void decrease(int amount);

    /** @brief Restores HP by the given amount, maximum maxHp. */
    void increase(int amount);

    /** @brief Returns true when current HP has reached zero. */
    bool isDead()   const;

    /** @brief Returns the current hit-point value. */
    int  getHp()    const;

    /** @brief Returns the maximum hit-point cap. */
    int  getMaxHp() const;

    /** @brief Clamps HP to [0, maxHp]. */
    void setHp(int h);
};
