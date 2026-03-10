#pragma once                     
#include "Item.h"                


class Block : public Item {     // Block class inherits from Item (so a Block is a type of Item)
private:
    int maxDurability;          
    int durability;             //< Remaining hits before the block is destroyed
    bool solid;                 //< Whether the block has collision (can the player walk through it?)

public:
    // this is a constructor for creating a Block item
    // name = block name (e.g., "Wood")
    // quantity = how many of this block exist in inventory
    // durability = how many hits the block can take before breaking
    // solid = whether the block blocks movement
    Block(const std::string& name, int quantity = 1,
        int durability = 3, bool solid = true)
        : Item(name, quantity, ItemType::BLOCK),  
        maxDurability(durability),                // Sets the maximum durability
        durability(durability),                   // Sets the current durability equal to the maximum initially
        solid(solid) {                            // Sets whether the block is solid or not
    }

    // Returns the current durability of the block
    int  getDurability()    const { return durability; }

    // Returns the maximum durability of the block
    int  getMaxDurability() const { return maxDurability; }

    // Returns whether the block is solid (collidable)
    bool isSolid()          const { return solid; }

    // Returns true if the block is destroyed (durability reached zero)
    bool isDestroyed()      const { return durability <= 0; }


    // Called when the block is hit (e.g., mined or damaged)
    bool hit() {
        if (durability > 0) --durability;   // Reduce durability by 1 if the block still has durability left
        return isDestroyed();               // Return true if the block has now been destroyed
    }

    // Resets the block's durability back to its maximum value
    void resetDurability() { durability = maxDurability; }

    // Returns a text description of the block (used for UI or inventory display)
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)   // Block name and quantity
            + " [" + std::to_string(durability) + "/"   // Current durability
            + std::to_string(maxDurability) + "]";      // Maximum durability
    }
};


// Namespace used as a factory/helper to quickly create common block types
namespace BlockItems {

    // Creates a Wood block with default durability = 5
    inline Block Wood(int qty = 1) { return { "Wood",  qty, 5 }; }

    // Creates a Stone block with durability = 8 (stronger block)
    inline Block Stone(int qty = 1) { return { "Stone", qty, 8 }; }

    // Creates a Dirt block with durability = 2 (weaker block)
    inline Block Dirt(int qty = 1) { return { "Dirt",  qty, 2 }; }

    // Creates a Sand block with durability = 2
    inline Block Sand(int qty = 1) { return { "Sand",  qty, 2 }; }
}