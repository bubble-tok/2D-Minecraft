#pragma once
#include "Health.h"


class Entity {
protected:
    float x, y;       ///< World position
    Health health;
    bool alive;

public:
    Entity(float x, float y, int maxHp = 100)
        : x(x), y(y), health(maxHp), alive(true) {}

    virtual ~Entity() = default;

    /// Apply damage; sets alive=false when hp reaches 0.
    virtual void takeDamage(int amount) {
        health.decrease(amount);
        if (health.isDead()) alive = false;
    }

    virtual void update(float deltaTime) = 0;

    // --- Getters / Setters ---
    float getX()      const { return x; }
    float getY()      const { return y; }
    void  setX(float nx)    { x = nx; }
    void  setY(float ny)    { y = ny; }
    bool  isAlive()   const { return alive; }
    int   getHp()     const { return health.getHp(); }
    Health& getHealth()     { return health; }
};
