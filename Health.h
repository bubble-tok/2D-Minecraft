#pragma once
#include <algorithm>

class Health {
private:
    int hp;
    int maxHp;
public:
    Health(int maxHp = 100) : hp(maxHp), maxHp(maxHp) {}

    void decrease(int amount) { hp = std::max(0, hp - amount); }
    void increase(int amount) { hp = std::min(maxHp, hp + amount); }
    bool isDead()   const { return hp <= 0; }
    int  getHp()    const { return hp; }
    int  getMaxHp() const { return maxHp; }
    void setHp(int h)     { hp = std::clamp(h, 0, maxHp); }
};
