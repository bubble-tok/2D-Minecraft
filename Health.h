/**
 * @file Health.h
 * @brief Defines the Health class used to manage entity health values.
 */

#pragma once
#include <algorithm>

/**
 * @class Health
 * @brief Manages health values for an entity.
 *
 * The Health class stores the current health points and the maximum health.
 * It provides functions to increase, decrease, and clamp health values so
 * that they always remain within valid bounds.
 */
class Health {
private:
    int hp;     ///< Current health points
    int maxHp;  ///< Maximum health points allowed

public:

    /**
     * @brief Constructs a Health object.
     *
     * Initializes both the current health and maximum health to the same value.
     *
     * @param maxHp Maximum health value (default = 100)
     */
    Health(int maxHp = 100) : hp(maxHp), maxHp(maxHp) {}

    /**
     * @brief Decreases the current health.
     *
     * Reduces health by the specified amount but prevents the value
     * from dropping below zero.
     *
     * @param amount Amount of damage to apply
     */
    void decrease(int amount) { hp = std::max(0, hp - amount); }

    /**
     * @brief Increases the current health.
     *
     * Adds the specified amount of health but prevents the value
     * from exceeding the maximum health.
     *
     * @param amount Amount of health to restore
     */
    void increase(int amount) { hp = std::min(maxHp, hp + amount); }

    /**
     * @brief Checks whether the entity is dead.
     *
     * @return True if health is zero or below
     */
    bool isDead() const { return hp <= 0; }

    /**
     * @brief Gets the current health value.
     *
     * @return Current health points
     */
    int getHp() const { return hp; }

    /**
     * @brief Gets the maximum health value.
     *
     * @return Maximum health points
     */
    int getMaxHp() const { return maxHp; }

    /**
     * @brief Sets the current health value.
     *
     * The value is clamped between 0 and maxHp to prevent invalid states.
     *
     * @param h New health value
     */
    void setHp(int h) { hp = std::clamp(h, 0, maxHp); }
};