/**
 * @file CraftingSystem.cpp
 * @brief The CraftingSystem.cpp file implements recipe registration and the crafting execution pipeline.
 *
 * The CraftingSystem.cpp file defines how the CraftingSystem class stores
 * recipes, looks up recipe data, checks crafting requirements, consumes
 * ingredients, and creates crafted output items.
 *
 * @author Group 46
 */
#include "CraftingSystem.h"
#include <iostream>

/**
 * @brief The constructor initializes the CraftingSystem object and registers all available recipes.
 *
 * The constructor adds recipes for basic materials, tools, food items, and
 * station-based crafting outputs. The constructor also separates recipes by
 * whether they require no station, a CraftingTable, or a Campfire.
 */
CraftingSystem::CraftingSystem() {
    // The constructor adds recipes that do not require a CraftingTable.
    recipes.push_back({"WoodPlank",    4, ItemType::BLOCK, 0, 0, {{"Wood",1}}});
    recipes.push_back({"Stick",        4, ItemType::MISC,  0, 0, {{"WoodPlank",2}}});
    recipes.push_back({"StoneBrick",   4, ItemType::BLOCK, 0, 0, {{"Stone",4}}});
    recipes.push_back({"GoldenApple",  1, ItemType::FOOD, 20,20, {{"Apple",1},{"Gold",1}}});
    recipes.push_back({"Bed",          1, ItemType::TOOL,  0, 0, {{"WoodPlank",3}},
                        ToolType::Bed, 0, 1.f, false});
    recipes.push_back({"Campfire",     1, ItemType::TOOL,  0, 0, {{"Wood",3},{"Stone",2}},
                        ToolType::Campfire, 0, 1.f, false});
    recipes.push_back({"CraftingTable",1, ItemType::BLOCK, 0, 0, {{"WoodPlank",4}}});

    // The constructor adds recipes that require a CraftingTable nearby.
    recipes.push_back({"WoodenPickaxe",1, ItemType::TOOL, 0, 0, {{"WoodPlank",3},{"Stick",2}},
                        ToolType::Pickaxe, 0, 0.6f, true});
    recipes.push_back({"StonePickaxe", 1, ItemType::TOOL, 0, 0, {{"StoneBrick",3},{"Stick",2}},
                        ToolType::Pickaxe, 0, 0.4f, true});
    recipes.push_back({"WoodenSword",  1, ItemType::TOOL, 0, 0, {{"WoodPlank",2},{"Stick",1}},
                        ToolType::Sword, 5, 1.f, true});
    recipes.push_back({"StoneSword",   1, ItemType::TOOL, 0, 0, {{"StoneBrick",2},{"Stick",1}},
                        ToolType::Sword, 12, 1.f, true});

    // The constructor adds recipes that require a Campfire nearby.
    recipes.push_back({"CookedMeat",   1, ItemType::FOOD, 40, 5, {{"RawMeat",1}},
                        ToolType::None, 0, 1.f, false});
}

/**
 * @brief The findRecipe function searches for a recipe by output item name.
 *
 * The findRecipe function scans the stored recipe list and returns a pointer
 * to the recipe whose output name matches the requested item name.
 *
 * @param name The name parameter represents the output item name being searched for.
 * @return The function returns a pointer to the matching Recipe object, or nullptr if no recipe is found.
 */
const Recipe* CraftingSystem::findRecipe(const std::string& name) const {
    for (auto& r : recipes)
        if (r.outputName == name) return &r;

    return nullptr; // The function returns nullptr when no matching recipe exists.
}

/**
 * @brief The requiresTable function checks whether a recipe requires a CraftingTable.
 *
 * The requiresTable function looks up the recipe by name and checks the
 * requiresCraftingTable flag stored in the Recipe object.
 *
 * @param name The name parameter represents the output item name of the recipe.
 * @return The function returns true if the recipe exists and requires a CraftingTable.
 */
bool CraftingSystem::requiresTable(const std::string& name) const {
    const Recipe* r = findRecipe(name);
    return r && r->requiresCraftingTable;
}

