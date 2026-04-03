/**
 * @file Entity.cpp
 * @brief Implements the Entity base class for all living game objects.
 *
 * @author Group 46
 */
#include "Entity.h"

Entity::Entity(float x, float y, int maxHp)
    : x(x), y(y), health(maxHp), alive(true) {}

void Entity::takeDamage(int amount) {
    if (invincible) return;
    health.decrease(amount);
    if (health.isDead()) alive = false;
}

void Entity::setInvincible(bool v) { invincible = v; }

bool Entity::isInvincible() const { return invincible; }

float Entity::getX()     const { return x; }

float Entity::getY()     const { return y; }

void  Entity::setX(float nx) { x = nx; }

void  Entity::setY(float ny) { y = ny; }

bool  Entity::isAlive()  const { return alive; }

int   Entity::getHp()    const { return health.getHp(); }

void  Entity::setHp(int h) { health.setHp(h); }

Health& Entity::getHealth() { return health; }
