#pragma once
#include <algorithm>


class Sleep {
private:
    int level;
    int maxLevel;

public:
    Sleep(int maxLevel = 100) : level(maxLevel), maxLevel(maxLevel) {}

    /// Called on a timer — fatigue builds up over time.
    void decrease(int amount = 1) { level = std::max(0, level - amount); }

    /// Player sleeps — fully restores sleep level.
    void sleep() { level = maxLevel; }

    bool isEmpty()     const { return level <= 0; }
    int  getLevel()    const { return level; }
    int  getMaxLevel() const { return maxLevel; }
    void setLevel(int l)     { level = std::clamp(l, 0, maxLevel); }
};
