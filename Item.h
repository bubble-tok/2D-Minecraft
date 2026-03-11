/**
 * @file Item.h
 * @brief Defines the base Item class and item type enumeration used by the inventory system.
 */

#pragma once
#include <string>

/**
 * @enum ItemType
 * @brief Represents the category of an item.
 *
 * Item types are used to determine item behavior and how
 * they interact with other systems such as inventory, crafting,
 * and gameplay mechanics.
 */
enum class ItemType {
    FOOD,   ///< Consumable item that restores hunger or health
    BLOCK,  ///< Placeable block item used in the world
    WEAPON, ///< Item used for combat
    TOOL,   ///< Utility item used for actions such as mining
    MISC    ///< Miscellaneous item type
};

/**
 * @class Item
 * @brief Base class representing any item in the game.
 *
 * The Item class stores common properties such as name, quantity,
 * and item type. Specialized items such as Food, Block, or Tools
 * inherit from this class and override behavior such as `use()`
 * or `getDescription()`.
 */
class Item {
protected:
    std::string name; ///< Name of the item
    int quantity;     ///< Number of items in the stack
    ItemType type;    ///< Category of the item

public:

    /**
     * @brief Constructs an Item.
     *
     * @param name Name of the item
     * @param quantity Quantity of items in the stack
     * @param type Type of the item
     */
    Item(const std::string& name, int quantity, ItemType type)
        : name(name), quantity(quantity), type(type) {}

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~Item() = default;

    /**
     * @brief Uses the item.
     *
     * This function is intended to be overridden by derived classes
     * such as Food or Weapon to define item-specific behavior.
     */
    virtual void use() {}

    /**
     * @brief Gets the item name.
     * @return Name of the item
     */
    std::string getName() const { return name; }

    /**
     * @brief Gets the quantity of the item stack.
     * @return Quantity value
     */
    int getQuantity() const { return quantity; }

    /**
     * @brief Gets the item type.
     * @return ItemType enumeration value
     */
    ItemType getType() const { return type; }

    /**
     * @brief Checks whether the item stack is empty.
     * @return True if quantity is zero or less
     */
    bool isEmpty() const { return quantity <= 0; }

    /**
     * @brief Sets the item quantity.
     *
     * @param q New quantity value
     */
    void setQuantity(int q) { quantity = q; }

    /**
     * @brief Adds items to the stack.
     *
     * @param q Quantity to add
     */
    void addQuantity(int q) { quantity += q; }

    /**
     * @brief Removes items from the stack.
     *
     * @param q Quantity to remove
     * @return True if removal succeeded, false if insufficient quantity
     */
    bool removeQuantity(int q) {
        if (quantity < q) return false;
        quantity -= q;
        return true;
    }

    /**
     * @brief Serializes the item into a string format.
     *
     * Used for saving and loading item data.
     *
     * @return Serialized string containing item name, quantity, and type
     */
    virtual std::string serialize() const {
        return name + ":" + std::to_string(quantity) + ":" +
               std::to_string(static_cast<int>(type));
    }

    /**
     * @brief Returns a human-readable description of the item.
     *
     * Used for displaying item information in inventory UI.
     *
     * @return Description string
     */
    virtual std::string getDescription() const {
        return name + " x" + std::to_string(quantity);
    }
};