/**
 * @file Entity.h
 * @brief Declares the abstract Entity base class for all living game objects.
 *
 * This file defines the Entity class, which serves as the foundation for all
 * living objects in the game, including the player, animals, and enemies.
 *
 * The class provides common functionality such as:
 * - Position tracking
 * - Health management
 * - Damage handling
 * - Invincibility (i-frame) support
 *
 * It is designed as an abstract base class and must be extended by concrete
 * subclasses that implement their own update behavior.
 *
 * @author Group 46
 */

#pragma once
#include "Health.h"

/**
 * @class Entity
 * @brief Abstract base class for all living entities in the game world.
 *
 * The Entity class represents any object that has a position and health,
 * and can take damage or be destroyed. It provides shared logic used by
 * all derived classes such as Player, Animal, and Zombie.
 *
 * Each subclass must implement its own update() function to define behavior.
 *
 * @author Group 46
 */
class Entity {
protected:
    float  x;          ///< Stores the entity's world X position in pixels.
    float  y;          ///< Stores the entity's world Y position in pixels.
    Health health;     ///< Manages the entity's current and maximum hit points.
    bool   alive;      ///< Indicates whether the entity is alive (false if HP reaches 0).
    bool   invincible = false; ///< When true, the entity ignores incoming damage.

public:
    /**
     * @brief Constructs an Entity with position and health.
     *
     * Initializes the entity at the given world position and assigns a
     * Health component with the specified maximum HP.
     *
     * @param x Initial world X position in pixels.
     * @param y Initial world Y position in pixels.
     * @param maxHp Maximum (and starting) hit points. Defaults to 100.
     */
    Entity(float x, float y, int maxHp = 100);

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~Entity() = default;

    /**
     * @brief Applies damage to the entity.
     *
     * Reduces the entity's health unless it is currently invincible.
     * If health reaches zero, the entity is marked as dead.
     *
     * @param amount The amount of damage to apply.
     */
    virtual void takeDamage(int amount);

    /**
     * @brief Updates the entity each frame.
     *
     * This is a pure virtual function that must be implemented by all
     * derived classes to define their behavior (movement, AI, etc.).
     *
     * @param deltaTime Time elapsed since the previous frame in seconds.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Sets the invincibility state of the entity.
     *
     * When invincible, the entity will ignore all incoming damage.
     *
     * @param v True to enable invincibility, false to disable it.
     */
    void setInvincible(bool v);

    /**
     * @brief Checks if the entity is currently invincible.
     *
     * @return True if invincible, false otherwise.
     */
    bool isInvincible() const;

    /**
     * @brief Gets the entity's current X position.
     *
     * @return The world X coordinate.
     */
    float getX() const;

    /**
     * @brief Gets the entity's current Y position.
     *
     * @return The world Y coordinate.
     */
    float getY() const;

    /**
     * @brief Sets the entity's X position.
     *
     * @param nx The new X coordinate.
     */
    void setX(float nx);

    /**
     * @brief Sets the entity's Y position.
     *
     * @param ny The new Y coordinate.
     */
    void setY(float ny);

    /**
     * @brief Checks whether the entity is alive.
     *
     * An entity is considered alive as long as its health is above zero.
     *
     * @return True if alive, false otherwise.
     */
    bool isAlive() const;

    /**
     * @brief Gets the current health points of the entity.
     *
     * @return The current HP value.
     */
    int getHp() const;

    /**
     * @brief Sets the entity's health points directly.
     *
     * This function is primarily used for loading saved game data.
     *
     * @param h The new health value.
     */
    void setHp(int h);

    /**
     * @brief Provides access to the Health component.
     *
     * Allows external systems (such as rendering or combat systems)
     * to query or modify detailed health information.
     *
     * @return A reference to the Health object.
     */
    Health& getHealth();
};