/**
 * @file Player.h
 * @brief Declares the Player class — the user-controlled entity.
 *
 * @author Group 46
 */
#pragma once
#include "Entity.h"
#include "Inventory.h"
#include "Hunger.h"
#include "Sleep.h"
#include "Food.h"
#include "Block.h"
#include "Tool.h"

/**
 * @class Player
 * @brief The user-controlled entity.
 *
 * Owns the player's Inventory, Hunger, and Sleep subsystems. Physics
 * (velocity, onGround, coyote time) are managed entirely by Game::tickPhysics()
 * to avoid split state. Combat and tool bonus calculations are delegated to
 * this class so Game can remain stat-agnostic.
 *
 * @author Group 46
 */
class Player : public Entity {
public:
    /**
     * @brief Constructs a Player at the given world position with 100 max HP.
     * @param x Initial X position in pixels. Defaults to 0.
     * @param y Initial Y position in pixels. Defaults to 0.
     */
    Player(float x = 0.f, float y = 0.f);

    /// Physics are handled by Game::tickPhysics(); this override is a no-op.
    void update(float) override {}

    /**
     * @brief Attacks a target entity if it is within attack range.
     * @param target      Entity to hit.
     * @param totalDamage Damage to deal (base + tool bonus, computed by Game).
     * @return True if the target was within range and hit; false otherwise.
     */
    bool attack(Entity& target, int totalDamage);

    /**
     * @brief Adds an item to the player's inventory.
     * @param item Shared pointer to the item to add.
     * @return True if picked up; false if inventory is full.
     */
    bool pickUp(std::shared_ptr<Item> item);

    /**
     * @brief Eats the food item in the currently selected hotbar slot.
     *
     * RawMeat is blocked — it must be cooked at a Campfire first.
     *
     * @return True if food was consumed; false if slot is empty, non-food, or RawMeat.
     */
    bool eat();

    /**
     * @brief Returns a pointer to the Tool currently equipped (selected slot).
     * @return Pointer to the equipped Tool, or nullptr if no tool is selected.
     */
    Tool*  getEquippedTool()      const;

    /**
     * @brief Returns total attack damage including any equipped sword bonus.
     * @return Base damage (10) plus sword bonusDamage if a sword is equipped.
     */
    int    getTotalAttackDamage() const;

    /**
     * @brief Returns the mining time multiplier from the equipped pickaxe.
     *
     * Values less than 1.0 mean faster mining. Returns 1.0 if no pickaxe equipped.
     *
     * @return Multiplier applied to Game::BASE_MINE_TIME.
     */
    float  getMineSpeedMult()     const;

    /** @brief Returns a mutable reference to the player's Inventory. */
    Inventory& getInventory();

    /** @brief Returns a const reference to the player's Inventory. */
    const Inventory& getInventory() const;

    /** @brief Returns a mutable reference to the player's Hunger. */
    Hunger&    getHunger();

    /** @brief Returns a mutable reference to the player's Sleep. */
    Sleep&     getSleep();

    /** @brief Returns the current hunger level (0–100). */
    int   getHungerLevel() const;

    /** @brief Returns the current sleep level (0–100). */
    int   getSleepLevel()  const;

    /** @brief Returns current HP (alias for Entity::getHp()). */
    int   getHealth()      const;

    /** @brief Returns current HP. */
    int   getHp()          const;

    /** @brief Returns the player's current world X position. */
    float getPositionX()   const;

    /** @brief Returns the player's current world Y position. */
    float getPositionY()   const;

    /**
     * @brief Directly sets the player's world position (used by Game and SaveManager).
     * @param nx New X coordinate.
     * @param ny New Y coordinate.
     */
    void  setPosition(float nx, float ny);

    /**
     * @brief Directly sets HP, clamped by Health::setHp().
     * @param h Desired HP value.
     */
    void  setHealth(int h);

    /**
     * @brief Directly sets the hunger level, clamped by Hunger::setLevel().
     * @param h Desired hunger level.
     */
    void  setHunger(int h);

    /**
     * @brief Directly sets the sleep level, clamped by Sleep::setLevel().
     * @param s Desired sleep level.
     */
    void  setSleep(int s);

    /// Prints player status and inventory to stdout for debugging.
    void printStatus() const;

private:
    Inventory inventory;          ///< The player's 36-slot item inventory.
    Hunger    hunger;             ///< Satiation level; reaches 0 → starvation.
    Sleep     sleep;              ///< Energy level; reaches 0 → movement penalty.

    int   baseAttackDamage = 10;  ///< Unarmed attack damage before tool bonus.
    float attackRange      = 80.f; ///< Maximum melee attack range in pixels.
};
