/**
 * @file Block.h
 * @brief Defines the Block class and the BlockItems factory namespace.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"

/**
 * @class Block
 * @brief Represents a placeable world tile as an Item subclass.
 *
 * @author Group 46
 */
class Block : public Item {
private:
    int  maxDurability; ///< Maximum hit-points of the block.
    int  durability;    ///< Remaining hit-points of the block.
    bool solid;         ///< Whether the block provides collision.

public:
    /**
     * @brief Constructs a Block object.
     * @param name       Display name (e.g. "Wood" or "Stone").
     * @param quantity   Stack size. Defaults to 1.
     * @param durability Starting and maximum durability. Defaults to 3.
     * @param solid      Whether the block blocks entity movement. Defaults to true.
     */
    Block(const std::string& name, int quantity = 1,
          int durability = 3, bool solid = true);

    /** @brief Returns the current remaining durability. */
    int  getDurability()    const;

    /** @brief Returns the maximum durability. */
    int  getMaxDurability() const;

    /** @brief Returns whether the block provides collision. */
    bool isSolid()          const;

    /** @brief Returns true if durability has reached zero. */
    bool isDestroyed()      const;

    /**
     * @brief Applies one hit to the block, reducing durability by 1.
     * @return True if the block is destroyed after the hit.
     */
    bool hit();

    /** @brief Restores durability to maxDurability. */
    void resetDurability();

    std::string getDescription() const override;
};

/**
 * @namespace BlockItems
 * @brief Factory functions for commonly used Block types.
 */
namespace BlockItems {
    Block Wood(int qty = 1);
    Block Stone(int qty = 1);
    Block Dirt(int qty = 1);
    Block Sand(int qty = 1);
}