/**
 * @brief The craft function attempts to craft an item and add the crafted result to the inventory.
 *
 * The craft function looks up the requested recipe, checks nearby station
 * requirements, verifies ingredient availability, removes the required
 * ingredients, creates the correct output item subtype, and adds the crafted
 * item to the Inventory object.
 *
 * @param itemName The itemName parameter represents the name of the item to craft.
 * @param inventory The inventory parameter represents the Inventory object used for ingredient checks and output storage.
 * @param hasCraftingTable The hasCraftingTable parameter indicates whether a CraftingTable is nearby.
 * @param hasCampfire The hasCampfire parameter indicates whether a Campfire is nearby.
 * @return The function returns true if crafting succeeds, and false if crafting fails.
 */
bool CraftingSystem::craft(const std::string& itemName, Inventory& inventory,
                           bool hasCraftingTable, bool hasCampfire) {
    const Recipe* recipe = findRecipe(itemName);
    if (!recipe) {
        std::cout << "[Craft] No recipe for: " << itemName << "\n";
        return false;
    }

    // The function checks whether the required crafting station is available.
    if (itemName == "CookedMeat" && !hasCampfire) {
        std::cout << "[Craft] Need a Campfire nearby.\n";
        return false;
    }
    if (recipe->requiresCraftingTable && !hasCraftingTable) {
        std::cout << "[Craft] Need a CraftingTable nearby.\n";
        return false;
    }

    // The function checks whether all required ingredients are available before consuming anything.
    for (auto& [ingr, qty] : recipe->ingredients) {
        if (!inventory.hasItem(ingr, qty)) {
            std::cout << "[Craft] Missing: " << qty << "x " << ingr << "\n";
            return false;
        }
    }

    // The function removes all recipe ingredients after all checks pass.
    for (auto& [ingr, qty] : recipe->ingredients)
        inventory.removeItem(ingr, qty);

    // The function creates the crafted output item using the correct item subtype.
    std::shared_ptr<Item> output;
    if (recipe->outputType == ItemType::FOOD)
        output = std::make_shared<Food>(recipe->outputName, recipe->outputQty,
                                        recipe->hungerRestore, recipe->healthRestore);
    else if (recipe->outputType == ItemType::TOOL)
        output = std::make_shared<Tool>(recipe->outputName, recipe->outputQty,
                                        recipe->toolType, recipe->bonusDamage,
                                        recipe->mineSpeedMult);
    else
        output = std::make_shared<Block>(recipe->outputName, recipe->outputQty);

    inventory.addItem(output);
    std::cout << "[Craft] Made " << recipe->outputQty << "x "
              << recipe->outputName << "\n";
    return true;
}

/**
 * @brief The getRecipeNames function returns the names of all registered recipes.
 *
 * The getRecipeNames function collects the outputName value from each Recipe
 * object stored in the recipe list.
 *
 * @return The function returns a vector containing the names of all recipes.
 */
const std::vector<Recipe>& CraftingSystem::getRecipes() const { return recipes; }

std::vector<std::string> CraftingSystem::getRecipeNames() const {
    std::vector<std::string> names;
    for (auto& r : recipes) names.push_back(r.outputName);
    return names;
}

/**
 * @brief The printRecipes function prints all registered recipes to the console.
 *
 * The printRecipes function displays recipe output names, output quantities,
 * ingredient requirements, and CraftingTable requirements in a readable format.
 */
void CraftingSystem::printRecipes() const {
    std::cout << "=== Recipes ===\n";
    for (auto& r : recipes) {
        std::cout << (r.requiresCraftingTable ? "[Table] " : "        ")
                  << r.outputQty << "x " << r.outputName << " <- ";

        bool first = true;
        for (auto& [ingr, qty] : r.ingredients) {
            if (!first) std::cout << " + ";
            std::cout << qty << "x " << ingr;
            first = false;
        }
        std::cout << "\n";
    }
}