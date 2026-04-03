/**
 * @file Sleep.cpp
 * @brief Implements the Sleep class for tracking player fatigue level.
 *
 * @author Group 46
 */
#include "Sleep.h"
#include <algorithm>

Sleep::Sleep(int maxLevel) : level(maxLevel), maxLevel(maxLevel) {}

void Sleep::decrease(int amount) { level = std::max(0, level - amount); }

void Sleep::sleep() { level = maxLevel; }

bool Sleep::isEmpty()     const { return level <= 0; }

int  Sleep::getLevel()    const { return level; }

int  Sleep::getMaxLevel() const { return maxLevel; }

void Sleep::setLevel(int l) { level = std::clamp(l, 0, maxLevel); }
