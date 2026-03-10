#pragma once
#include "Entity.h"           
#include "CollisionHelper.h"   
#include <cmath>               

class Player;                  // Forward declaration of Player class 
class Monster : public Entity {   // Monster class inherits from Entity 
protected:
    int   attackDamage;       // Amount of damage dealt per attack
    float attackRange;        // Maximum distance at which the monster can attack
    float attackCooldown;     // Time required between attacks
    float attackTimer;        // Countdown timer before the monster can attack again

public:
    // this is a constructor for creating a monster
    // x, y = starting position
    // hp = health points
    // damage = attack damage
    // range = attack range
    Monster(float x, float y, int hp, int damage, float range)
        : Entity(x, y, hp),                // Initialize base Entity 
        attackDamage(damage),              // Set attack damage
        attackRange(range),                // Set attack range
        attackCooldown(1.5f),              // Default cooldown between attacks (1.5 seconds)
        attackTimer(0.f) {                 // Attack timer starts at 0 (can attack immediately)
    }

    // Attempts to attack the target entity if it is within range
    virtual void attack(Entity& target) {
        float dx = target.getX() - x;      // Horizontal distance to target
        float dy = target.getY() - y;      // Vertical distance to target
        float dist = std::sqrt(dx * dx + dy * dy); // Calculate Euclidean distance

        // Check if target is within attack range and attack cooldown has finished
        if (dist <= attackRange && attackTimer <= 0.f) {
            target.takeDamage(attackDamage); // Apply damage to the target
            attackTimer = attackCooldown;    // Reset attack timer
        }
    }

    // Update function called every frame
    void update(float deltaTime) override {
        if (attackTimer > 0.f) attackTimer -= deltaTime;  // Decrease attack cooldown timer over time
    }

    // Getter for attack damage
    int   getAttackDamage() const { return attackDamage; }

    // Getter for attack range
    float getAttackRange()  const { return attackRange; }
};

class Zombie : public Monster {   // Zombie is a specific type of Monster
private:
    float moveSpeed;        // Horizontal movement speed
    float vy;               // Vertical velocity (used for gravity and jumping physics)
    bool  onGround;         // Indicates whether the zombie is standing on the ground

public:
    // Constructor that creates a zombie with predefined stats
    Zombie(float x, float y)
        : Monster(x, y, 50, 10, 40.f),   // Call Monster constructor: hp=50, damage=10, range=40
        moveSpeed(40.f),                 // Set zombie movement speed
        vy(0.f),                         // Initial vertical velocity
        onGround(false) {                // Zombie starts in air until collision is resolved
    }

    // Update function called each frame
    void update(float deltaTime) override {
        Monster::update(deltaTime);      // Update monster logic (cooldown timer)
    }

    // Makes the zombie move toward the target and attack if close enough
    void chaseAndAttack(Entity& target, const TileMap& map, float deltaTime) {
        if (!alive) return;              // If the zombie is dead, stop updating behavior

        float dx = target.getX() - x;    // Horizontal distance to the target
        float vx = 0.f;                  // Horizontal velocity

        // Move toward the target if they are farther than 40 units away
        if (std::abs(dx) > 40.f)
            vx = (dx > 0.f ? 1.f : -1.f) * moveSpeed;  // Move left or right depending on target position

        x += vx * deltaTime;             // Update horizontal position
        vy += 900.f * deltaTime;         // Apply gravity acceleration
        y += vy * deltaTime;             // Update vertical position

        // Resolve collision with the tile map (prevents walking through blocks)
        resolveTileCollision(x, y, vx, vy, onGround, map);

        attack(target);                  // Attempt to attack the target
    }
};