/**
 * @file Food.cpp
 * @brief Implements the Food class and predefined food item factory functions.
 *
 * This file defines how food items behave in the game. Food restores hunger,
 * and some items additionally restore health. It also includes helper factory
 * functions for creating common food items such as apples and bread.
 *
 * Food is a subclass of Item and integrates with both the Hunger and Health
 * systems to support the survival gameplay loop.
 *
 * @author Group 46
 */

#include "Food.h"

/**
 * @brief Constructs a Food item with hunger and health restoration values.
 *
 * Initializes a food item with a name, quantity, and its effects on hunger
 * and health when consumed.
 *
 * @param name Name of the food item.
 * @param quantity Number of items in the stack.
 * @param hungerRestore Amount of hunger restored when consumed.
 * @param healthRestore Amount of health restored when consumed.
 */
Food::Food(const std::string& name, int quantity,
           int hungerRestore, int healthRestore)
    : Item(name, quantity, ItemType::FOOD),
      hungerRestore(hungerRestore),
      healthRestore(healthRestore) {}

/**
 * @brief Returns the hunger restoration value of the food.
 *
 * @return Amount of hunger restored.
 */
int Food::getHungerRestore() const { 
    return hungerRestore; 
}

/**
 * @brief Returns the health restoration value of the food.
 *
 * @return Amount of health restored.
 */
int Food::getHealthRestore() const { 
    return healthRestore; 
}

/**
 * @brief Consumes one unit of the food item.
 *
 * If the food stack is not empty, this function increases the player's
 * hunger and health based on the food's properties, then reduces the
 * quantity by one.
 *
 * @param hunger Reference to the Hunger system to update.
 * @param health Reference to the Health system to update.
 *
 * @return True if the food was successfully consumed, false if empty.
 */
bool Food::consume(Hunger& hunger, Health& health) {
    if (isEmpty()) return false;

    hunger.increase(hungerRestore);   // Restore hunger
    health.increase(healthRestore);   // Restore health (if applicable)

    removeQuantity(1);                // Consume one item
    return true;
}

/**
 * @brief Returns a human-readable description of the food item.
 *
 * Includes the item name, quantity, hunger restoration, and optionally
 * health restoration if it is greater than zero.
 *
 * @return A formatted string describing the item.
 */
std::string Food::getDescription() const {
    return name + " x" + std::to_string(quantity)
        + " [Hunger +" + std::to_string(hungerRestore) + "]"
        + (healthRestore > 0
           ? " [HP +" + std::to_string(healthRestore) + "]" : "");
}

/**
 * @brief Serializes the food item into a string format.
 *
 * Extends the base Item serialization by appending hunger and health
 * restoration values. This is used for saving/loading game state.
 *
 * @return A string representing the serialized food item.
 */
std::string Food::serialize() const {
    return Item::serialize() + ":"
        + std::to_string(hungerRestore) + ":"
        + std::to_string(healthRestore);
}

/**
 * @namespace FoodItems
 * @brief Factory functions for creating predefined food items.
 *
 * Provides convenient helper functions to construct commonly used food
 * items with predefined stats, avoiding repetitive constructor calls.
 */
namespace FoodItems {

    /**
     * @brief Creates an Apple item.
     * @param qty Quantity of apples.
     * @return Food object representing apples.
     */
    Food Apple(int qty) { 
        return Food("Apple", qty, 10, 3); 
    }

    /**
     * @brief Creates a Bread item.
     * @param qty Quantity of bread.
     * @return Food object representing bread.
     */
    Food Bread(int qty) { 
        return Food("Bread", qty, 25, 5); 
    }

    /**
     * @brief Creates Raw Meat.
     * @param qty Quantity of raw meat.
     * @return Food object representing raw meat.
     */
    Food RawMeat(int qty) { 
        return Food("RawMeat", qty, 15, 0); 
    }

    /**
     * @brief Creates Cooked Meat.
     * @param qty Quantity of cooked meat.
     * @return Food object representing cooked meat.
     */
    Food CookedMeat(int qty) { 
        return Food("CookedMeat", qty, 40, 5); 
    }

    /**
     * @brief Creates a Golden Apple.
     *
     * This is a high-value item used as the win condition in the game.
     *
     * @param qty Quantity of golden apples.
     * @return Food object representing golden apples.
     */
    Food GoldenApple(int qty) { 
        return Food("GoldenApple", qty, 20, 20); 
    }
}