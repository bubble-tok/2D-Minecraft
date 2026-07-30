/**
 * @file Block.h
 * @brief Declares the Block class and BlockItems factory functions used for placeable world blocks.
 *
 * This file defines the Block class, which extends Item to represent blocks
 * that can exist in the world, be stored in the inventory, placed by the player,
 * and mined or damaged over time.
 *
 * It also declares the BlockItems namespace, which provides helper factory
 * functions for creating common block types with predefined durability values.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"

/**
 * @class Block
 * @brief Represents a placeable and mineable world block.
 *
 * The Block class is a subclass of Item used for terrain and structure blocks
 * such as Wood, Stone, Dirt, and Sand. Each block stores durability values
 * so it can take damage when mined, and a solid flag so the game can determine
 * whether entities should collide with it.
 *
 * Blocks can be stacked in the inventory, placed into the world, and destroyed
 * after taking enough hits.
 *
 * @author Group 46
 */
class Block : public Item {
private:
    int  maxDurability; ///< Stores the maximum durability the block can have.
    int  durability;    ///< Stores the current remaining durability of the block.
    bool solid;         ///< Stores whether the block should block movement and collision.

public:
    /**
     * @brief Constructs a Block object with the given properties.
     *
     * This constructor creates a block item with a name, stack quantity,
     * durability value, and collision behavior.
     *
     * @param name The display name of the block, such as "Wood" or "Stone".
     * @param quantity The number of blocks in the stack. The default value is 1.
     * @param durability The starting and maximum durability of the block. The default value is 3.
     * @param solid Indicates whether the block is solid and should block movement. The default value is true.
     */
    Block(const std::string& name, int quantity = 1,
          int durability = 3, bool solid = true);

    /**
     * @brief Returns the current durability of the block.
     *
     * The current durability decreases as the block is hit or mined.
     *
     * @return The current remaining durability.
     */
    int getDurability() const;

    /**
     * @brief Returns the maximum durability of the block.
     *
     * This value represents the full durability the block starts with or
     * returns to after being reset.
     *
     * @return The maximum durability of the block.
     */
    int getMaxDurability() const;

    /**
     * @brief Checks whether the block is solid.
     *
     * Solid blocks participate in collision and prevent movement through them.
     *
     * @return True if the block is solid, or false if it is non-solid.
     */
    bool isSolid() const;

    /**
     * @brief Checks whether the block has been destroyed.
     *
     * A block is considered destroyed when its durability reaches zero or below.
     *
     * @return True if the block is destroyed, or false otherwise.
     */
    bool isDestroyed() const;

    /**
     * @brief Applies one hit of damage to the block.
     *
     * This function reduces the current durability of the block by one,
     * as long as the block still has durability remaining.
     *
     * @return True if the hit destroys the block, or false otherwise.
     */
    bool hit();

    /**
     * @brief Restores the block's durability back to its maximum value.
     *
     * This is useful when resetting a block to its original undamaged state.
     */
    void resetDurability();

    /**
     * @brief Returns a formatted text description of the block.
     *
     * The description includes the block name, quantity, and durability values.
     *
     * @return A string describing the block.
     */
    std::string getDescription() const override;
};

/**
 * @namespace BlockItems
 * @brief Provides helper functions for creating standard block types.
 *
 * The BlockItems namespace contains factory functions that simplify the
 * creation of commonly used block objects with preset names and durability.
 *
 * @author Group 46
 */
namespace BlockItems {
    /**
     * @brief Creates a Wood block item.
     *
     * @param qty The number of Wood blocks to create. The default value is 1.
     * @return A Block object representing Wood.
     */
    Block Wood(int qty = 1);

    /**
     * @brief Creates a Stone block item.
     *
     * @param qty The number of Stone blocks to create. The default value is 1.
     * @return A Block object representing Stone.
     */
    Block Stone(int qty = 1);

    /**
     * @brief Creates a Dirt block item.
     *
     * @param qty The number of Dirt blocks to create. The default value is 1.
     * @return A Block object representing Dirt.
     */
    Block Dirt(int qty = 1);

    /**
     * @brief Creates a Sand block item.
     *
     * @param qty The number of Sand blocks to create. The default value is 1.
     * @return A Block object representing Sand.
     */
    Block Sand(int qty = 1);
}