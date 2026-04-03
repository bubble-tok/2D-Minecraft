/**
 * @file Entity.h
 * @brief Defines the Entity abstract base class for all living game objects.
 *
 * @author Group 46
 */

#pragma once
#include "Health.h"

/**
 * @class Entity
 * @brief Abstract base class for all living game objects.
 *
 * @author Group 46
 */
class Entity {
protected:
    float  x;          ///< World X position in pixels.
    float  y;          ///< World Y position in pixels.
    Health health;     ///< Current hit points for this entity.
    bool   alive;      ///< False once health reaches zero.
    bool   invincible = false; ///< When true, takeDamage() is ignored.

public:
    /**
     * @brief Constructs an Entity at the given world position with the given max HP.
     * @param x     Initial world X position in pixels.
     * @param y     Initial world Y position in pixels.
     * @param maxHp Maximum (and initial) hit points. Defaults to 100.
     */
    Entity(float x, float y, int maxHp = 100);

    virtual ~Entity() = default;

    /**
     * @brief Applies damage unless the entity is invincible.
     * @param amount Positive damage amount to apply.
     */
    virtual void takeDamage(int amount);

    /**
     * @brief Per-frame update hook. Must be implemented by each concrete subclass.
     * @param deltaTime Elapsed time since the previous frame in seconds.
     */
    virtual void update(float deltaTime) = 0;

    /** @brief Enables or disables the invincibility (iframe) window. */
    void setInvincible(bool v);

    /** @brief Returns whether this entity is currently invincible. */
    bool isInvincible()  const;

    /** @brief Returns the entity's current world X position. */
    float getX()      const;

    /** @brief Returns the entity's current world Y position. */
    float getY()      const;

    /** @brief Sets the entity's world X position directly. */
    void  setX(float nx);

    /** @brief Sets the entity's world Y position directly. */
    void  setY(float ny);

    /** @brief Returns true if this entity has not yet been killed. */
    bool  isAlive()   const;

    /** @brief Returns current hit points of the entity. */
    int   getHp()     const;

    /** @brief Sets current HP directly (used by save/load). */
    void  setHp(int h);

    /**
     * @brief Returns a mutable reference to the Health component.
     * @return Reference to the Health object.
     */
    Health& getHealth();
};
