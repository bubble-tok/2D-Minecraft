/**
 * @file Food.h
 * @brief Defines the Food item class and FoodItems factory namespace.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"
#include "Hunger.h"
#include "Health.h"

/**
 * @class Food
 * @brief An Item subclass representing a consumable food item.
 *
 * @author Group 46
 */
class Food : public Item {
private:
    int hungerRestore; ///< Hunger points restored on consumption.
    int healthRestore; ///< HP restored on consumption.

public:
    /**
     * @brief Constructs a Food item.
     * @param name          Display name.
     * @param quantity      Stack size.
     * @param hungerRestore Hunger points restored when one unit is eaten.
     * @param healthRestore HP restored when one unit is eaten. Defaults to 0.
     */
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0);

    /** @brief Returns the hunger restore value for one unit of this food. */
    int getHungerRestore() const;

    /** @brief Returns the HP restore value for one unit of this food. */
    int getHealthRestore() const;

    /**
     * @brief Consumes one unit of this food, restoring Hunger and HP.
     * @return True if consumption succeeded; false if stack was empty.
     */
    bool consume(Hunger& hunger, Health& health);

    std::string getDescription() const override;
    std::string serialize()      const override;
};

/**
 * @namespace FoodItems
 * @brief Factory helpers for all Food types used in the game.
 */
namespace FoodItems {
    Food Apple(int qty = 1);
    Food Bread(int qty = 1);
    Food RawMeat(int qty = 1);
    Food CookedMeat(int qty = 1);
    Food GoldenApple(int qty = 1);
}
