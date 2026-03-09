#pragma once
#include "Entity.h"

class Player; // forward declaration


class Monster : public Entity {
protected:
    int   attackDamage;
    float attackRange;
    float attackCooldown;   ///< Seconds between attacks
    float attackTimer;

public:
    Monster(float x, float y, int hp, int damage, float range)
        : Entity(x, y, hp),
          attackDamage(damage), attackRange(range),
          attackCooldown(1.5f), attackTimer(0.f) {}

    
    virtual void attack(Entity& target) {
        float dx = target.getX() - x;
        float dy = target.getY() - y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist <= attackRange && attackTimer <= 0.f) {
            target.takeDamage(attackDamage);
            attackTimer = attackCooldown;
        }
    }

    void update(float deltaTime) override {
        if (attackTimer > 0.f) attackTimer -= deltaTime;
    }

    int   getAttackDamage() const { return attackDamage; }
    float getAttackRange()  const { return attackRange; }
};


class Zombie : public Monster {
private:
    float moveSpeed;

public:
    Zombie(float x, float y)
        : Monster(x, y, 50, 10, 40.f), moveSpeed(40.f) {}

    
    void update(float deltaTime) override {
        Monster::update(deltaTime); // tick cooldown
    }

    
    void chaseAndAttack(Entity& target, float deltaTime) {
        if (!alive) return;
        float dx = target.getX() - x;
        float dist = std::abs(dx);
        // Move toward target
        if (dist > 40.f)
            x += (dx > 0 ? 1.f : -1.f) * moveSpeed * deltaTime;
        // Try to attack
        attack(target);
    }
};
