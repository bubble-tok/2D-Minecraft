/**
 * @file Monster.h
 * @brief Defines monster entities such as Zombie that interact with the player.
 */

#pragma once
#include "Entity.h"
#include "CollisionHelper.h"
#include <cmath>

/**
 * @class Player
 * @brief Forward declaration of Player class.
 */
class Player;

/**
 * @class Monster
 * @brief Base class representing hostile entities in the game world.
 *
 * Monsters inherit from Entity and have attack capabilities such as
 * attack damage, attack range, and cooldown timers.
 */
class Monster : public Entity {
protected:
    int   attackDamage;   ///< Damage dealt per attack
    float attackRange;    ///< Maximum distance at which the monster can attack
    float attackCooldown; ///< Time required between attacks
    float attackTimer;    ///< Timer tracking when the monster can attack again

public:

    /**
     * @brief Constructs a Monster entity.
     *
     * @param x Initial x-position
     * @param y Initial y-position
     * @param hp Maximum health points
     * @param damage Attack damage value
     * @param range Attack range distance
     */
    Monster(float x, float y, int hp, int damage, float range)
        : Entity(x, y, hp),
          attackDamage(damage),
          attackRange(range),
          attackCooldown(1.5f),
          attackTimer(0.f) {
    }

    /**
     * @brief Attempts to attack a target entity.
     *
     * The attack only succeeds if the target is within attack range
     * and the attack cooldown has finished.
     *
     * @param target Target entity being attacked
     */
    virtual void attack(Entity& target) {
        float dx = target.getX() - x;
        float dy = target.getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= attackRange && attackTimer <= 0.f) {
            target.takeDamage(attackDamage);
            attackTimer = attackCooldown;
        }
    }

    /**
     * @brief Updates the monster each frame.
     *
     * Handles attack cooldown timing.
     *
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override {
        if (attackTimer > 0.f)
            attackTimer -= deltaTime;
    }

    /**
     * @brief Gets the monster's attack damage.
     *
     * @return Attack damage value
     */
    int getAttackDamage() const { return attackDamage; }

    /**
     * @brief Gets the monster's attack range.
     *
     * @return Attack range distance
     */
    float getAttackRange() const { return attackRange; }
};

/**
 * @class Zombie
 * @brief A specific monster type that chases and attacks the player.
 *
 * Zombies move toward their target, are affected by gravity, and
 * collide with the tile-based world environment.
 */
class Zombie : public Monster {
private:
    float moveSpeed; ///< Horizontal movement speed
    float vy;        ///< Vertical velocity (gravity physics)
    bool  onGround;  ///< True if the zombie is standing on a tile

public:

    /**
     * @brief Constructs a Zombie entity.
     *
     * Zombies have predefined attributes such as health,
     * attack damage, and movement speed.
     *
     * @param x Initial x-position
     * @param y Initial y-position
     */
    Zombie(float x, float y)
        : Monster(x, y, 50, 10, 40.f),
          moveSpeed(40.f),
          vy(0.f),
          onGround(false) {
    }

    /**
     * @brief Updates the zombie each frame.
     *
     * Currently updates only the base Monster cooldown logic.
     *
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override {
        Monster::update(deltaTime);
    }

    /**
     * @brief Causes the zombie to chase and attack a target entity.
     *
     * The zombie moves toward the target if it is far away, applies
     * gravity physics, resolves collisions with the tile map, and
     * attempts to attack.
     *
     * @param target Entity being chased and attacked
     * @param map Tile map used for collision detection
     * @param deltaTime Time elapsed since last frame
     */
    void chaseAndAttack(Entity& target, const TileMap& map, float deltaTime) {
        if (!alive) return;

        float dx = target.getX() - x;
        float vx = 0.f;

        if (std::abs(dx) > 40.f)
            vx = (dx > 0.f ? 1.f : -1.f) * moveSpeed;

        x += vx * deltaTime;
        vy += 900.f * deltaTime;
        y += vy * deltaTime;

        resolveTileCollision(x, y, vx, vy, onGround, map);

        attack(target);
    }
};