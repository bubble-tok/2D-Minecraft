/**
 * @file Entity.h
 * @brief Defines the base Entity class used by all world objects with position and health.
 */

#pragma once
#include "Health.h"

/**
 * @class Entity
 * @brief Abstract base class representing any living object in the game world.
 *
 * Entities have a world position, health system, and alive state.
 * Derived classes (such as Player, Animal, or Monster) implement
 * their own update behavior.
 */
class Entity {
protected:
    float x, y;   ///< World position of the entity (top-left coordinate)
    Health health;///< Health system associated with the entity
    bool alive;   ///< Indicates whether the entity is alive

public:

    /**
     * @brief Constructs an Entity.
     *
     * Initializes the entity's world position and maximum health.
     *
     * @param x Initial x-position in world coordinates
     * @param y Initial y-position in world coordinates
     * @param maxHp Maximum health of the entity (default = 100)
     */
    Entity(float x, float y, int maxHp = 100)
        : x(x), y(y), health(maxHp), alive(true) {}

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~Entity() = default;

    /**
     * @brief Applies damage to the entity.
     *
     * Reduces health by the specified amount. If health reaches zero,
     * the entity is marked as no longer alive.
     *
     * @param amount Amount of damage to apply
     */
    virtual void takeDamage(int amount) {
        health.decrease(amount);
        if (health.isDead()) alive = false;
    }

    /**
     * @brief Updates the entity's state each frame.
     *
     * This function must be implemented by derived classes to define
     * entity-specific behavior.
     *
     * @param deltaTime Time elapsed since the last update
     */
    virtual void update(float deltaTime) = 0;

    // --- Getters / Setters ---

    /**
     * @brief Gets the entity's x-position.
     * @return Current x-coordinate
     */
    float getX() const { return x; }

    /**
     * @brief Gets the entity's y-position.
     * @return Current y-coordinate
     */
    float getY() const { return y; }

    /**
     * @brief Sets the entity's x-position.
     * @param nx New x-coordinate
     */
    void setX(float nx) { x = nx; }

    /**
     * @brief Sets the entity's y-position.
     * @param ny New y-coordinate
     */
    void setY(float ny) { y = ny; }

    /**
     * @brief Checks whether the entity is alive.
     * @return True if the entity is alive
     */
    bool isAlive() const { return alive; }

    /**
     * @brief Gets the current health points.
     * @return Current HP value
     */
    int getHp() const { return health.getHp(); }

    /**
     * @brief Provides access to the entity's Health object.
     *
     * Allows direct manipulation of health (e.g., healing).
     *
     * @return Reference to the Health component
     */
    Health& getHealth() { return health; }
};