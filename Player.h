/**
 * @file Player.h
 * @brief Defines the Player class representing the controllable character.
 */

#pragma once
#include "Entity.h"
#include "Inventory.h"
#include "Hunger.h"
#include "Sleep.h"
#include "Food.h"
#include "Block.h"
#include <iostream>
#include <cmath>

/**
 * @class Player
 * @brief Represents the player character controlled by the user.
 *
 * The Player class extends the Entity class and adds systems for
 * inventory management, hunger, sleep, movement physics, combat,
 * and block interaction (mining and placing).
 */
class Player : public Entity {
private:
    Inventory inventory; ///< Player inventory system
    Hunger    hunger;    ///< Hunger system affecting health and gameplay
    Sleep     sleep;     ///< Sleep system affecting movement speed

    // Physics
    float velocityY; ///< Vertical velocity used for gravity simulation
    bool  onGround;  ///< True if the player is currently on the ground

    static constexpr float GRAVITY    = 800.f;  ///< Downward acceleration
    static constexpr float JUMP_FORCE = -400.f; ///< Upward force applied when jumping
    static constexpr float MOVE_SPEED = 150.f;  ///< Base horizontal movement speed
    static constexpr float GROUND_Y   = 400.f;  ///< Temporary ground position

    // Combat
    int   attackDamage;   ///< Damage dealt per attack
    float attackRange;    ///< Maximum attack distance
    float attackCooldown; ///< Time required between attacks
    float attackTimer;    ///< Cooldown timer for next attack

    // Block mining
    float mineTimer;        ///< Time spent mining a block
    float mineTimeRequired; ///< Time required to break a block

public:

    /**
     * @brief Constructs a Player object.
     *
     * Initializes the player with default health, movement,
     * combat attributes, and mining properties.
     *
     * @param x Initial x-position
     * @param y Initial y-position
     */
    Player(float x = 0.f, float y = 0.f)
        : Entity(x, y, 100),
          velocityY(0.f), onGround(false),
          attackDamage(10), attackRange(80.f),
          attackCooldown(0.5f), attackTimer(0.f),
          mineTimer(0.f), mineTimeRequired(1.5f) {}

    /**
     * @brief Updates the player each frame.
     *
     * Handles gravity, ground collision, attack cooldown,
     * hunger decay, and starvation damage.
     *
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override {

        // Apply gravity
        if (!onGround) {
            velocityY += GRAVITY * deltaTime;
            y += velocityY * deltaTime;
        }

        // Simple ground collision
        if (y >= GROUND_Y) {
            y = GROUND_Y;
            velocityY = 0.f;
            onGround = true;
        }

        // Attack cooldown timer
        if (attackTimer > 0.f)
            attackTimer -= deltaTime;

        // Hunger decay
        hunger.decrease(static_cast<int>(deltaTime));

        // Starvation damage
        if (hunger.isEmpty())
            health.decrease(1);
    }

    /**
     * @brief Moves the player horizontally.
     *
     * Movement speed is reduced if the player is sleep-deprived.
     *
     * @param direction -1 for left, +1 for right
     * @param deltaTime Time elapsed since last frame
     */
    void move(int direction, float deltaTime) {
        float speed = MOVE_SPEED * (sleep.isEmpty() ? 0.5f : 1.0f);
        x += direction * speed * deltaTime;
    }

    /**
     * @brief Makes the player jump.
     *
     * Jumping only works when the player is on the ground.
     */
    void jump() {
        if (onGround) {
            velocityY = JUMP_FORCE;
            onGround  = false;
        }
    }

    /**
     * @brief Attacks a target entity.
     *
     * The attack succeeds only if the target is within range
     * and the cooldown timer has expired.
     *
     * @param target Target entity to attack
     * @return True if the attack was successful
     */
    bool attack(Entity& target) {
        if (attackTimer > 0.f) return false;

        float dx   = target.getX() - x;
        float dy   = target.getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > attackRange) return false;

        target.takeDamage(attackDamage);
        attackTimer = attackCooldown;

        std::cout << "[Player] Hit "
                  << (target.isAlive() ? "entity" : "entity (killed!)")
                  << " for " << attackDamage << " dmg\n";

        return true;
    }

    /**
     * @brief Mines a block over time.
     *
     * The player must hold the mining action until the required
     * mining time is reached.
     *
     * @param block Block being mined
     * @param deltaTime Time elapsed since last frame
     * @return True if the block was successfully mined
     */
    bool mineBlock(Block& block, float deltaTime) {
        mineTimer += deltaTime;

        if (mineTimer >= mineTimeRequired) {
            mineTimer = 0.f;
            block.hit();

            std::cout << "[Player] Mined " << block.getName() << "\n";
            return true;
        }

        return false;
    }

    /**
     * @brief Resets the mining timer.
     */
    void resetMineTimer() { mineTimer = 0.f; }

    /**
     * @brief Picks up an item and adds it to the inventory.
     *
     * @param item Shared pointer to the item
     * @return True if the item was successfully added
     */
    bool pickUp(std::shared_ptr<Item> item) {
        bool ok = inventory.addItem(item);
        if (ok)
            std::cout << "[Player] Picked up " << item->getName() << "\n";
        return ok;
    }

    /**
     * @brief Consumes food from the selected inventory slot.
     *
     * @return True if food was successfully eaten
     */
    bool eat() {
        bool ok = inventory.eatSelected(hunger, health);

        if (ok)
            std::cout << "[Player] Ate food. Hunger: "
                      << hunger.getLevel() << "\n";

        return ok;
    }

    /**
     * @brief Places the currently selected item in the world.
     *
     * Removes one item from the inventory.
     *
     * @return Shared pointer to the placed item
     */
    std::shared_ptr<Item> placeItem() {
        auto item = inventory.getSelectedItem();

        if (!item) {
            std::cout << "[Player] Nothing to place.\n";
            return nullptr;
        }

        inventory.removeItem(item->getName(), 1);

        std::cout << "[Player] Placed " << item->getName() << "\n";

        return item;
    }

    /// @brief Gets the player's inventory.
    Inventory& getInventory() { return inventory; }

    /// @brief Gets the player's hunger system.
    Hunger& getHunger() { return hunger; }

    /// @brief Gets the player's sleep system.
    Sleep& getSleep() { return sleep; }

    /// @brief Gets the current hunger level.
    int getHungerLevel() const { return hunger.getLevel(); }

    /// @brief Gets the current sleep level.
    int getSleepLevel() const { return sleep.getLevel(); }

    /// @brief Gets the player's current health.
    int getHp() const { return health.getHp(); }

    /// @brief Gets the player's X position.
    float getPositionX() const { return x; }

    /// @brief Gets the player's Y position.
    float getPositionY() const { return y; }

    /// @brief Sets the player's position.
    void setPosition(float nx, float ny) { x = nx; y = ny; }

    /// @brief Sets the player's health value.
    void setHealth(int h) { health.setHp(h); }

    /// @brief Sets the player's hunger value.
    void setHunger(int h) { hunger.setLevel(h); }

    /// @brief Returns the player's current health.
    int getHealth() const { return health.getHp(); }

    /**
     * @brief Prints player status and inventory contents.
     */
    void printStatus() const {
        std::cout << "HP: " << health.getHp()
                  << "  Hunger: " << hunger.getLevel()
                  << "  Pos: (" << x << ", " << y << ")\n";

        inventory.print();
    }
};