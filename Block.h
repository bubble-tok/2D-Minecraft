#pragma once
#include "Item.h"

/**
 * @class Block
 * @brief Represents a placeable block item with durability and collision properties.
 *
 * Blocks are items that can exist in the world and can be damaged or destroyed.
 * Each block has durability which decreases when it is hit (e.g., mined).
 * Blocks may also be solid, meaning they block movement and collide with entities.
 */
class Block : public Item {
private:
    int maxDurability;   ///< Maximum durability the block can have
    int durability;      ///< Remaining hits before the block is destroyed
    bool solid;          ///< Whether the block has collision (cannot be walked through)

public:

    /**
     * @brief Constructs a Block item.
     *
     * @param name Name of the block (e.g., "Wood")
     * @param quantity Number of blocks in the inventory
     * @param durability Maximum durability of the block
     * @param solid Whether the block has collision
     */
    Block(const std::string& name, int quantity = 1,
        int durability = 3, bool solid = true)
        : Item(name, quantity, ItemType::BLOCK),
        maxDurability(durability),
        durability(durability),
        solid(solid) {
    }

    /**
     * @brief Gets the current durability of the block.
     * @return Current durability value
     */
    int  getDurability() const { return durability; }

    /**
     * @brief Gets the maximum durability of the block.
     * @return Maximum durability value
     */
    int  getMaxDurability() const { return maxDurability; }

    /**
     * @brief Checks if the block is solid.
     * @return True if the block has collision
     */
    bool isSolid() const { return solid; }

    /**
     * @brief Checks if the block has been destroyed.
     * @return True if durability is zero or below
     */
    bool isDestroyed() const { return durability <= 0; }

    /**
     * @brief Applies damage to the block.
     *
     * Each hit reduces durability by 1 until the block is destroyed.
     *
     * @return True if the block becomes destroyed after the hit
     */
    bool hit() {
        if (durability > 0) --durability;
        return isDestroyed();
    }

    /**
     * @brief Resets the block durability to its maximum value.
     */
    void resetDurability() { durability = maxDurability; }

    /**
     * @brief Returns a human-readable description of the block.
     *
     * Used for displaying block information in the inventory UI.
     *
     * @return Description string containing name, quantity, and durability
     */
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [" + std::to_string(durability) + "/"
            + std::to_string(maxDurability) + "]";
    }
};

/**
 * @namespace BlockItems
 * @brief Factory functions for creating common block types.
 *
 * These helper functions simplify the creation of predefined block items.
 */
namespace BlockItems {

    /**
     * @brief Creates a Wood block item.
     * @param qty Quantity of the block
     * @return Block instance representing Wood
     */
    inline Block Wood(int qty = 1) { return { "Wood",  qty, 5 }; }

    /**
     * @brief Creates a Stone block item.
     * @param qty Quantity of the block
     * @return Block instance representing Stone
     */
    inline Block Stone(int qty = 1) { return { "Stone", qty, 8 }; }

    /**
     * @brief Creates a Dirt block item.
     * @param qty Quantity of the block
     * @return Block instance representing Dirt
     */
    inline Block Dirt(int qty = 1) { return { "Dirt",  qty, 2 }; }

    /**
     * @brief Creates a Sand block item.
     * @param qty Quantity of the block
     * @return Block instance representing Sand
     */
    inline Block Sand(int qty = 1) { return { "Sand",  qty, 2 }; }
}