/**
 * @file Tool.cpp
 * @brief Implements the Tool item class and ToolItems factory functions.
 *
 * @author Group 46
 */
#include "Tool.h"

Tool::Tool(const std::string& name, int quantity,
           ToolType tt, int bonusDmg, float mineSpeedMult)
    : Item(name, quantity, ItemType::TOOL),
      toolType(tt), bonusDamage(bonusDmg),
      mineSpeedMult(mineSpeedMult) {}

ToolType Tool::getToolType()      const { return toolType; }

int      Tool::getBonusDamage()   const { return bonusDamage; }

float    Tool::getMineSpeedMult() const { return mineSpeedMult; }

std::string Tool::getDescription() const {
    std::string desc = name + " x" + std::to_string(quantity);
    if (bonusDamage > 0)
        desc += " [DMG+" + std::to_string(bonusDamage) + "]";
    if (mineSpeedMult < 1.f)
        desc += " [Mine" + std::to_string((int)((1.f - mineSpeedMult) * 100)) + "%faster]";
    return desc;
}

std::string Tool::serialize() const {
    return name + ":" + std::to_string(quantity) + ":"
         + std::to_string(static_cast<int>(ItemType::TOOL));
}

namespace ToolItems {
    Tool WoodenSword(int qty)   { return Tool("WoodenSword",   qty, ToolType::Sword,   5,  1.f); }
    Tool StoneSword(int qty)    { return Tool("StoneSword",    qty, ToolType::Sword,   12, 1.f); }
    Tool WoodenPickaxe(int qty) { return Tool("WoodenPickaxe", qty, ToolType::Pickaxe, 0,  0.6f); }
    Tool StonePickaxe(int qty)  { return Tool("StonePickaxe",  qty, ToolType::Pickaxe, 0,  0.4f); }
    Tool Bed(int qty)           { return Tool("Bed",           qty, ToolType::Bed,     0,  1.f); }
    Tool Campfire(int qty)      { return Tool("Campfire",      qty, ToolType::Campfire,0,  1.f); }
}
