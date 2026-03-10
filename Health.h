#pragma once
#include <algorithm>        
class Health {
private:
    int hp;                 // Current health points
    int maxHp;              // Maximum health points the entity can have

public:
    // Constructor initializes both current health and maximum health
    // Default maximum health is 100 if no value is provided
    Health(int maxHp = 100) : hp(maxHp), maxHp(maxHp) {}

    // Decreases health by a specified amount, std::max ensures health never goes below 0
    void decrease(int amount) { hp = std::max(0, hp - amount); }

    // Increases health by a specified amount, std::min ensures health never exceeds the maximum health
    void increase(int amount) { hp = std::min(maxHp, hp + amount); }

    // Returns true if health has reached zero or below (entity is dead)
    bool isDead()   const { return hp <= 0; }

    // Returns the current health value
    int  getHp()    const { return hp; }

    // Returns the maximum health value
    int  getMaxHp() const { return maxHp; }

    // Sets the current health value directly, std::clamp ensures the value stays between 0 and maxHp
    void setHp(int h)     { hp = std::clamp(h, 0, maxHp); }
};
