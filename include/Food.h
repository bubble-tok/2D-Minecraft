/**
 * @file Food.h
 * @brief Declares the Food class and factory helpers for consumable items.
 *
 * This file defines the Food class, a subclass of Item that represents
 * consumable items used to restore hunger and optionally health.
 *
 * It also includes the FoodItems namespace, which provides convenient
 * factory functions for creating predefined food types used in the game.
 *
 * Food plays a key role in the survival system by allowing the player to
 * recover hunger and maintain health over time.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"
#include "Hunger.h"
#include "Health.h"

/**
 * @class Food
 * @brief Represents a consumable item that restores hunger and optionally health.
 *
 * The Food class extends Item by adding gameplay effects when consumed.
 * Each food item restores a fixed amount of hunger, and some also restore
 * health points.
 *
 * This class interacts directly with the Hunger and Health systems to
 * support the survival mechanics of the game.
 *
 * @author Group 46
 */
class Food : public Item {
private:
    int hungerRestore; ///< Amount of hunger restored when consumed.
    int healthRestore; ///< Amount of health restored when consumed.

public:
    /**
     * @brief Constructs a Food item with specific effects.
     *
     * Initializes a food item with a name, quantity, and its associated
     * hunger and health restoration values.
     *
     * @param name Display name of the food item.
     * @param quantity Number of items in the stack.
     * @param hungerRestore Amount of hunger restored per use.
     * @param healthRestore Amount of health restored per use (default is 0).
     */
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0);

    /**
     * @brief Gets the hunger restoration value.
     *
     * @return Amount of hunger restored by one unit.
     */
    int getHungerRestore() const;

    /**
     * @brief Gets the health restoration value.
     *
     * @return Amount of health restored by one unit.
     */
    int getHealthRestore() const;

    /**
     * @brief Consumes one unit of this food item.
     *
     * If the stack is not empty, this function increases the player's
     * hunger and health based on the item's values and reduces the
     * quantity by one.
     *
     * @param hunger Reference to the Hunger system.
     * @param health Reference to the Health system.
     *
     * @return True if the item was successfully consumed, false otherwise.
     */
    bool consume(Hunger& hunger, Health& health);

    /**
     * @brief Returns a formatted description of the food item.
     *
     * Includes name, quantity, hunger restoration, and optional health
     * restoration values for display in the UI.
     *
     * @return A string describing the food item.
     */
    std::string getDescription() const override;

    /**
     * @brief Serializes the food item into a string.
     *
     * Used for saving and loading game state, extending the base Item
     * serialization with food-specific attributes.
     *
     * @return A serialized string representation of the food item.
     */
    std::string serialize() const override;
};

/**
 * @namespace FoodItems
 * @brief Factory functions for creating predefined food items.
 *
 * This namespace provides helper functions that construct commonly used
 * food items with predefined values, simplifying item creation throughout
 * the game code.
 */
namespace FoodItems {

    /**
     * @brief Creates an Apple item.
     * @param qty Quantity of apples (default is 1).
     * @return A Food object representing apples.
     */
    Food Apple(int qty = 1);

    /**
     * @brief Creates a Bread item.
     * @param qty Quantity of bread (default is 1).
     * @return A Food object representing bread.
     */
    Food Bread(int qty = 1);

    /**
     * @brief Creates a Raw Meat item.
     * @param qty Quantity of raw meat (default is 1).
     * @return A Food object representing raw meat.
     */
    Food RawMeat(int qty = 1);

    /**
     * @brief Creates a Cooked Meat item.
     * @param qty Quantity of cooked meat (default is 1).
     * @return A Food object representing cooked meat.
     */
    Food CookedMeat(int qty = 1);

    /**
     * @brief Creates a Golden Apple item.
     *
     * This is a high-value item used as the win condition in the game.
     *
     * @param qty Quantity of golden apples (default is 1).
     * @return A Food object representing golden apples.
     */
    Food GoldenApple(int qty = 1);
}