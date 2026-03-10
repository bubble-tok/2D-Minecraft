#pragma once
#include <algorithm>


class Hunger {
private:
    int level;
    int maxLevel;
public:
    Hunger(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /// Called each game tick — decreases hunger naturally.
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /// Called when the player eats food.
    void increase(int amount)     { level = std::min(maxLevel, level + amount); }

    bool isEmpty()       const { return level <= 0; }
    int  getLevel()      const { return level; }
    int  getMaxLevel()   const { return maxLevel; }
    void setLevel(int l)       { level = std::clamp(l, 0, maxLevel); }
};
