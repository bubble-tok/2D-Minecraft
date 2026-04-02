/**
 * @file Block.h
 * @brief The Block.h file defines the Block class and the BlockItems factory namespace.
 *
 * The Block.h file declares a placeable item type that the player can mine and
 * place in the world. Each Block object stores durability for the inventory
 * representation, which is separate from the durability stored in the world.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"

/**
 * @class Block
 * @brief The Block class represents a placeable world tile as an Item subclass.
 *
 * The Block class stores durability information for inventory usage so the player
 * can see how many hits the block has taken. The Block class also stores whether
 * the block provides collision when placed in the world.
 *
 * @author Group 46
 */
class Block : public Item {
private:
    int  maxDurability; ///< The maxDurability variable stores the maximum hit-points of the block.
    int  durability;    ///< The durability variable stores the remaining hit-points of the block.
    bool solid;         ///< The solid variable indicates whether the block provides collision.

public:
    /**
     * @brief The constructor creates a Block object.
     *
     * The constructor initializes the name, quantity, durability, and collision
     * behavior of the Block object.
     *
     * @param name The name parameter represents the display name (for example, "Wood" or "Stone").
     * @param quantity The quantity parameter represents the stack size. The default value is 1.
     * @param durability The durability parameter represents both the starting and maximum durability. The default value is 3.
     * @param solid The solid parameter determines whether the block blocks entity movement. The default value is true.
     */
    Block(const std::string& name, int quantity = 1,
          int durability = 3, bool solid = true)
        : Item(name, quantity, ItemType::BLOCK),
          maxDurability(durability), durability(durability), solid(solid) {}

    /**
     * @brief The getDurability function returns the current remaining durability of the block.
     *
     * @return The function returns the number of hits remaining before the block is destroyed.
     */
    int getDurability() const { return durability; }

    /**
     * @brief The getMaxDurability function returns the maximum durability of the block.
     *
     * @return The function returns the maximum hit-points of the block.
     */
    int getMaxDurability() const { return maxDurability; }

    /**
     * @brief The isSolid function returns whether the block provides collision.
     *
     * @return The function returns true if the block is solid, and false if the player can pass through the block.
     */
    bool isSolid() const { return solid; }

    /**
     * @brief The isDestroyed function checks whether the block has been destroyed.
     *
     * @return The function returns true if the durability is less than or equal to zero.
     */
    bool isDestroyed() const { return durability <= 0; }

    /**
     * @brief The hit function applies damage to the block.
     *
     * The hit function reduces the durability of the block by 1 if durability
     * is greater than zero.
     *
     * @return The function returns true if the block is destroyed after the hit.
     */
    bool hit() {
        if (durability > 0) --durability;
        return isDestroyed();
    }

    /**
     * @brief The resetDurability function restores the durability of the block to its maximum value.
     *
     * The resetDurability function is used when the block returns to the player's
     * inventory so that the block appears unused.
     */
    void resetDurability() { durability = maxDurability; }

    /**
     * @brief The getDescription function returns a formatted description of the block.
     *
     * The getDescription function includes the block name, quantity, and current
     * durability compared to maximum durability.
     *
     * @return The function returns a formatted string for UI or console display.
     */
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [" + std::to_string(durability) + "/"
            + std::to_string(maxDurability) + "]";
    }
};

/**
 * @namespace BlockItems
 * @brief The BlockItems namespace provides factory functions for commonly used Block types.
 *
 * The BlockItems namespace contains helper functions that return pre-configured
 * Block objects with correct durability values for each material type.
 */
namespace BlockItems {

    /**
     * @brief The Wood function creates a Wood Block object.
     *
     * @param qty The qty parameter represents the stack size.
     * @return The function returns a Wood Block object.
     */
    inline Block Wood(int qty = 1) { return {"Wood", qty, 5}; }

    /**
     * @brief The Stone function creates a Stone Block object.
     *
     * @param qty The qty parameter represents the stack size.
     * @return The function returns a Stone Block object.
     */
    inline Block Stone(int qty = 1) { return {"Stone", qty, 8}; }

    /**
     * @brief The Dirt function creates a Dirt Block object.
     *
     * @param qty The qty parameter represents the stack size.
     * @return The function returns a Dirt Block object.
     */
    inline Block Dirt(int qty = 1) { return {"Dirt", qty, 2}; }

    /**
     * @brief The Sand function creates a Sand Block object.
     *
     * @param qty The qty parameter represents the stack size.
     * @return The function returns a Sand Block object.
     */
    inline Block Sand(int qty = 1) { return {"Sand", qty, 2}; }
}