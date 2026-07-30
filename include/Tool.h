/**
 * @file Tool.h
 * @brief Defines the Tool item class, ToolType enumeration, and ToolItems namespace.
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
    None,
    Sword,
    Pickaxe,
    Axe,
    Bed,
    Campfire
};

/**
 * @class Tool
 * @brief An Item subclass representing equippable or placeable tools.
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
     * @param name          Display name.
     * @param quantity      Stack size.
     * @param tt            Functional type of this tool.
     * @param bonusDmg      Extra attack damage. Defaults to 0.
     * @param mineSpeedMult Mining time multiplier. Defaults to 1.0.
     */
    Tool(const std::string& name, int quantity,
         ToolType tt, int bonusDmg = 0, float mineSpeedMult = 1.f);

    /** @brief Returns the tool's functional type. */
    ToolType    getToolType()      const;

    /** @brief Returns the bonus attack damage granted by this tool. */
    int         getBonusDamage()   const;

    /** @brief Returns the mining speed multiplier for this tool. */
    float       getMineSpeedMult() const;

    std::string getDescription() const override;
    std::string serialize()      const override;
};

/**
 * @namespace ToolItems
 * @brief Factory helpers for all Tool types used in the game.
 */
namespace ToolItems {
    Tool WoodenSword(int qty = 1);
    Tool StoneSword(int qty = 1);
    Tool WoodenPickaxe(int qty = 1);
    Tool StonePickaxe(int qty = 1);
    Tool Bed(int qty = 1);
    Tool Campfire(int qty = 1);
}
