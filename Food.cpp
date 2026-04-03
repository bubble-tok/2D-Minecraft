/**
 * @file Food.cpp
 * @brief Implements the Food item class and FoodItems factory functions.
 *
 * Apple and Bread now restore a small amount of HP in addition to hunger.
 *
 * @author Group 46
 */
#include "Food.h"

Food::Food(const std::string& name, int quantity,
           int hungerRestore, int healthRestore)
    : Item(name, quantity, ItemType::FOOD),
      hungerRestore(hungerRestore),
      healthRestore(healthRestore) {}

int Food::getHungerRestore() const { return hungerRestore; }

int Food::getHealthRestore() const { return healthRestore; }

bool Food::consume(Hunger& hunger, Health& health) {
    if (isEmpty()) return false;
    hunger.increase(hungerRestore);
    health.increase(healthRestore);
    removeQuantity(1);
    return true;
}

std::string Food::getDescription() const {
    return name + " x" + std::to_string(quantity)
        + " [Hunger +" + std::to_string(hungerRestore) + "]"
        + (healthRestore > 0
           ? " [HP +" + std::to_string(healthRestore) + "]" : "");
}

std::string Food::serialize() const {
    return Item::serialize() + ":"
        + std::to_string(hungerRestore) + ":"
        + std::to_string(healthRestore);
}

namespace FoodItems {
    Food Apple(int qty)       { return Food("Apple",       qty, 10,  3); }
    Food Bread(int qty)       { return Food("Bread",       qty, 25,  5); }
    Food RawMeat(int qty)     { return Food("RawMeat",     qty, 15,  0); }
    Food CookedMeat(int qty)  { return Food("CookedMeat",  qty, 40,  5); }
    Food GoldenApple(int qty) { return Food("GoldenApple", qty, 20, 20); }
}
