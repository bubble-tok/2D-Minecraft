/**
 * @file Item.h
 * @brief Defines the Item base class and ItemType enumeration.
 *
 * Item is the polymorphic base for every object that can exist in the
 * player's inventory. Subclasses (Food, Block, Tool) extend it with
 * type-specific behaviour and serialisation.
 *
 * @author Group 46
 */

#pragma once
#include <string>

/**
 * @enum ItemType
 * @brief Functional category of an inventory item.
 *
 * Used by Inventory to route stacking, eating, and placement logic
 * without requiring dynamic_cast on every operation.
 */
enum class ItemType {
    FOOD,   ///< Consumable that restores Hunger and/or HP.
    BLOCK,  ///< Placeable world tile.
    WEAPON, ///< Reserved for future weapon items.
    TOOL,   ///< Equipment that grants stat bonuses (pickaxe, sword, bed, campfire).
    MISC    ///< Uncategorised items (e.g. Gold ore).
};

/**
 * @class Item
 * @brief Abstract base class for all inventory items.
 *
 * Stores the item's name, stack quantity, and type. Provides stacking
 * helpers (addQuantity, removeQuantity) and a virtual serialise interface
 * used by SaveManager when writing the player's inventory to disk.
 *
 * @author Group 46
 */
class Item {
protected:
    std::string name;     ///< Human-readable item identifier (e.g. "Wood", "Apple").
    int         quantity; ///< Number of items in this stack.
    ItemType    type;     ///< Functional category; governs how Inventory handles it.

public:
    /**
     * @brief Constructs an Item with a given name, quantity, and type.
     * @param name     Display name of the item.
     * @param quantity Initial stack size.
     * @param type     Functional category.
     */
    Item(const std::string& name, int quantity, ItemType type)
        : name(name), quantity(quantity), type(type) {}

    /// Virtual destructor — required because subclasses are stored as shared_ptr<Item>.
    virtual ~Item() = default;

    /**
     * @brief Optional use action for usable items.
     *
     * Default implementation is a no-op. Subclasses (e.g. Food) override
     * this to implement consumption behaviour.
     */
    virtual void use() {}

    /**
     * @brief Returns the item's display name.
     * @return Constant reference to the name string.
     */
    std::string getName()     const { return name; }

    /**
     * @brief Returns the current stack quantity.
     * @return Number of items in this stack (>= 0).
     */
    int         getQuantity() const { return quantity; }

    /**
     * @brief Returns the item's functional type.
     * @return ItemType enum value for this item.
     */
    ItemType    getType()     const { return type; }

    /**
     * @brief Returns true if the stack is empty (quantity <= 0).
     * @return True when the item should be removed from its inventory slot.
     */
    bool        isEmpty()     const { return quantity <= 0; }

    /**
     * @brief Directly sets the stack quantity.
     * @param q New quantity (no clamping; caller is responsible).
     */
    void setQuantity(int q) { quantity = q; }

    /**
     * @brief Adds items to this stack.
     * @param q Number of items to add.
     */
    void addQuantity(int q) { quantity += q; }

    /**
     * @brief Removes items from this stack if sufficient quantity exists.
     * @param q Number of items to remove.
     * @return True if removal succeeded; false if quantity was insufficient.
     */
    bool removeQuantity(int q) {
        if (quantity < q) return false;
        quantity -= q;
        return true;
    }

    /**
     * @brief Serialises this item to a colon-delimited string.
     *
     * Format: "name:quantity:typeInt"
     * Subclasses append additional fields (e.g. Food appends hungerRestore).
     *
     * @return Serialised representation for use by SaveManager.
     */
    virtual std::string serialize() const {
        return name + ":" + std::to_string(quantity) + ":" +
               std::to_string(static_cast<int>(type));
    }

    /**
     * @brief Returns a human-readable description of this stack.
     *
     * Subclasses override this to include type-specific details
     * (e.g. Food shows hunger restore value).
     *
     * @return Description string for UI or console display.
     */
    virtual std::string getDescription() const {
        return name + " x" + std::to_string(quantity);
    }
};
