/**
 * @file Player.cpp
 * @brief Implements Player combat, inventory interaction, and tool stat queries.
 *
 * @author Group 46
 */
#include "Player.h"
#include <cmath>
#include <iostream>

Player::Player(float x, float y) : Entity(x, y, 100) {}

bool Player::attack(Entity& target, int totalDamage) {
    float dx   = target.getX() - this->x;
    float dy   = target.getY() - this->y;
    // Only attack if target is within melee range
    if (std::sqrt(dx * dx + dy * dy) > attackRange) return false;
    target.takeDamage(totalDamage); // respects target's invincible flag
    return true;
}

bool Player::pickUp(std::shared_ptr<Item> item) {
    bool ok = inventory.addItem(item);
    if (ok) std::cout << "[Player] Picked up " << item->getName() << "\n";
    return ok;
}

bool Player::eat() {
    auto sel = inventory.getSelectedItem();
    // Block raw meat — it must be cooked at a Campfire first
    if (sel && sel->getName() == "RawMeat") {
        std::cout << "[Player] RawMeat must be cooked first!\n";
        return false;
    }
    bool ok = inventory.eatSelected(hunger, health);
    if (ok) std::cout << "[Player] Ate. Hunger: " << hunger.getLevel() << "\n";
    return ok;
}

Tool* Player::getEquippedTool() const {
    auto item = inventory.getSelectedItem();
    if (!item) return nullptr;
    // Dynamic cast — returns nullptr if the selected item is not a Tool
    return dynamic_cast<Tool*>(item.get());
}

int Player::getTotalAttackDamage() const {
    int dmg = baseAttackDamage; // start with unarmed damage
    Tool* t = getEquippedTool();
    // Add sword bonus if a sword is equipped
    if (t && t->getToolType() == ToolType::Sword)
        dmg += t->getBonusDamage();
    return dmg;
}

float Player::getMineSpeedMult() const {
    Tool* t = getEquippedTool();
    // Return pickaxe/axe multiplier if equipped; otherwise no bonus (1.0)
    if (t && (t->getToolType() == ToolType::Pickaxe ||
              t->getToolType() == ToolType::Axe))
        return t->getMineSpeedMult();
    return 1.f;
}

Inventory& Player::getInventory()             { return inventory; }
const Inventory& Player::getInventory() const  { return inventory; }
Hunger&    Player::getHunger()                 { return hunger; }
Sleep&     Player::getSleep()                  { return sleep; }

int   Player::getHungerLevel() const { return hunger.getLevel(); }
int   Player::getSleepLevel()  const { return sleep.getLevel(); }
int   Player::getHealth()      const { return health.getHp(); }
int   Player::getHp()          const { return health.getHp(); }

float Player::getPositionX()   const { return x; }
float Player::getPositionY()   const { return y; }
void  Player::setPosition(float nx, float ny) { x = nx; y = ny; }
void  Player::setHealth(int h)  { health.setHp(h); }
void  Player::setHunger(int h)  { hunger.setLevel(h); }
void  Player::setSleep(int s)   { sleep.setLevel(s); }

void Player::printStatus() const {
    std::cout << "HP:" << health.getHp()
              << " Hunger:" << hunger.getLevel()
              << " Sleep:"  << sleep.getLevel()
              << " Pos:(" << x << "," << y << ")\n";
    inventory.print();
}
