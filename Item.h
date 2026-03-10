#pragma once
#include <string>

enum class ItemType { FOOD, BLOCK, WEAPON, TOOL, MISC };

class Item {
protected:
    std::string name;
    int         quantity;
    ItemType    type;

public:
    Item(const std::string& name, int quantity, ItemType type)
        : name(name), quantity(quantity), type(type) {}

    virtual ~Item() = default;

    virtual void use() {} ///< Override for usable items (food, weapons, etc.)

    std::string getName()     const { return name; }
    int         getQuantity() const { return quantity; }
    ItemType    getType()     const { return type; }
    bool        isEmpty()     const { return quantity <= 0; }

    void setQuantity(int q)        { quantity = q; }
    void addQuantity(int q)        { quantity += q; }
    bool removeQuantity(int q) {
        if (quantity < q) return false;
        quantity -= q;
        return true;
    }

    virtual std::string serialize() const {
        return name + ":" + std::to_string(quantity) + ":" +
               std::to_string(static_cast<int>(type));
    }

    virtual std::string getDescription() const {
        return name + " x" + std::to_string(quantity);
    }
};
