#pragma once
#include "Item.h"
#include "Hunger.h"
#include "Health.h"

class Food : public Item {
private:
    int hungerRestore;
    int healthRestore;

public:
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0)
        : Item(name, quantity, ItemType::FOOD),
          hungerRestore(hungerRestore),
          healthRestore(healthRestore) {}

    int getHungerRestore() const { return hungerRestore; }
    int getHealthRestore() const { return healthRestore; }

    
    bool consume(Hunger& hunger, Health& health) {
        if (isEmpty()) return false;
        hunger.increase(hungerRestore);
        health.increase(healthRestore);
        removeQuantity(1);
        return true;
    }

    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [Hunger +" + std::to_string(hungerRestore) + "]"
            + (healthRestore > 0 ? " [HP +" + std::to_string(healthRestore) + "]" : "");
    }

    std::string serialize() const override {
        return Item::serialize() + ":" +
               std::to_string(hungerRestore) + ":" +
               std::to_string(healthRestore);
    }
};

namespace FoodItems {
    inline Food Apple(int qty = 1)       { return {"Apple",       qty, 10,  0}; }
    inline Food Bread(int qty = 1)       { return {"Bread",       qty, 25,  0}; }
    inline Food RawMeat(int qty = 1)     { return {"RawMeat",     qty, 15,  0}; }
    inline Food CookedMeat(int qty = 1)  { return {"CookedMeat",  qty, 40,  5}; }
    inline Food GoldenApple(int qty = 1) { return {"GoldenApple", qty, 20, 20}; }
}
