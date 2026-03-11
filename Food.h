/**
 * @file Food.h
 * @brief Defines the Food class representing consumable items that restore hunger and health.
 */

#pragma once                     
#include "Item.h"                
#include "Hunger.h"              
#include "Health.h"             

/**
 * @class Food
 * @brief Represents a consumable item that restores hunger and optionally health.
 *
 * Food items can be stored in an inventory and consumed by the player.
 * When consumed, they increase hunger and optionally restore health.
 */
class Food : public Item {      
private:
    int hungerRestore;   ///< Amount of hunger restored when this food is consumed
    int healthRestore;   ///< Amount of health restored when this food is consumed

public:

    /**
     * @brief Constructs a Food item.
     *
     * @param name Name of the food item
     * @param quantity Number of items in the inventory
     * @param hungerRestore Amount of hunger restored when consumed
     * @param healthRestore Amount of health restored when consumed (default = 0)
     */
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0)
        : Item(name, quantity, ItemType::FOOD),
          hungerRestore(hungerRestore),
          healthRestore(healthRestore) {}

    /**
     * @brief Gets the hunger restoration value.
     * @return Amount of hunger restored by the food item
     */
    int getHungerRestore() const { return hungerRestore; }

    /**
     * @brief Gets the health restoration value.
     * @return Amount of health restored by the food item
     */
    int getHealthRestore() const { return healthRestore; }

    /**
     * @brief Consumes one unit of the food item.
     *
     * Applies hunger and health restoration effects and removes
     * one item from the inventory.
     *
     * @param hunger Reference to the player's Hunger system
     * @param health Reference to the player's Health system
     * @return True if the food was successfully consumed
     */
    bool consume(Hunger& hunger, Health& health) {
        if (isEmpty()) return false;
        hunger.increase(hungerRestore);
        health.increase(healthRestore);
        removeQuantity(1);
        return true;
    }

    /**
     * @brief Returns a human-readable description of the food item.
     *
     * Used for displaying item information in the inventory UI.
     *
     * @return Description string containing name, quantity, and effects
     */
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [Hunger +" + std::to_string(hungerRestore) + "]"
            + (healthRestore > 0 ? " [HP +" + std::to_string(healthRestore) + "]" : "");
    }

    /**
     * @brief Serializes the food item into a string format.
     *
     * Used for saving and loading game state.
     *
     * @return Serialized representation of the food item
     */
    std::string serialize() const override {
        return Item::serialize() + ":" +
               std::to_string(hungerRestore) + ":" +
               std::to_string(healthRestore);
    }
};

/**
 * @namespace FoodItems
 * @brief Factory helper functions for creating predefined food items.
 *
 * These functions simplify the creation of commonly used food objects.
 */
namespace FoodItems {

    /**
     * @brief Creates an Apple food item.
     * @param qty Quantity of apples
     * @return Food object representing an apple
     */
    inline Food Apple(int qty = 1)       { return {"Apple",       qty, 10,  0}; }

    /**
     * @brief Creates a Bread food item.
     * @param qty Quantity of bread
     * @return Food object representing bread
     */
    inline Food Bread(int qty = 1)       { return {"Bread",       qty, 25,  0}; }

    /**
     * @brief Creates a RawMeat food item.
     * @param qty Quantity of raw meat
     * @return Food object representing raw meat
     */
    inline Food RawMeat(int qty = 1)     { return {"RawMeat",     qty, 15,  0}; }

    /**
     * @brief Creates a CookedMeat food item.
     * @param qty Quantity of cooked meat
     * @return Food object representing cooked meat
     */
    inline Food CookedMeat(int qty = 1)  { return {"CookedMeat",  qty, 40,  5}; }

    /**
     * @brief Creates a GoldenApple food item.
     * @param qty Quantity of golden apples
     * @return Food object representing a golden apple
     */
    inline Food GoldenApple(int qty = 1) { return {"GoldenApple", qty, 20, 20}; }
}