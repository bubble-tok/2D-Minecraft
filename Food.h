/**
 * @file Food.h
 * @brief Defines the Food item class and FoodItems factory namespace.
 *
 * Food items restore the player's Hunger and optionally HP when consumed.
 * RawMeat is blocked from direct consumption; it must be cooked at a
 * Campfire first. GoldenApple triggers the game's win condition.
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
 * Each Food stores how much Hunger and HP it restores on consumption.
 * The consume() method integrates directly with Hunger and Health objects
 * owned by the player.
 *
 * @author Group 46
 */
class Food : public Item {
private:
    int hungerRestore; ///< Hunger points restored on consumption.
    int healthRestore; ///< HP restored on consumption (0 for most foods).

public:
    /**
     * @brief Constructs a Food item.
     * @param name          Display name (e.g. "Apple", "CookedMeat").
     * @param quantity      Stack size.
     * @param hungerRestore Hunger points restored when one unit is eaten.
     * @param healthRestore HP restored when one unit is eaten. Defaults to 0.
     */
    Food(const std::string& name, int quantity,
         int hungerRestore, int healthRestore = 0)
        : Item(name, quantity, ItemType::FOOD),
          hungerRestore(hungerRestore),
          healthRestore(healthRestore) {}

    /**
     * @brief Returns the hunger restore value for one unit of this food.
     * @return Points of Hunger restored on consumption.
     */
    int getHungerRestore() const { return hungerRestore; }

    /**
     * @brief Returns the HP restore value for one unit of this food.
     * @return Points of Health restored on consumption (may be 0).
     */
    int getHealthRestore() const { return healthRestore; }

    /**
     * @brief Consumes one unit of this food, restoring Hunger and HP.
     *
     * Reduces the stack quantity by 1. If the stack becomes empty after
     * consumption, the caller (Inventory::eatSelected) sets the slot to nullptr.
     *
     * @param hunger Reference to the player's Hunger object to restore.
     * @param health Reference to the player's Health object to restore.
     * @return True if consumption succeeded; false if the stack was already empty.
     */
    bool consume(Hunger& hunger, Health& health) {
        if (isEmpty()) return false;
        hunger.increase(hungerRestore); // restore satiation
        health.increase(healthRestore); // restore HP (0 for most foods)
        removeQuantity(1);              // decrement stack
        return true;
    }

    /**
     * @brief Returns a description including hunger and HP restore values.
     * @return Formatted string for console or UI display.
     */
    std::string getDescription() const override {
        return name + " x" + std::to_string(quantity)
            + " [Hunger +" + std::to_string(hungerRestore) + "]"
            + (healthRestore > 0
               ? " [HP +" + std::to_string(healthRestore) + "]" : "");
    }

    /**
     * @brief Serialises this food to a colon-delimited string.
     *
     * Extends the base Item serialisation with hunger and health restore
     * values so they can be reconstructed by SaveManager::load.
     * Format: "name:qty:typeInt:hungerRestore:healthRestore"
     *
     * @return Serialised string for SaveManager.
     */
    std::string serialize() const override {
        return Item::serialize() + ":"
            + std::to_string(hungerRestore) + ":"
            + std::to_string(healthRestore);
    }
};

/**
 * @namespace FoodItems
 * @brief Factory helpers for all Food types used in the game.
 */
namespace FoodItems {
    /** @brief Apple — basic hunger restore. @param qty Stack size. @return Apple Food. */
    inline Food Apple(int qty = 1)       { return {"Apple",       qty, 10,  0}; }

    /** @brief Bread — better hunger restore. @param qty Stack size. @return Bread Food. */
    inline Food Bread(int qty = 1)       { return {"Bread",       qty, 25,  0}; }

    /** @brief Raw meat — cannot be eaten directly; must be cooked. @param qty Stack size. */
    inline Food RawMeat(int qty = 1)     { return {"RawMeat",     qty, 15,  0}; }

    /** @brief Cooked meat — best regular food; also heals 5 HP. @param qty Stack size. */
    inline Food CookedMeat(int qty = 1)  { return {"CookedMeat",  qty, 40,  5}; }

    /** @brief Golden Apple — win condition item; heals 20 HP and 20 Hunger. @param qty Stack size. */
    inline Food GoldenApple(int qty = 1) { return {"GoldenApple", qty, 20, 20}; }
}
