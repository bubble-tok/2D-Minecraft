#pragma once
#include "Item.h"


class Block : public Item {
private:
    int maxDurability;
    int durability;    ///< Remaining hits before the block is destroyed
    bool solid;        ///< Whether the block has collision

public:
    Block(const std::string& name, int quantity = 1,
        int durability = 3, bool solid = true)
        : Item(name, quantity, ItemType::BLOCK),
        maxDurability(durability), durability(durability), solid(solid) {
    }

    int  getDurability()    const { return durability; }
    int  getMaxDurability() const { return maxDurability; }
    bool isSolid()          const { return solid; }
    bool isDestroyed()      const { return durability <= 0; }


    bool hit() {
        if (durability > 0) --durability;
        return isDestroyed();
    }

    void resetDurability() { durability = maxDurability; }

    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [" + std::to_string(durability) + "/" +
            std::to_string(maxDurability) + "]";
    }
};

namespace BlockItems {
    inline Block Wood(int qty = 1) { return { "Wood",  qty, 5 }; }
    inline Block Stone(int qty = 1) { return { "Stone", qty, 8 }; }
    inline Block Dirt(int qty = 1) { return { "Dirt",  qty, 2 }; }
    inline Block Sand(int qty = 1) { return { "Sand",  qty, 2 }; }
}
