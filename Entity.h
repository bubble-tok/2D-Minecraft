/**
 * @file Entity.h
 * @brief This header file defines the Entity abstract base class for all living game objects.
 *
 * Entity encapsulates the position, health, alive-state, and invincibility
 * flag shared by Player, Animal, Monster, and Zombie. Concrete subclasses
 * implement the update() pure virtual method and may override takeDamage().
 *
 * @author Group 46
 */

#pragma once
#include "Health.h"

/**
 * @class Entity
 * @brief Abstract base class for all living game objects.
 *
 * Stores world-space position (x, y), a Health component, a boolean alive
 * flag, and an invincible flag that blocks damage during iframe windows.
 * All entities that move, take damage, or are simulated will inherit from this.
 *
 * @author Group 46
 */
class Entity {
protected:
    float  x;          ///< World X position in pixels.
    float  y;          ///< World Y position in pixels.
    Health health;     ///< Current amount of hit points for this entity.
    bool   alive;      ///< False once health reaches zero; entity is awaiting removal.
    bool   invincible = false; ///< When true, takeDamage() is ignored (iframe window).

public:
    /**
     * @brief Constructs an Entity at the given world position with the given max HP.
     * @param x     Initial world X position in pixels.
     * @param y     Initial world Y position in pixels.
     * @param maxHp Maximum (and initial) hit points. Defaults to 100.
     */
    Entity(float x, float y, int maxHp = 100)
        : x(x), y(y), health(maxHp), alive(true) {}

    /// Virtual destructor required because subclasses are managed via pointers.
    virtual ~Entity() = default;

    /**
     * @brief Applies damage to this entity unless it is currently invincible.
     *
     * If the resulting HP reaches zero, sets alive to false so the entity
     * can be removed from the world on the next cleanup pass.
     *
     * @param amount Positive damage amount to apply.
     */
    virtual void takeDamage(int amount) {
        if (invincible) return;        // the damage will be ignored if the entity is invincible.
        health.decrease(amount);
        if (health.isDead()) alive = false;
    }

    /**
     * @brief Per-frame update hook. Must be implemented by each concrete subclass.
     * @param deltaTime Elapsed time since the previous frame in seconds.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Enables or disables the invincibility (iframe) window.
     *
     * While invincible is true, all calls to takeDamage() are no-ops.
     * Used by Game to give the player a grace period after being hit.
     *
     * @param v True to enable invincibility; false to disable it.
     */
    void setInvincible(bool v) { invincible = v; }

    /**
     * @brief Returns whether this entity is currently invincible.
     * @return True if the entity is in an iframe window, false otherwise.
     */
    bool isInvincible()  const { return invincible; }

    /**
     * @brief Returns the entity's current world X position.
     * @return X coordinate in pixels.
     */
    float getX()      const { return x; }

    /**
     * @brief Returns the entity's current world Y position.
     * @return Y coordinate in pixels.
     */
    float getY()      const { return y; }

    /**
     * @brief Sets the entity's world X position directly.
     * @param nx New X coordinate in pixels.
     */
    void  setX(float nx) { x = nx; }

    /**
     * @brief Sets the entity's world Y position directly.
     * @param ny New Y coordinate in pixels.
     */
    void  setY(float ny) { y = ny; }

    /**
     * @brief Returns true if this entity has not yet been killed.
     * @return True while alive; false once HP has reached zero.
     */
    bool  isAlive()   const { return alive; }

    /**
     * @brief Returns current hit points of the entity
     * @return Current HP value.
     */
    int   getHp()     const { return health.getHp(); }

    /**
     * @brief Returns a mutable reference to the Health component.
     *
     * Used by Renderer to query maxHp for health-bar display without
     * exposing individual get/set methods on Entity itself.
     *
     * @return Reference to the Health object.
     */
    Health& getHealth() { return health; }
};
