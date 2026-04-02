/**
 * @file Monster.cpp
 * @brief This class implements Monster chase-and-attack behaviour.
 * 
 * Monsters are entities that can damage the player when in range.
 * Since they are an entity, they can be killed.
 * Zombies are a monster that can chase the player.
 * @author Group 46
 */
#include "Monster.h"
#include <cmath>

// ── Monster ───────────────────────────────────────────────────────────────────

Monster::Monster(float x, float y, int hp, int damage, float range)
    : Entity(x, y, hp),
      attackDamage(damage), attackRange(range),
      attackCooldown(1.5f), attackTimer(0.f) {}

bool Monster::attack(Entity& target) {
    float dx   = target.getX() - x;
    float dy   = target.getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Only attack if target is in range and cooldown has expired
    if (dist <= attackRange && attackTimer <= 0.f) {
        target.takeDamage(attackDamage); // respects target's invincible flag
        attackTimer = attackCooldown;    // reset cooldown after hit
        return true;
    }
    return false;
}

void Monster::update(float dt) {
    // Tick down the attack cooldown each frame
    if (attackTimer > 0.f) attackTimer -= dt;
}

// ── Zombie ────────────────────────────────────────────────────────────────────

Zombie::Zombie(float x, float y)
    : Monster(x, y, 50, 10, 60.f), // 50 HP, 10 damage, 60px range (~1.5 tiles)
      moveSpeed(40.f), vy(0.f), onGround(false) {}

void Zombie::update(float dt) {
    Monster::update(dt);
}

bool Zombie::chaseAndAttack(Entity& target, const TileMap& map, float dt) {
    if (!alive) return false;

    // Tick cooldown once — World::update does NOT call update() separately
    Monster::update(dt);

    // Chase: move horizontally toward the target if not already adjacent
    float dx = target.getX() - x;
    float vx = 0.f;
    if (std::abs(dx) > 40.f)
        vx = (dx > 0.f ? 1.f : -1.f) * moveSpeed;

    // Integrate movement with gravity
    x  += vx * dt;
    vy += 900.f * dt;
    y  += vy * dt;

    // Resolve tile collisions so zombies walk on terrain
    resolveTileCollision(x, y, vx, vy, onGround, map);

    // Attempt attack — returns true if damage was dealt this frame
    return attack(target);
}
