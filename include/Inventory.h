/**
 * @file Inventory.h
 * @brief Declares the Inventory class for managing the player's item slots.
 *
 * @author Group 46
 */

#pragma once
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include "Hunger.h"
#include "Health.h"
#include <vector>
#include <memory>

/**
 * @class Inventory
 * @brief Manages 36 item slots (9 hotbar + 27 storage) for the player.
 *
 * Items are stored as shared_ptr<Item> so Food, Block, and Tool subclasses
 * are handled polymorphically. addItem() stacks matching items by name and
 * type before falling back to an empty slot. The selectedSlot tracks which
 * hotbar slot is active for eating, placing, and tool bonuses.
 *
 * @author Group 46
 */
class Inventory {
public:
    static const int MAX_SLOTS = 36; ///< Total slot count (9 hotbar + 27 storage).

    /**
     * @brief Constructs an empty Inventory with all slots set to nullptr.
     */
    Inventory();

    /**
     * @brief Attempts to add an item to the inventory.
     *
     * First tries to stack the item onto an existing slot with the same name
     * and type. If no matching slot exists, places it in the first empty slot.
     *
     * @param item Shared pointer to the item to add.
     * @return True if the item was added or stacked; false if all slots are full.
     */
    bool addItem(std::shared_ptr<Item> item);

    /**
     * @brief Removes a quantity of the named item from the inventory.
     *
     * Searches all slots for the first matching name and removes qty units.
     * If the slot becomes empty it is set to nullptr.
     *
     * @param name Item name to search for.
     * @param qty  Number of units to remove. Defaults to 1.
     * @return True if the removal succeeded; false if no matching item was found.
     */
    bool removeItem(const std::string& name, int qty = 1);

    /**
     * @brief Eats the food item in the currently selected slot.
     *
     * Fails if the selected slot is empty or not a Food item.
     * On success, calls Food::consume() to apply hunger/health restoration
     * and clears the slot if the stack is depleted.
     *
     * @param hunger Reference to the player's Hunger to restore.
     * @param health Reference to the player's Health to restore.
     * @return True if food was consumed; false otherwise.
     */
    bool eatSelected(Hunger& hunger, Health& health);

    /**
     * @brief Changes the currently active hotbar slot.
     * @param index New slot index in the range [0, MAX_SLOTS-1].
     */
    void selectSlot(int index);

    /**
     * @brief Returns the index of the currently selected hotbar slot.
     * @return Selected slot index (0–8 for hotbar slots).
     */
    int  getSelectedSlot() const;

    /**
     * @brief Returns the item in the currently selected slot, or nullptr.
     * @return Shared pointer to the selected item, or nullptr if the slot is empty.
     */
    std::shared_ptr<Item> getSelectedItem() const;

    /**
     * @brief Returns the item at the given slot index, or nullptr.
     * @param i Slot index in the range [0, MAX_SLOTS-1].
     * @return Shared pointer to the item, or nullptr if out-of-range or empty.
     */
    std::shared_ptr<Item> getSlot(int i) const;

    /**
     * @brief Returns true if the inventory contains at least qty of the named item.
     * @param name Item name to search for.
     * @param qty  Minimum quantity required. Defaults to 1.
     * @return True if a slot holds the item with at least qty units.
     */
    bool hasItem(const std::string& name, int qty = 1) const;

    /**
     * @brief Returns true when every slot is occupied.
     * @return True if no empty slots remain.
     */
    bool isFull() const;

    /**
     * @brief Empties all slots by setting them to nullptr.
     *
     * Used by SaveManager::load() before restoring the saved inventory.
     */
    void clear();

    /**
     * @brief Returns the total slot capacity.
     * @return Always MAX_SLOTS (36).
     */
    int  getMaxSlots() const;

    /// Prints the inventory contents to stdout for debugging.
    void print() const;

    /**
     * @brief Serialises the inventory to a vector of strings for saving.
     *
     * Each non-null slot is serialised via Item::serialize() and appended.
     * Empty slots are omitted.
     *
     * @return Vector of serialised item strings.
     */
    std::vector<std::string> serialize() const;

    /**
     * @brief Returns read-only access to the raw slot vector.
     * @return Const reference to the internal slot array.
     */
    const std::vector<std::shared_ptr<Item>>& getSlots() const;

private:
    std::vector<std::shared_ptr<Item>> slots; ///< The 36 item slots.
    int selectedSlot;                          ///< Currently active hotbar slot index.
};
