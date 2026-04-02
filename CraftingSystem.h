/**
 * @file CraftingSystem.h
 * @brief The CraftingSystem.h file declares the Recipe structure and the CraftingSystem class.
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
 * @brief The Recipe structure stores the data that describes one craftable item.
 *
 * The Recipe structure stores the crafted output, ingredient requirements,
 * optional food values, optional tool values, and crafting-station requirements.
 * The CraftingSystem class registers Recipe objects in the constructor and uses
 * the Recipe data for both recipe display and recipe execution.
 *
 * @author Group 46
 */
struct Recipe {
    std::string outputName;   ///< The outputName variable stores the name of the crafted item.
    int         outputQty;    ///< The outputQty variable stores the number of items produced per craft.
    ItemType    outputType;   ///< The outputType variable determines which Item subclass is created.
    int         hungerRestore = 0; ///< The hungerRestore variable stores the hunger restored by Food output.
    int         healthRestore = 0; ///< The healthRestore variable stores the health restored by Food output.
    std::map<std::string, int> ingredients; ///< The ingredients variable stores the required item names and quantities.
    ToolType    toolType      = ToolType::None; ///< The toolType variable stores the tool subtype for Tool output.
    int         bonusDamage   = 0;   ///< The bonusDamage variable stores the attack bonus for Sword output.
    float       mineSpeedMult = 1.f; ///< The mineSpeedMult variable stores the mining speed multiplier for Pickaxe output.
    bool        requiresCraftingTable = false; ///< The requiresCraftingTable variable indicates whether a CraftingTable must be nearby.
};

/**
 * @class CraftingSystem
 * @brief The CraftingSystem class stores all recipes and executes crafting actions using a player inventory.
 *
 * The CraftingSystem class registers all recipes in the constructor. The
 * craft function checks station requirements and ingredient availability before
 * consuming the required inputs and producing the output item.
 *
 * The CraftingSystem class supports hand-craftable recipes such as WoodPlank,
 * Stick, StoneBrick, GoldenApple, Bed, Campfire, and CraftingTable.
 *
 * The CraftingSystem class supports CraftingTable recipes such as
 * WoodenPickaxe, StonePickaxe, WoodenSword, and StoneSword.
 *
 * The CraftingSystem class supports Campfire recipes such as CookedMeat.
 *
 * @author Group 46
 */
class CraftingSystem {
public:
    /**
     * @brief The constructor initializes the CraftingSystem object and registers all game recipes.
     */
    CraftingSystem();

    /**
     * @brief The craft function attempts to craft the named item using ingredients from the inventory.
     *
     * The craft function checks whether the recipe exists, whether the required
     * crafting station is available, and whether all required ingredients are
     * present in sufficient quantity. The craft function removes ingredients and
     * adds the crafted output item to the inventory when all checks pass.
     *
     * @param itemName The itemName parameter represents the name of the recipe to craft.
     * @param inventory The inventory parameter represents the player inventory used to remove ingredients and store the crafted item.
     * @param hasCraftingTable The hasCraftingTable parameter indicates whether a CraftingTable is within range.
     * @param hasCampfire The hasCampfire parameter indicates whether a Campfire is within range.
     * @return The function returns true if crafting succeeds, and false if any validation step fails.
     */
    bool craft(const std::string& itemName, Inventory& inventory,
               bool hasCraftingTable = false, bool hasCampfire = false);

    /**
     * @brief The requiresTable function checks whether the named recipe requires a CraftingTable nearby.
     *
     * @param itemName The itemName parameter represents the recipe name to check.
     * @return The function returns true if the recipe requires a CraftingTable.
     */
    bool requiresTable(const std::string& itemName) const;

    /**
     * @brief The getRecipes function returns a constant reference to the full recipe list.
     *
     * The getRecipes function allows other systems, such as the crafting UI,
     * to iterate through and display all available recipes.
     *
     * @return The function returns a reference to the internal recipe vector.
     */
    const std::vector<Recipe>& getRecipes() const { return recipes; }

    /**
     * @brief The getRecipeNames function returns a vector containing the names of all recipe outputs.
     *
     * @return The function returns a vector of recipe name strings in registration order.
     */
    std::vector<std::string> getRecipeNames() const;

    /**
     * @brief The printRecipes function prints all recipes to standard output for debugging.
     */
    void printRecipes() const;

private:
    std::vector<Recipe> recipes; ///< The recipes variable stores all registered recipes in display order.

    /**
     * @brief The findRecipe function searches for a recipe by output name.
     *
     * @param name The name parameter represents the recipe output name to search for.
     * @return The function returns a pointer to the matching Recipe object, or nullptr if no matching recipe exists.
     */
    const Recipe* findRecipe(const std::string& name) const;
};