/**
 * @file Entity.cpp
 * @brief Implements the Entity base class, which represents all living objects in the game.
 *
 * This file defines the core behavior shared by all entities such as players,
 * animals, and enemies. It includes position handling, health management,
 * damage processing, and invincibility logic.
 *
 * The Entity class acts as a foundational class that other game objects extend.
 *
 * @author Group 46
 */

#include "Entity.h"

/**
 * @brief Constructs an Entity with a position and maximum health.
 *
 * Initializes the entity at a given (x, y) position and assigns a Health
 * object with the specified maximum HP. The entity starts alive by default.
 *
 * @param x Initial x-position of the entity.
 * @param y Initial y-position of the entity.
 * @param maxHp Maximum health points of the entity.
 */
Entity::Entity(float x, float y, int maxHp)
    : x(x), y(y), health(maxHp), alive(true) {}

/**
 * @brief Applies damage to the entity.
 *
 * If the entity is not invincible, its health is reduced by the given amount.
 * If health reaches zero, the entity is marked as no longer alive.
 *
 * @param amount The amount of damage to apply.
 */
void Entity::takeDamage(int amount) {
    if (invincible) return; // Ignore damage if invincible

    health.decrease(amount);

    if (health.isDead()) {
        alive = false; // Mark entity as dead
    }
}

/**
 * @brief Sets whether the entity is invincible.
 *
 * When invincible, the entity will not take any damage.
 *
 * @param v True to make the entity invincible, false otherwise.
 */
void Entity::setInvincible(bool v) { 
    invincible = v; 
}

/**
 * @brief Checks if the entity is currently invincible.
 *
 * @return True if the entity is invincible, false otherwise.
 */
bool Entity::isInvincible() const { 
    return invincible; 
}

/**
 * @brief Gets the current x-position of the entity.
 *
 * @return The x-coordinate.
 */
float Entity::getX() const { 
    return x; 
}

/**
 * @brief Gets the current y-position of the entity.
 *
 * @return The y-coordinate.
 */
float Entity::getY() const { 
    return y; 
}

/**
 * @brief Sets the x-position of the entity.
 *
 * @param nx The new x-coordinate.
 */
void Entity::setX(float nx) { 
    x = nx; 
}

/**
 * @brief Sets the y-position of the entity.
 *
 * @param ny The new y-coordinate.
 */
void Entity::setY(float ny) { 
    y = ny; 
}

/**
 * @brief Checks whether the entity is alive.
 *
 * An entity is considered alive if its health has not reached zero.
 *
 * @return True if alive, false otherwise.
 */
bool Entity::isAlive() const { 
    return alive; 
}

/**
 * @brief Returns the current health points of the entity.
 *
 * @return The current HP value.
 */
int Entity::getHp() const { 
    return health.getHp(); 
}

/**
 * @brief Sets the entity's health points.
 *
 * The value is clamped within valid bounds by the Health class.
 *
 * @param h The new health value.
 */
void Entity::setHp(int h) { 
    health.setHp(h); 
}

/**
 * @brief Provides access to the entity's Health object.
 *
 * This allows external systems (such as rendering or combat logic)
 * to query additional health-related information like max HP.
 *
 * @return A reference to the Health object.
 */
Health& Entity::getHealth() { 
    return health; 
}