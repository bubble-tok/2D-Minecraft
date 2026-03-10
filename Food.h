#pragma once                     
#include "Item.h"                
#include "Hunger.h"              
#include "Health.h"             

class Food : public Item {      
private:
    int hungerRestore;          // Amount of hunger restored when this food is consumed
    int healthRestore;          // Amount of health restored when this food is consumed

public:
    // this is a constructor for creating a Food item
    // name = name of the food item
    // quantity = how many items exist in inventory
    // hungerRestore = how much hunger it restores
    // healthRestore = optional health recovery (default is 0)
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0)
        : Item(name, quantity, ItemType::FOOD),  
          hungerRestore(hungerRestore),          // Initialize hunger restoration value
          healthRestore(healthRestore) {}        // Initialize health restoration value

    // Returns how much hunger this food restores
    int getHungerRestore() const { return hungerRestore; }

    // Returns how much health this food restores
    int getHealthRestore() const { return healthRestore; }

    
    // Consumes one unit of the food item and applies its effects
    bool consume(Hunger& hunger, Health& health) {
        if (isEmpty()) return false;          // If there are no items left, consumption fails
        hunger.increase(hungerRestore);       // Increase the player's hunger meter
        health.increase(healthRestore);       // Increase the player's health 
        removeQuantity(1);                    // Remove one food item from inventory
        return true;                          // Return true indicating successful consumption
    }

    // Returns a description string for displaying the item in UI or inventory
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)          // Item name and quantity
            + " [Hunger +" + std::to_string(hungerRestore) + "]" // Hunger restoration info
            + (healthRestore > 0 ? " [HP +" + std::to_string(healthRestore) + "]" : ""); // Health info if applicable
    }

    // Serializes the item into a string format for saving/loading game state
    std::string serialize() const override {
        return Item::serialize() + ":" +            // Base item serialization (name, quantity, type, etc.)
               std::to_string(hungerRestore) + ":" + // Add hunger restoration value
               std::to_string(healthRestore);        // Add health restoration value
    }
};


// Namespace used as a factory/helper for creating predefined food items
namespace FoodItems {

    // Creates an Apple food item 
    inline Food Apple(int qty = 1)       { return {"Apple",       qty, 10,  0}; }

    // Creates Bread 
    inline Food Bread(int qty = 1)       { return {"Bread",       qty, 25,  0}; }

    // Creates Raw Meat
    inline Food RawMeat(int qty = 1)     { return {"RawMeat",     qty, 15,  0}; }

    // Creates Cooked Meat 
    inline Food CookedMeat(int qty = 1)  { return {"CookedMeat",  qty, 40,  5}; }

    // Creates Golden Apple
    inline Food GoldenApple(int qty = 1) { return {"GoldenApple", qty, 20, 20}; }
}