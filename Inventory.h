#pragma once
#include "Item.h"
#include "Food.h"
#include "Block.h"
#include "Hunger.h"
#include "Health.h"
#include <vector>
#include <memory>
#include <iostream>


class Inventory {
private:
    static const int MAX_SLOTS = 36; ///< 9 hotbar + 27 storage
    std::vector<std::shared_ptr<Item>> slots;
    int selectedSlot;

public:
    Inventory() : selectedSlot(0) {
        slots.resize(MAX_SLOTS, nullptr);
    }

  
    bool addItem(std::shared_ptr<Item> item) {
        // Try to stack first
        for (auto& slot : slots) {
            if (slot && slot->getName() == item->getName()
                     && slot->getType() == item->getType()) {
                slot->addQuantity(item->getQuantity());
                return true;
            }
        }
        // Empty slot
        for (auto& slot : slots) {
            if (!slot) { slot = item; return true; }
        }
        std::cout << "[Inventory] Full! Cannot add " << item->getName() << "\n";
        return false;
    }

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


    void selectSlot(int index) {
        if (index >= 0 && index < MAX_SLOTS) selectedSlot = index;
    }

    int getSelectedSlot() const { return selectedSlot; }

    std::shared_ptr<Item> getSelectedItem() const {
        return slots[selectedSlot];
    }

    std::shared_ptr<Item> getSlot(int i) const {
        if (i >= 0 && i < MAX_SLOTS) return slots[i];
        return nullptr;
    }


    bool hasItem(const std::string& name, int qty = 1) const {
        for (auto& slot : slots)
            if (slot && slot->getName() == name && slot->getQuantity() >= qty)
                return true;
        return false;
    }

    int  getMaxSlots() const { return MAX_SLOTS; }
    bool isFull()      const {
        for (auto& s : slots) if (!s) return false;
        return true;
    }


    void print() const {
        std::cout << "=== Inventory ===\n";
        bool any = false;
        for (int i = 0; i < MAX_SLOTS; ++i) {
            if (slots[i]) {
                std::cout << (i == selectedSlot ? " >[" : "  [")
                          << i << "] " << slots[i]->getDescription() << "\n";
                any = true;
            }
        }
        if (!any) std::cout << "  (empty)\n";
        std::cout << "=================\n";
    }


    std::vector<std::string> serialize() const {
        std::vector<std::string> data;
        for (auto& slot : slots)
            if (slot) data.push_back(slot->serialize());
        return data;
    }

    const std::vector<std::shared_ptr<Item>>& getSlots() const { return slots; }
};
