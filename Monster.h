#pragma once
#include "Entity.h"
#include "CollisionHelper.h"
#include <cmath>

class Player;

class Monster : public Entity {
protected:
    int   attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
public:
    Monster(float x, float y, int hp, int damage, float range)
        : Entity(x, y, hp),
        attackDamage(damage), attackRange(range),
        attackCooldown(1.5f), attackTimer(0.f) {
    }

    virtual void attack(Entity& target) {
        float dx = target.getX() - x;
        float dy = target.getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);
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
    float vy;
    bool  onGround;
public:
    Zombie(float x, float y)
        : Monster(x, y, 50, 10, 40.f),
        moveSpeed(40.f), vy(0.f), onGround(false) {
    }

    void update(float deltaTime) override {
        Monster::update(deltaTime);
    }

    void chaseAndAttack(Entity& target, const TileMap& map, float deltaTime) {
        if (!alive) return;

        // Tick attack cooldown every frame
        Monster::update(deltaTime);

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
