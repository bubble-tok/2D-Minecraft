/**
 * @file CraftingSystem.cpp
 * @brief Implements recipe registration and the craft execution pipeline.
 *
 * @author Group 46
 */
#include "CraftingSystem.h"
#include <iostream>

CraftingSystem::CraftingSystem() {
    // Recipes that do not require a CraftingTable
    recipes.push_back({"WoodPlank",    4, ItemType::BLOCK, 0, 0, {{"Wood",1}}});
    recipes.push_back({"Stick",        4, ItemType::MISC,  0, 0, {{"WoodPlank",2}}});
    recipes.push_back({"StoneBrick",   4, ItemType::BLOCK, 0, 0, {{"Stone",4}}});
    recipes.push_back({"GoldenApple",  1, ItemType::FOOD, 20,20, {{"Apple",1},{"Gold",1}}});
    recipes.push_back({"Bed",          1, ItemType::TOOL,  0, 0, {{"WoodPlank",3}},
                        ToolType::Bed, 0, 1.f, false});
    recipes.push_back({"Campfire",     1, ItemType::TOOL,  0, 0, {{"Wood",3},{"Stone",2}},
                        ToolType::Campfire, 0, 1.f, false});
    recipes.push_back({"CraftingTable",1, ItemType::BLOCK, 0, 0, {{"WoodPlank",4}}});

    // Requires CraftingTable nearby
    recipes.push_back({"WoodenPickaxe",1, ItemType::TOOL, 0, 0, {{"WoodPlank",3},{"Stick",2}},
                        ToolType::Pickaxe, 0, 0.6f, true});
    recipes.push_back({"StonePickaxe", 1, ItemType::TOOL, 0, 0, {{"StoneBrick",3},{"Stick",2}},
                        ToolType::Pickaxe, 0, 0.4f, true});
    recipes.push_back({"WoodenSword",  1, ItemType::TOOL, 0, 0, {{"WoodPlank",2},{"Stick",1}},
                        ToolType::Sword, 5, 1.f, true});
    recipes.push_back({"StoneSword",   1, ItemType::TOOL, 0, 0, {{"StoneBrick",2},{"Stick",1}},
                        ToolType::Sword, 12, 1.f, true});

    // Requires Campfire nearby
    recipes.push_back({"CookedMeat",   1, ItemType::FOOD, 40, 5, {{"RawMeat",1}},
                        ToolType::None, 0, 1.f, false});
}

const Recipe* CraftingSystem::findRecipe(const std::string& name) const {
    for (auto& r : recipes)
        if (r.outputName == name) return &r;
    return nullptr; // not found
}

bool CraftingSystem::requiresTable(const std::string& name) const {
    const Recipe* r = findRecipe(name);
    return r && r->requiresCraftingTable;
}

bool CraftingSystem::craft(const std::string& itemName, Inventory& inventory,
                           bool hasCraftingTable, bool hasCampfire) {
    const Recipe* recipe = findRecipe(itemName);
    if (!recipe) {
        std::cout << "[Craft] No recipe for: " << itemName << "\n";
        return false;
    }

    // Station checks (either a Campfire or CraftingTable)
    if (itemName == "CookedMeat" && !hasCampfire) {
        std::cout << "[Craft] Need a Campfire nearby.\n";
        return false;
    }
    if (recipe->requiresCraftingTable && !hasCraftingTable) {
        std::cout << "[Craft] Need a CraftingTable nearby.\n";
        return false;
    }

    // Ingredient availability check — all must pass before any are consumed
    for (auto& [ingr, qty] : recipe->ingredients) {
        if (!inventory.hasItem(ingr, qty)) {
            std::cout << "[Craft] Missing: " << qty << "x " << ingr << "\n";
            return false;
        }
    }

    // Consume all ingredients on successful crafting
    for (auto& [ingr, qty] : recipe->ingredients)
        inventory.removeItem(ingr, qty);

    // Produce the output item with the correct subtype
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

std::vector<std::string> CraftingSystem::getRecipeNames() const {
    std::vector<std::string> names;
    for (auto& r : recipes) names.push_back(r.outputName);
    return names;
}

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
