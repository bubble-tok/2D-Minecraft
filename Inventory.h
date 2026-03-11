/**
 * @file Inventory.h
 * @brief Defines the Inventory class used to store and manage player items.
 */

#pragma once
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include "Hunger.h"
#include "Health.h"
#include <vector>
#include <memory>
#include <iostream>

/**
 * @class Inventory
 * @brief Manages a collection of items stored by the player.
 *
 * The inventory contains a fixed number of slots (36 total) consisting of
 * a hotbar and storage area. Items can stack if they share the same type
 * and name. The inventory also supports consuming food, selecting hotbar
 * slots, and serialization for saving game state.
 */
class Inventory {
private:
    static const int MAX_SLOTS = 36; ///< Total number of inventory slots (9 hotbar + 27 storage)

    std::vector<std::shared_ptr<Item>> slots; ///< Container storing inventory items
    int selectedSlot; ///< Currently selected hotbar slot

public:

    /**
     * @brief Constructs an empty inventory.
     *
     * Initializes all inventory slots to nullptr and selects slot 0.
     */
    Inventory() : selectedSlot(0) {
        slots.resize(MAX_SLOTS, nullptr);
    }

    /**
     * @brief Adds an item to the inventory.
     *
     * Attempts to stack the item with an existing stack first. If stacking
     * is not possible, the item is placed into the first available empty slot.
     *
     * @param item Shared pointer to the item being added
     * @return True if the item was successfully added, false if the inventory is full
     */
    bool addItem(std::shared_ptr<Item> item) {

        /// Attempt to stack with existing items
        for (auto& slot : slots) {
            if (slot && slot->getName() == item->getName()
                     && slot->getType() == item->getType()) {
                slot->addQuantity(item->getQuantity());
                return true;
            }
        }

        /// Find an empty slot
        for (auto& slot : slots) {
            if (!slot) {
                slot = item;
                return true;
            }
        }

        std::cout << "[Inventory] Full! Cannot add " << item->getName() << "\n";
        return false;
    }

    /**
     * @brief Removes a specified quantity of an item from the inventory.
     *
     * @param name Name of the item to remove
     * @param qty Quantity to remove (default = 1)
     * @return True if the item was successfully removed
     */
    bool removeItem(const std::string& name, int qty = 1) {
        for (auto& slot : slots) {
            if (slot && slot->getName() == name) {
                if (slot->removeQuantity(qty)) {
                    if (slot->isEmpty()) slot = nullptr;
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Consumes food from the currently selected slot.
     *
     * If the selected item is food, its effects are applied to the
     * provided Hunger and Health systems.
     *
     * @param hunger Reference to the player's Hunger system
     * @param health Reference to the player's Health system
     * @return True if food was successfully consumed
     */
    bool eatSelected(Hunger& hunger, Health& health) {
        auto& slot = slots[selectedSlot];

        if (!slot || slot->getType() != ItemType::FOOD) {
            std::cout << "[Inventory] No food in selected slot " << selectedSlot << ".\n";
            return false;
        }

        Food* food = dynamic_cast<Food*>(slot.get());
        if (!food) return false;

        bool ate = food->consume(hunger, health);

        if (slot->isEmpty()) slot = nullptr;

        return ate;
    }

    /**
     * @brief Selects an inventory slot.
     *
     * @param index Slot index to select
     */
    void selectSlot(int index) {
        if (index >= 0 && index < MAX_SLOTS)
            selectedSlot = index;
    }

    /**
     * @brief Gets the currently selected slot index.
     * @return Selected slot index
     */
    int getSelectedSlot() const { return selectedSlot; }

    /**
     * @brief Gets the item in the currently selected slot.
     * @return Shared pointer to the selected item
     */
    std::shared_ptr<Item> getSelectedItem() const {
        return slots[selectedSlot];
    }

    /**
     * @brief Retrieves the item in a specific slot.
     *
     * @param i Slot index
     * @return Shared pointer to the item or nullptr if invalid
     */
    std::shared_ptr<Item> getSlot(int i) const {
        if (i >= 0 && i < MAX_SLOTS)
            return slots[i];
        return nullptr;
    }

    /**
     * @brief Checks if the inventory contains a specific item.
     *
     * @param name Item name
     * @param qty Required quantity
     * @return True if the item exists in sufficient quantity
     */
    bool hasItem(const std::string& name, int qty = 1) const {
        for (auto& slot : slots)
            if (slot && slot->getName() == name && slot->getQuantity() >= qty)
                return true;
        return false;
    }

    /**
     * @brief Gets the maximum number of slots in the inventory.
     * @return Maximum slot count
     */
    int getMaxSlots() const { return MAX_SLOTS; }

    /**
     * @brief Checks whether the inventory is full.
     * @return True if no empty slots remain
     */
    bool isFull() const {
        for (auto& s : slots)
            if (!s) return false;
        return true;
    }

    /**
     * @brief Prints the inventory contents to the console.
     *
     * Displays each occupied slot and highlights the selected slot.
     */
    void print() const {
        std::cout << "=== Inventory ===\n";

        bool any = false;

        for (int i = 0; i < MAX_SLOTS; ++i) {
            if (slots[i]) {
                std::cout << (i == selectedSlot ? " >[" : "  [")
                          << i << "] "
                          << slots[i]->getDescription() << "\n";
                any = true;
            }
        }

        if (!any)
            std::cout << "  (empty)\n";

        std::cout << "=================\n";
    }

    /**
     * @brief Serializes inventory items for saving.
     *
     * @return Vector of serialized item strings
     */
    std::vector<std::string> serialize() const {
        std::vector<std::string> data;

        for (auto& slot : slots)
            if (slot)
                data.push_back(slot->serialize());

        return data;
    }

    /**
     * @brief Gets a reference to the internal slot container.
     * @return Const reference to the slot vector
     */
    const std::vector<std::shared_ptr<Item>>& getSlots() const {
        return slots;
    }
};