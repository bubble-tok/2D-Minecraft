/**
 * @file Block.cpp
 * @brief Implements the Block item class and BlockItems factory functions.
 *
 * @author Group 46
 */
#include "Block.h"

Block::Block(const std::string& name, int quantity,
             int durability, bool solid)
    : Item(name, quantity, ItemType::BLOCK),
      maxDurability(durability), durability(durability), solid(solid) {}

int  Block::getDurability()    const { return durability; }

int  Block::getMaxDurability() const { return maxDurability; }

bool Block::isSolid()          const { return solid; }

bool Block::isDestroyed()      const { return durability <= 0; }

bool Block::hit() {
    if (durability > 0) --durability;
    return isDestroyed();
}

void Block::resetDurability() { durability = maxDurability; }

std::string Block::getDescription() const {
    return name + " x" + std::to_string(quantity)
        + " [" + std::to_string(durability) + "/"
        + std::to_string(maxDurability) + "]";
}

namespace BlockItems {
    Block Wood(int qty)  { return Block("Wood",  qty, 5); }
    Block Stone(int qty) { return Block("Stone", qty, 8); }
    Block Dirt(int qty)  { return Block("Dirt",  qty, 2); }
    Block Sand(int qty)  { return Block("Sand",  qty, 2); }
}
