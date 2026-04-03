/**
 * @file Block.cpp
 * @brief Implements the Block class, representing placeable world blocks with durability.
 *
 * This file defines the behavior of Block objects, including durability handling,
 * destruction logic, and helper factory functions for commonly used block types.
 *
 * Blocks are a type of Item and are used in the world as terrain elements that
 * can be mined, placed, and interacted with by the player.
 *
 * @author Group 46
 */

#include "Block.h"

/**
 * @brief Constructs a Block object.
 *
 * Initializes a block with a given name, quantity, durability, and solidity.
 * Durability determines how many hits the block can take before being destroyed.
 *
 * @param name        The name of the block (e.g., "Wood", "Stone").
 * @param quantity    Number of blocks in the stack.
 * @param durability  Maximum durability of the block.
 * @param solid       Whether the block is solid (collidable).
 */
Block::Block(const std::string& name, int quantity,
             int durability, bool solid)
    : Item(name, quantity, ItemType::BLOCK),
      maxDurability(durability),
      durability(durability),
      solid(solid) {}

/**
 * @brief Gets the current durability of the block.
 * @return Current durability value.
 */
int Block::getDurability() const { 
    return durability; 
}

/**
 * @brief Gets the maximum durability of the block.
 * @return Maximum durability value.
 */
int Block::getMaxDurability() const { 
    return maxDurability; 
}

/**
 * @brief Checks if the block is solid.
 *
 * Solid blocks can collide with the player and other entities.
 *
 * @return True if the block is solid, false otherwise.
 */
bool Block::isSolid() const { 
    return solid; 
}

/**
 * @brief Checks if the block has been destroyed.
 *
 * A block is considered destroyed when its durability reaches zero.
 *
 * @return True if destroyed, false otherwise.
 */
bool Block::isDestroyed() const { 
    return durability <= 0; 
}

/**
 * @brief Applies damage to the block.
 *
 * Reduces durability by one each time it is hit. If durability reaches zero,
 * the block is considered destroyed.
 *
 * @return True if the block is destroyed after the hit, false otherwise.
 */
bool Block::hit() {
    if (durability > 0) {
        --durability; // Reduce durability by 1
    }
    return isDestroyed();
}

/**
 * @brief Resets the block's durability to its maximum value.
 */
void Block::resetDurability() { 
    durability = maxDurability; 
}

/**
 * @brief Generates a string description of the block.
 *
 * Format: "Name xQuantity [current/max durability]"
 *
 * @return A formatted string describing the block.
 */
std::string Block::getDescription() const {
    return name + " x" + std::to_string(quantity)
        + " [" + std::to_string(durability) + "/"
        + std::to_string(maxDurability) + "]";
}

/**
 * @namespace BlockItems
 * @brief Provides factory functions for commonly used block types.
 *
 * These helper functions simplify the creation of standard block types
 * with predefined durability values.
 */
namespace BlockItems {

    /**
     * @brief Creates a Wood block.
     * @param qty Quantity of wood blocks.
     * @return A Block object representing wood.
     */
    Block Wood(int qty) { 
        return Block("Wood", qty, 5); 
    }

    /**
     * @brief Creates a Stone block.
     * @param qty Quantity of stone blocks.
     * @return A Block object representing stone.
     */
    Block Stone(int qty) { 
        return Block("Stone", qty, 8); 
    }

    /**
     * @brief Creates a Dirt block.
     * @param qty Quantity of dirt blocks.
     * @return A Block object representing dirt.
     */
    Block Dirt(int qty) { 
        return Block("Dirt", qty, 2); 
    }

    /**
     * @brief Creates a Sand block.
     * @param qty Quantity of sand blocks.
     * @return A Block object representing sand.
     */
    Block Sand(int qty) { 
        return Block("Sand", qty, 2); 
    }
}