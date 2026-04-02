/**
 * @file Inventory.cpp
 * @brief Implements Inventory slot management, stacking, eating, and serialisation.
 *
 * @author Group 46
 */

#include "Inventory.h"
#include <iostream>

Inventory::Inventory() : selectedSlot(0) {
    // Initialise all 36 slots to nullptr (empty)
    slots.resize(MAX_SLOTS, nullptr);
}

bool Inventory::addItem(std::shared_ptr<Item> item) {
    // First pass: try to stack onto an existing slot with matching name and type.
    // This ensures items of the same kind are combined rather than taking a new slot.
    for (auto& slot : slots) {
        if (slot && slot->getName() == item->getName()
                 && slot->getType() == item->getType()) {
            slot->addQuantity(item->getQuantity());
            return true;
        }
    }

    // Second pass: place in the first empty slot
    for (auto& slot : slots) {
        if (!slot) { slot = item; return true; }
    }

    // All slots occupied — item cannot be added
    std::cout << "[Inventory] Full — cannot add " << item->getName() << "\n";
    return false;
}

bool Inventory::removeItem(const std::string& name, int qty) {
    for (auto& slot : slots) {
        if (slot && slot->getName() == name) {
            if (slot->removeQuantity(qty)) {
                if (slot->isEmpty()) slot = nullptr; // clear the slot when depleted
                return true;
            }
        }
    }
    return false; // item not found or insufficient quantity
}

bool Inventory::eatSelected(Hunger& hunger, Health& health) {
    auto& slot = slots[selectedSlot];
    // Guard: slot must exist and be a food item
    if (!slot || slot->getType() != ItemType::FOOD) return false;

    // Dynamic cast to access Food::consume()
    Food* food = dynamic_cast<Food*>(slot.get());
    if (!food) return false;

    bool ate = food->consume(hunger, health);
    if (slot->isEmpty()) slot = nullptr; // clear slot when stack is exhausted
    return ate;
}

void Inventory::selectSlot(int index) {
    if (index >= 0 && index < MAX_SLOTS) selectedSlot = index;
}

int Inventory::getSelectedSlot() const { return selectedSlot; }

std::shared_ptr<Item> Inventory::getSelectedItem() const {
    return slots[selectedSlot];
}

std::shared_ptr<Item> Inventory::getSlot(int i) const {
    if (i >= 0 && i < MAX_SLOTS) return slots[i];
    return nullptr;
}

bool Inventory::hasItem(const std::string& name, int qty) const {
    for (auto& slot : slots)
        if (slot && slot->getName() == name && slot->getQuantity() >= qty)
            return true;
    return false;
}

bool Inventory::isFull() const {
    for (auto& s : slots) if (!s) return false;
    return true;
}

void Inventory::clear() {
    for (auto& slot : slots) slot = nullptr;
}

void Inventory::print() const {
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

std::vector<std::string> Inventory::serialize() const {
    std::vector<std::string> data;
    for (auto& slot : slots)
        if (slot) data.push_back(slot->serialize()); // skip empty slots
    return data;
}

const std::vector<std::shared_ptr<Item>>& Inventory::getSlots() const {
    return slots;
}
