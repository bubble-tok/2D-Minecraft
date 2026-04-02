/**
 * @file Monster.h
 * @brief Declares Monster (base) and Zombie (concrete) hostile entity classes.
 *
 * @author Group 46
 */
#pragma once
#include "Entity.h"
#include "CollisionHelper.h"

/**
 * @class Monster
 * @brief Abstract base class for hostile entities.
 *
 * Manages an attack cooldown timer and deals damage when the target is
 * within attackRange. Subclasses implement movement behaviour.
 *
 * @author Group 46
 */
class Monster : public Entity {
public:
    /**
     * @brief Constructs a Monster with the given combat stats.
     * @param x      Initial X position.
     * @param y      Initial Y position.
     * @param hp     Maximum hit points.
     * @param damage Damage dealt per hit.
     * @param range  Maximum attack range in pixels.
     */
    Monster(float x, float y, int hp, int damage, float range);

    /**
     * @brief Attempts to attack the target if in range and cooldown has expired.
     *
     * Sets attackTimer to attackCooldown on a successful hit so the monster
     * cannot attack again immediately.
     *
     * @param target Entity to attack.
     * @return True if damage was dealt this call; false if out of range or on cooldown.
     */
    virtual bool attack(Entity& target);

    /**
     * @brief Ticks down the attack cooldown timer.
     * @param deltaTime Elapsed time in seconds.
     */
    void update(float deltaTime) override;

    /** @brief Returns the damage dealt per hit. */
    int   getAttackDamage() const { return attackDamage; }

    /** @brief Returns the maximum attack range in pixels. */
    float getAttackRange()  const { return attackRange; }

protected:
    int   attackDamage;    ///< Damage dealt per successful hit.
    float attackRange;     ///< Maximum range in pixels for a hit to land.
    float attackCooldown;  ///< Seconds between successive attacks.
    float attackTimer;     ///< Remaining cooldown; attack fires when this reaches 0.
};

/**
 * @class Zombie
 * @brief A hostile entity that chases and attacks the player.
 *
 * Zombies apply gravity, resolve tile collisions, and move toward the player
 * each frame. Attack range is 60px (~1.5 tiles). Spawn rate increases as the
 * player explores further right.
 *
 * @author Group 46
 */
class Zombie : public Monster {
public:
    /**
     * @brief Constructs a Zombie with default stats (50 HP, 10 damage, 60px range).
     * @param x Initial X position.
     * @param y Initial Y position.
     */
    Zombie(float x, float y);

    /// Delegates to Monster::update() for cooldown ticking.
    void update(float deltaTime) override;

    /**
     * @brief Moves toward the target, applies physics, and attempts to attack.
     *
     * Calls Monster::update() once to tick the cooldown, then moves toward
     * the target, integrates gravity, resolves tile collisions, and calls
     * Monster::attack(). The player's invincibility flag (set by Game's
     * iframe system) is respected inside takeDamage().
     *
     * @param target  Entity to chase and attack (typically the player).
     * @param map     TileMap for collision resolution.
     * @param deltaTime Elapsed time in seconds.
     * @return True if a hit landed on the target this frame.
     */
    bool chaseAndAttack(Entity& target, const TileMap& map, float deltaTime);

private:
    float moveSpeed; ///< Horizontal chase speed in pixels per second.
    float vy;        ///< Current vertical velocity.
    bool  onGround;  ///< True when the zombie is standing on a tile.
};
