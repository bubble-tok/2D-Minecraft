/**
 * @file CraftingSystem.h
 * @brief Defines crafting recipes and the crafting system used to combine items.
 */

#pragma once
#include "Inventory.h"
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>

/**
 * @struct Recipe
 * @brief Represents a crafting recipe.
 *
 * A recipe defines an output item and the ingredients required to craft it.
 */
struct Recipe {
    std::string outputName;                 ///< Name of the crafted item
    int         outputQty;                  ///< Quantity of the crafted item produced
    ItemType    outputType;                 ///< Type of item produced (FOOD or BLOCK)
    int         hungerRestore = 0;          ///< Hunger restoration value (used only if FOOD)
    int         healthRestore = 0;          ///< Health restoration value (used only if FOOD)
    std::map<std::string, int> ingredients; ///< Map of ingredient name → required quantity
};

/**
 * @class CraftingSystem
 * @brief Handles crafting logic and recipe management.
 *
 * The CraftingSystem stores all available recipes and allows items to be crafted
 * if the required ingredients are present in the player's inventory.
 */
class CraftingSystem {
private:
    std::vector<Recipe> recipes; ///< List of all registered crafting recipes

public:

    /**
     * @brief Constructs the crafting system and registers all recipes.
     */
    CraftingSystem() {

        /// Bread: 3 Wheat → 1 Bread
        recipes.push_back({
            "Bread", 1, ItemType::FOOD, 25, 0,
            { {"Wheat", 3} }
        });

        /// CookedMeat: 1 RawMeat → 1 CookedMeat
        recipes.push_back({
            "CookedMeat", 1, ItemType::FOOD, 40, 5,
            { {"RawMeat", 1} }
        });

        /// Wooden Plank: 1 Wood → 4 WoodPlank
        recipes.push_back({
            "WoodPlank", 4, ItemType::BLOCK, 0, 0,
            { {"Wood", 1} }
        });

        /// Stone Brick: 4 Stone → 4 StoneBrick
        recipes.push_back({
            "StoneBrick", 4, ItemType::BLOCK, 0, 0,
            { {"Stone", 4} }
        });

        /// GoldenApple: 1 Apple + 1 Gold → 1 GoldenApple
        recipes.push_back({
            "GoldenApple", 1, ItemType::FOOD, 20, 20,
            { {"Apple", 1}, {"Gold", 1} }
        });
    }

    /**
     * @brief Attempts to craft an item using the player's inventory.
     *
     * Checks if the required ingredients are present in the inventory,
     * consumes them if available, and adds the crafted item.
     *
     * @param itemName Name of the item to craft
     * @param inventory Reference to the player's inventory
     * @return True if crafting succeeded, false otherwise
     */
    bool craft(const std::string& itemName, Inventory& inventory) {

        // Find matching recipe
        const Recipe* recipe = findRecipe(itemName);
        if (!recipe) {
            std::cout << "[Craft] No recipe found for: " << itemName << "\n";
            return false;
        }

        // Check ingredients
        for (auto& [ingr, qty] : recipe->ingredients) {
            if (!inventory.hasItem(ingr, qty)) {
                std::cout << "[Craft] Missing ingredient: "
                          << qty << "x " << ingr << "\n";
                return false;
            }
        }

        // Consume ingredients
        for (auto& [ingr, qty] : recipe->ingredients)
            inventory.removeItem(ingr, qty);

        // Produce output item
        std::shared_ptr<Item> output;
        if (recipe->outputType == ItemType::FOOD) {
            output = std::make_shared<Food>(
                recipe->outputName, recipe->outputQty,
                recipe->hungerRestore, recipe->healthRestore);
        } else {
            output = std::make_shared<Block>(
                recipe->outputName, recipe->outputQty);
        }

        inventory.addItem(output);

        std::cout << "[Craft] Crafted " << recipe->outputQty
                  << "x " << recipe->outputName << "!\n";

        return true;
    }

    /**
     * @brief Prints all available crafting recipes to the console.
     */
    void printRecipes() const {
        std::cout << "=== Crafting Recipes ===\n";
        for (auto& r : recipes) {
            std::cout << "  " << r.outputQty << "x " << r.outputName << " <- ";
            bool first = true;
            for (auto& [ingr, qty] : r.ingredients) {
                if (!first) std::cout << " + ";
                std::cout << qty << "x " << ingr;
                first = false;
            }
            std::cout << "\n";
        }
        std::cout << "========================\n";
    }

    /**
     * @brief Returns a list of all recipe output names.
     *
     * Useful for displaying available recipes in a UI.
     *
     * @return Vector containing the names of craftable items
     */
    std::vector<std::string> getRecipeNames() const {
        std::vector<std::string> names;
        for (auto& r : recipes) names.push_back(r.outputName);
        return names;
    }

private:

    /**
     * @brief Finds a recipe by its output item name.
     *
     * @param name Name of the item produced by the recipe
     * @return Pointer to the recipe if found, otherwise nullptr
     */
    const Recipe* findRecipe(const std::string& name) const {
        for (auto& r : recipes)
            if (r.outputName == name) return &r;
        return nullptr;
    }
};