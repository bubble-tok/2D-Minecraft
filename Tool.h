/**
 * @file Tool.h
 * @brief Defines the Tool item class, ToolType enumeration, and ToolItems namespace.
 *
 * Tools are equippable items that grant bonuses while held in the hotbar.
 * Swords increase attack damage; pickaxes reduce mining time. Beds and
 * Campfires can be placed in the world and interact with the sleep and
 * cooking systems respectively.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"
#include <string>

/**
 * @enum ToolType
 * @brief Functional sub-category of a Tool item.
 */
enum class ToolType {
    None,     ///< Unspecified; used as a default in Recipe structs.
    Sword,    ///< Increases player attack damage when equipped.
    Pickaxe,  ///< Reduces mining time per durability hit when equipped.
    Axe,      ///< Reduces wood-block mining time (future use).
    Bed,      ///< Placeable world object; required for sleeping.
    Campfire  ///< Placeable world object; required for cooking RawMeat.
};

/**
 * @class Tool
 * @brief An Item subclass representing equippable or placeable tools.
 *
 * Equippable tools (Sword, Pickaxe) grant bonuses through Player's
 * getTotalAttackDamage() and getMineSpeedMult() queries. Placeable tools
 * (Bed, Campfire) are deposited as named tiles in the world and detected
 * by Game::nearBlock().
 *
 * @author Group 46
 */
class Tool : public Item {
private:
    ToolType toolType;       ///< Determines which bonus this tool provides.
    int      bonusDamage;    ///< Extra attack damage added by a Sword.
    float    mineSpeedMult;  ///< Mining time multiplier; < 1.0 means faster.

public:
    /**
     * @brief Constructs a Tool item.
     * @param name          Display name (e.g. "WoodenPickaxe", "Bed").
     * @param quantity      Stack size (typically 1 for tools).
     * @param tt            Functional type of this tool.
     * @param bonusDmg      Extra attack damage (Sword only). Defaults to 0.
     * @param mineSpeedMult Mining time multiplier (Pickaxe/Axe only). Defaults to 1.0.
     */
    Tool(const std::string& name, int quantity,
         ToolType tt, int bonusDmg = 0, float mineSpeedMult = 1.f)
        : Item(name, quantity, ItemType::TOOL),
          toolType(tt), bonusDamage(bonusDmg),
          mineSpeedMult(mineSpeedMult) {}

    /**
     * @brief Returns the tool's functional type.
     * @return ToolType enum value.
     */
    ToolType getToolType()      const { return toolType; }

    /**
     * @brief Returns the bonus attack damage granted by this tool.
     * @return Extra damage points; 0 for non-Sword tools.
     */
    int      getBonusDamage()   const { return bonusDamage; }

    /**
     * @brief Returns the mining speed multiplier for this tool.
     *
     * Values less than 1.0 mean faster mining.
     * A WoodenPickaxe returns 0.6 (40% faster); StonePickaxe returns 0.4 (60% faster).
     *
     * @return Multiplier applied to BASE_MINE_TIME; 1.0 means no bonus.
     */
    float    getMineSpeedMult() const { return mineSpeedMult; }

    /**
     * @brief Returns a description including bonus stats.
     * @return Formatted string for console or UI display.
     */
    std::string getDescription() const override {
        std::string desc = name + " x" + std::to_string(quantity);
        if (bonusDamage > 0)
            desc += " [DMG+" + std::to_string(bonusDamage) + "]";
        if (mineSpeedMult < 1.f)
            desc += " [Mine" + std::to_string((int)((1.f - mineSpeedMult) * 100)) + "%faster]";
        return desc;
    }

    /**
     * @brief Serialises the tool as a TOOL-typed item.
     *
     * Tool-specific fields (bonusDamage, mineSpeedMult) are NOT serialised
     * because tools are identified by name on load and reconstructed with
     * their default stats by the recipe system.
     *
     * @return Serialised string: "name:qty:typeInt".
     */
    std::string serialize() const override {
        return name + ":" + std::to_string(quantity) + ":"
             + std::to_string(static_cast<int>(ItemType::TOOL));
    }
};

/**
 * @namespace ToolItems
 * @brief Factory helpers for all Tool types used in the game.
 */
namespace ToolItems {
    /** @brief Wooden sword (+5 attack damage). @param qty Stack size. */
    inline Tool WoodenSword(int qty = 1)   { return {"WoodenSword",   qty, ToolType::Sword,   5,  1.f}; }

    /** @brief Stone sword (+12 attack damage). @param qty Stack size. */
    inline Tool StoneSword(int qty = 1)    { return {"StoneSword",    qty, ToolType::Sword,   12, 1.f}; }

    /** @brief Wooden pickaxe (mines 40% faster). @param qty Stack size. */
    inline Tool WoodenPickaxe(int qty = 1) { return {"WoodenPickaxe", qty, ToolType::Pickaxe, 0,  0.6f}; }

    /** @brief Stone pickaxe (mines 60% faster). @param qty Stack size. */
    inline Tool StonePickaxe(int qty = 1)  { return {"StonePickaxe",  qty, ToolType::Pickaxe, 0,  0.4f}; }

    /** @brief Bed — placeable; required to sleep during Night. @param qty Stack size. */
    inline Tool Bed(int qty = 1)           { return {"Bed",           qty, ToolType::Bed,      0,  1.f}; }

    /** @brief Campfire — placeable; required to cook RawMeat. @param qty Stack size. */
    inline Tool Campfire(int qty = 1)      { return {"Campfire",      qty, ToolType::Campfire, 0,  1.f}; }
}
