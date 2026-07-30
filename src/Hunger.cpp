/**
 * @file Hunger.cpp
 * @brief Implements the Hunger class for tracking player satiation level.
 *
 * @author Group 46
 */
#include "Hunger.h"
#include <algorithm>

Hunger::Hunger(int maxLevel) : level(maxLevel), maxLevel(maxLevel) {}

void Hunger::decrease(int amount) { level = std::max(0, level - amount); }

void Hunger::increase(int amount) { level = std::min(maxLevel, level + amount); }

bool Hunger::isEmpty()     const { return level <= 0; }

int  Hunger::getLevel()    const { return level; }

int  Hunger::getMaxLevel() const { return maxLevel; }

void Hunger::setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
