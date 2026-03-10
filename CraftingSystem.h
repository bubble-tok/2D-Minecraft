#pragma once
#include "Inventory.h"
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>


struct Recipe {
    std::string outputName;
    int         outputQty;
    ItemType    outputType;
    int         hungerRestore = 0; ///< Only used if outputType == FOOD
    int         healthRestore = 0;
    std::map<std::string, int> ingredients; ///< name -> required quantity
};

class CraftingSystem {
private:
    std::vector<Recipe> recipes;

public:
    CraftingSystem() {
        // ── Register all recipes ──────────────────────────────────────────────

        // Bread: 3 Wheat → 1 Bread
        recipes.push_back({
            "Bread", 1, ItemType::FOOD, 25, 0,
            { {"Wheat", 3} }
        });

        // CookedMeat: 1 RawMeat → 1 CookedMeat
        recipes.push_back({
            "CookedMeat", 1, ItemType::FOOD, 40, 5,
            { {"RawMeat", 1} }
        });

        // Wooden Plank: 1 Wood → 4 WoodPlank
        recipes.push_back({
            "WoodPlank", 4, ItemType::BLOCK, 0, 0,
            { {"Wood", 1} }
        });

        // Stone Brick: 4 Stone → 4 StoneBrick
        recipes.push_back({
            "StoneBrick", 4, ItemType::BLOCK, 0, 0,
            { {"Stone", 4} }
        });

        // GoldenApple: 1 Apple + 1 Gold → 1 GoldenApple
        recipes.push_back({
            "GoldenApple", 1, ItemType::FOOD, 20, 20,
            { {"Apple", 1}, {"Gold", 1} }
        });
    }

   
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

        // Produce output
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

    /// Print all available recipes to console.
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

    /// Get list of recipe names (for UI display).
    std::vector<std::string> getRecipeNames() const {
        std::vector<std::string> names;
        for (auto& r : recipes) names.push_back(r.outputName);
        return names;
    }

private:
    const Recipe* findRecipe(const std::string& name) const {
        for (auto& r : recipes)
            if (r.outputName == name) return &r;
        return nullptr;
    }
};
