#pragma once
#include "Entity.h"
#include "Inventory.h"
#include "Hunger.h"
#include "Sleep.h"
#include "Food.h"
#include "Block.h"
#include <iostream>
#include <cmath>


class Player : public Entity {
private:
    Inventory inventory;
    Hunger    hunger;
    Sleep     sleep;

    // Physics
    float velocityY;
    bool  onGround;
    static constexpr float GRAVITY      = 800.f;
    static constexpr float JUMP_FORCE   = -400.f;
    static constexpr float MOVE_SPEED   = 150.f;
    static constexpr float GROUND_Y     = 400.f; ///< Placeholder ground level

    // Combat
    int   attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;

    // Block mining
    float mineTimer;        ///< Accumulated time holding mouse on a block
    float mineTimeRequired; ///< Seconds needed to break a block

public:
    Player(float x = 0.f, float y = 0.f)
        : Entity(x, y, 100),
          velocityY(0.f), onGround(false),
          attackDamage(10), attackRange(80.f),
          attackCooldown(0.5f), attackTimer(0.f),
          mineTimer(0.f), mineTimeRequired(1.5f) {}

    void update(float deltaTime) override {
        // Cooldowns - attack cooldown is managed by main.cpp, skip here
        // Hunger decay is also handled by main.cpp hunger timer, skip here
    }


    /// Move left (-1) or right (+1). Sleep deprivation slows movement by 50%.
    void move(int direction, float deltaTime) {
        float speed = MOVE_SPEED * (sleep.isEmpty() ? 0.5f : 1.0f);
        x += direction * speed * deltaTime;
    }

    /// Jump if standing on the ground.
    void jump() {
        if (onGround) {
            velocityY = JUMP_FORCE;
            onGround  = false;
        }
    }

   
    bool attack(Entity& target) {
        // Cooldown is managed externally in main.cpp via attackTimer
        float dx = target.getX() - x;
        float dy = target.getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > attackRange) return false;
        target.takeDamage(attackDamage);
        std::cout << "[Player] Hit " << (target.isAlive() ? "entity" : "entity (killed!)")
            << " for " << attackDamage << " dmg\n";
        return true;
    }

   
    bool mineBlock(Block& block, float deltaTime) {
        mineTimer += deltaTime;
        if (mineTimer >= mineTimeRequired) {
            mineTimer = 0.f;
            block.hit(); // mark destroyed
            std::cout << "[Player] Mined " << block.getName() << "\n";
            return true;
        }
        return false;
    }

    void resetMineTimer() { mineTimer = 0.f; }

    // ── Item interaction ──────────────────────────────────────────────────────

    
    bool pickUp(std::shared_ptr<Item> item) {
        bool ok = inventory.addItem(item);
        if (ok) std::cout << "[Player] Picked up " << item->getName() << "\n";
        return ok;
    }

    
    bool eat() {
        bool ok = inventory.eatSelected(hunger, health);
        if (ok) std::cout << "[Player] Ate food. Hunger: " << hunger.getLevel() << "\n";
        return ok;
    }

   
    std::shared_ptr<Item> placeItem() {
        auto item = inventory.getSelectedItem();
        if (!item) { std::cout << "[Player] Nothing to place.\n"; return nullptr; }
        inventory.removeItem(item->getName(), 1);
        std::cout << "[Player] Placed " << item->getName() << "\n";
        return item;
    }


    Inventory& getInventory()       { return inventory; }
    Hunger&    getHunger()          { return hunger; }
    Sleep&     getSleep()           { return sleep; }
    int        getHungerLevel()     const { return hunger.getLevel(); }
    int        getSleepLevel()      const { return sleep.getLevel(); }
    int        getHp()              const { return health.getHp(); }

    // For SaveManager compatibility
    float getPositionX() const { return x; }
    float getPositionY() const { return y; }
    void  setPosition(float nx, float ny) { x = nx; y = ny; }
    void  setHealth(int h)  { health.setHp(h); }
    void  setHunger(int h)  { hunger.setLevel(h); }
    int   getHealth()       const { return health.getHp(); }

    void printStatus() const {
        std::cout << "HP: " << health.getHp()
                  << "  Hunger: " << hunger.getLevel()
                  << "  Pos: (" << x << ", " << y << ")\n";
        inventory.print();
    }
};
