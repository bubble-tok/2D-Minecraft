/**
 * @file Item.h
 * @brief Defines the Item base class and ItemType enumeration.
 *
 * @author Group 46
 */

#pragma once
#include <string>

/**
 * @enum ItemType
 * @brief Functional category of an inventory item.
 */
enum class ItemType {
    FOOD,
    BLOCK,
    WEAPON,
    TOOL,
    MISC
};

/**
 * @class Item
 * @brief Abstract base class for all inventory items.
 *
 * @author Group 46
 */
class Item {
protected:
    std::string name;     ///< Human-readable item identifier.
    int         quantity; ///< Number of items in this stack.
    ItemType    type;     ///< Functional category.

public:
    /**
     * @brief Constructs an Item with a given name, quantity, and type.
     */
    Item(const std::string& name, int quantity, ItemType type);

    virtual ~Item() = default;

    /** @brief Optional use action for usable items. Default is a no-op. */
    virtual void use();

    /** @brief Returns the item's display name. */
    std::string getName()     const;

    /** @brief Returns the current stack quantity. */
    int         getQuantity() const;

    /** @brief Returns the item's functional type. */
    ItemType    getType()     const;

    /** @brief Returns true if the stack is empty (quantity <= 0). */
    bool        isEmpty()     const;

    /** @brief Directly sets the stack quantity. */
    void setQuantity(int q);

    /** @brief Adds items to this stack. */
    void addQuantity(int q);

    /**
     * @brief Removes items from this stack if sufficient quantity exists.
     * @return True if removal succeeded; false if quantity was insufficient.
     */
    bool removeQuantity(int q);

    /**
     * @brief Serialises this item to a colon-delimited string.
     * Format: "name:quantity:typeInt"
     */
    virtual std::string serialize() const;

    /** @brief Returns a human-readable description of this stack. */
    virtual std::string getDescription() const;
};
