/**
 * @file Health.cpp
 * @brief Implements the Health class for managing entity hit points.
 *
 * @author Group 46
 */
#include "Health.h"
#include <algorithm>

Health::Health(int maxHp) : hp(maxHp), maxHp(maxHp) {}


void Health::decrease(int amount) { hp = std::max(0, hp - amount); }

void Health::increase(int amount) { hp = std::min(maxHp, hp + amount); }

bool Health::isDead()   const { return hp <= 0; }

int  Health::getHp()    const { return hp; }

int  Health::getMaxHp() const { return maxHp; }

void Health::setHp(int h) { hp = std::clamp(h, 0, maxHp); }
