/**
 * @file CraftingSystem.h
 * @brief Declares the Recipe struct and CraftingSystem class.
 *
 * @author Group 46
 */
#pragma once
#include "Inventory.h"
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include "Tool.h"
#include <map>
#include <vector>
#include <string>

/**
 * @struct Recipe
 * @brief Data describing a single craftable item.
 *
 * Recipes are registered in CraftingSystem's constructor and queried by
 * both the crafting UI (for display) and Game::requestCraft() (for execution).
 *
 * @author Group 46
 */
struct Recipe {
    std::string outputName;   ///< Name of the crafted item.
    int         outputQty;    ///< Number of items produced per craft.
    ItemType    outputType;   ///< Determines which Item subclass is created.
    int         hungerRestore = 0; ///< Hunger restored (Food only).
    int         healthRestore = 0; ///< HP restored (Food only).
    std::map<std::string, int> ingredients; ///< Required item name → quantity.
    ToolType    toolType      = ToolType::None; ///< Tool sub-type (Tool output only).
    int         bonusDamage   = 0;   ///< Attack bonus (Sword only).
    float       mineSpeedMult = 1.f; ///< Mining speed multiplier (Pickaxe only).
    bool        requiresCraftingTable = false; ///< True if a CraftingTable must be nearby.
};

/**
 * @class CraftingSystem
 * @brief Owns all recipes and executes crafts against a player inventory.
 *
 * All recipes are registered in the constructor. The craft() method validates
 * station requirements and ingredient availability before consuming inputs and
 * producing the output item.
 *
 * Hand-craftable recipes (no station required): WoodPlank, Stick, StoneBrick,
 * GoldenApple, Bed, Campfire, CraftingTable.
 *
 * CraftingTable recipes: WoodenPickaxe, StonePickaxe, WoodenSword, StoneSword.
 *
 * Campfire recipes: CookedMeat (requires Campfire nearby).
 *
 * @author Group 46
 */
class CraftingSystem {
public:
    /**
     * @brief Constructs CraftingSystem and registers all game recipes.
     */
    CraftingSystem();

    /**
     * @brief Attempts to craft the named item using ingredients from the inventory.
     *
     * Validates:
     * 1. The recipe exists.
     * 2. Station requirements (CraftingTable / Campfire nearby).
     * 3. All ingredients are present in sufficient quantity.
     *
     * On success, removes ingredients and adds the output item to the inventory.
     *
     * @param itemName         Name of the recipe to craft.
     * @param inventory        Player inventory to deduct ingredients from and add output to.
     * @param hasCraftingTable True if a CraftingTable is within range.
     * @param hasCampfire      True if a Campfire is within range.
     * @return True if crafting succeeded; false if any validation step failed.
     */
    bool craft(const std::string& itemName, Inventory& inventory,
               bool hasCraftingTable = false, bool hasCampfire = false);

    /**
     * @brief Returns true if the named recipe requires a CraftingTable nearby.
     * @param itemName Recipe name to check.
     * @return True if requiresCraftingTable is set for this recipe.
     */
    bool requiresTable(const std::string& itemName) const;

    /**
     * @brief Returns a const reference to the full recipe list.
     *
     * Used by CraftingUI to iterate and display all available recipes.
     *
     * @return Reference to the internal recipe vector.
     */
    const std::vector<Recipe>& getRecipes() const { return recipes; }

    /**
     * @brief Returns a vector of all recipe output names.
     * @return Vector of name strings in registration order.
     */
    std::vector<std::string> getRecipeNames() const;

    /// Prints all recipes to stdout for debugging.
    void printRecipes() const;

private:
    std::vector<Recipe> recipes; ///< All registered recipes in display order.

    /**
     * @brief Finds a recipe by output name.
     * @param name Recipe output name to search for.
     * @return Pointer to the matching Recipe, or nullptr if not found.
     */
    const Recipe* findRecipe(const std::string& name) const;
};
