/**
 * @file Item.cpp
 * @brief Implements the Item base class for all inventory items.
 *
 * @author Group 46
 */
#include "Item.h"

Item::Item(const std::string& name, int quantity, ItemType type)
    : name(name), quantity(quantity), type(type) {}

void Item::use() {}

std::string Item::getName()     const { return name; }

int         Item::getQuantity() const { return quantity; }

ItemType    Item::getType()     const { return type; }

bool        Item::isEmpty()     const { return quantity <= 0; }

void Item::setQuantity(int q) { quantity = q; }

void Item::addQuantity(int q) { quantity += q; }

bool Item::removeQuantity(int q) {
    if (quantity < q) return false;
    quantity -= q;
    return true;
}

std::string Item::serialize() const {
    return name + ":" + std::to_string(quantity) + ":" +
           std::to_string(static_cast<int>(type));
}

std::string Item::getDescription() const {
    return name + " x" + std::to_string(quantity);
}
