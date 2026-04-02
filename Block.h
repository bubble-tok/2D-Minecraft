/**
 * @file Block.h
 * @brief Defines the Block item class and BlockItems factory namespace.
 *
 * Blocks are placeable tiles that the player mines from the world and can
 * re-place using right-click. Each Block tracks its own durability for the
 * item representation (distinct from the world tile durability tracked by
 * World::durabilityMap).
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"

/**
 * @class Block
 * @brief An Item subclass representing a placeable world tile.
 *
 * Stores per-item durability so that the player's inventory shows how many
 * hits the block has taken. The solid flag indicates whether the block
 * provides collision when placed in the world.
 *
 * @author Group 46
 */
class Block : public Item {
private:
    int  maxDurability; ///< Hit-points when brand new.
    int  durability;    ///< Remaining hits before the block is destroyed.
    bool solid;         ///< Whether this block provides tile collision.

public:
    /**
     * @brief Constructs a Block item.
     * @param name       Display name (e.g. "Wood", "Stone").
     * @param quantity   Stack size. Defaults to 1.
     * @param durability Starting (and maximum) durability. Defaults to 3.
     * @param solid      True if the block should block entity movement. Defaults to true.
     */
    Block(const std::string& name, int quantity = 1,
          int durability = 3, bool solid = true)
        : Item(name, quantity, ItemType::BLOCK),
          maxDurability(durability), durability(durability), solid(solid) {}

    /**
     * @brief Returns the current remaining durability.
     * @return Hits remaining before the block is destroyed.
     */
    int  getDurability()    const { return durability; }

    /**
     * @brief Returns the maximum durability at creation.
     * @return Maximum hit-points for this block type.
     */
    int  getMaxDurability() const { return maxDurability; }

    /**
     * @brief Returns whether this block provides tile collision.
     * @return True if solid, false if passable.
     */
    bool isSolid()          const { return solid; }

    /**
     * @brief Returns true when durability has been reduced to zero.
     * @return True if the block has been destroyed.
     */
    bool isDestroyed()      const { return durability <= 0; }

    /**
     * @brief Applies one hit to the block, reducing durability by 1.
     * @return True if this hit destroyed the block (durability reached 0).
     */
    bool hit() {
        if (durability > 0) --durability;
        return isDestroyed();
    }

    /**
     * @brief Resets durability to its maximum value.
     *
     * Used when a block is returned to the player's inventory after
     * being mined so it appears fresh for re-placement.
     */
    void resetDurability() { durability = maxDurability; }

    /**
     * @brief Returns a description including current/max durability.
     * @return Formatted string for console or UI display.
     */
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [" + std::to_string(durability) + "/"
            + std::to_string(maxDurability) + "]";
    }
};

/**
 * @namespace BlockItems
 * @brief Factory helpers for commonly used Block types.
 *
 * Each function returns a pre-configured Block with the correct durability
 * for that material type, matching the values in World::blockMaxDurability.
 */
namespace BlockItems {
    /** @brief Creates a Wood block item. @param qty Stack size. @return Wood Block. */
    inline Block Wood(int qty = 1)  { return {"Wood",  qty, 5}; }

    /** @brief Creates a Stone block item. @param qty Stack size. @return Stone Block. */
    inline Block Stone(int qty = 1) { return {"Stone", qty, 8}; }

    /** @brief Creates a Dirt block item. @param qty Stack size. @return Dirt Block. */
    inline Block Dirt(int qty = 1)  { return {"Dirt",  qty, 2}; }

    /** @brief Creates a Sand block item. @param qty Stack size. @return Sand Block. */
    inline Block Sand(int qty = 1)  { return {"Sand",  qty, 2}; }
}
