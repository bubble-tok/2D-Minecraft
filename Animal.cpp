/**
 * @file Animal.cpp
 * @brief Implements Animal wandering physics and meat drop behaviour.
 *
 * @author Group 46
 */
#include "Animal.h"

Animal::Animal(float x, float y,
               const std::string& meatType,
               int meatAmount, int hp)
    : Entity(x, y, hp),
      meatType(meatType), meatAmount(meatAmount),
      wanderSpeed(30.f), wanderDir(1.f), wanderTimer(0.f),
      vy(0.f), onGround(false) {}

void Animal::tick(float dt, const TileMap& map) {
    if (!alive) return;

    // Reverse direction every 2 seconds (simple idle wander)
    wanderTimer += dt;
    if (wanderTimer > 2.f) { wanderDir = -wanderDir; wanderTimer = 0.f; }

    float vx    = wanderDir * wanderSpeed;
    float prevX = x;

    // Integrate horizontal movement
    x  += vx * dt;

    // Integrate gravity and vertical movement
    vy += 900.f * dt;
    y  += vy * dt;

    // Resolve collisions against the tile map
    resolveTileCollision(x, y, vx, vy, onGround, map);

    // If horizontal movement was blocked by terrain, reverse direction
    if ((vx > 0.f && x < prevX + vx * dt - 0.5f) ||
        (vx < 0.f && x > prevX + vx * dt + 0.5f))
        wanderDir = -wanderDir;
}

std::shared_ptr<Food> Animal::dropMeat() const {
    // Animals configured as CookedMeat drop the cooked variant directly
    if (meatType == "CookedMeat")
        return std::make_shared<Food>(FoodItems::CookedMeat(meatAmount));
    return std::make_shared<Food>(FoodItems::RawMeat(meatAmount));
}
