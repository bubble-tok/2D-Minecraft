/**
 * @file Animal.cpp
 * @brief The Animal.cpp file implements the movement, physics, and meat drop behavior of the Animal class.
 *
 * The Animal.cpp file defines how the Animal object behaves in the game world.
 * The implementation includes wandering movement, gravity effects, collision
 * handling with the tile map, and logic for generating meat drops when the
 * Animal object is defeated.
 *
 * @author Group 46
 */
#include "Animal.h"

/**
 * @brief The constructor initializes an Animal object with position, health, and drop properties.
 *
 * The constructor sets the initial state of the Animal object, including its
 * position, health, movement variables, and meat drop configuration.
 *
 * @param x The x parameter represents the initial x-position of the Animal object
 * @param y The y parameter represents the initial y-position of the Animal object
 * @param meatType The meatType parameter defines the type of meat dropped
 * @param meatAmount The meatAmount parameter defines the number of meat items dropped
 * @param hp The hp parameter represents the initial health of the Animal object
 */
Animal::Animal(float x, float y,
               const std::string& meatType,
               int meatAmount, int hp)
    : Entity(x, y, hp),
      meatType(meatType), meatAmount(meatAmount),
      wanderSpeed(30.f), wanderDir(1.f), wanderTimer(0.f),
      vy(0.f), onGround(false) {}

/**
 * @brief The tick function updates movement, gravity, and collision for the Animal object.
 *
 * The tick function controls the behavior of the Animal object over time.
 * The Animal object moves horizontally, is affected by gravity, and interacts
 * with the TileMap for collision detection. The Animal object also reverses
 * direction periodically and when movement is blocked by terrain.
 *
 * @param dt The dt parameter represents the time elapsed since the last frame
 * @param map The map parameter represents the TileMap used for collision detection
 */
void Animal::tick(float dt, const TileMap& map) {
    if (!alive) return; // The Animal object does not update if the Animal object is not alive

    // The wanderTimer variable tracks time and triggers direction changes
    wanderTimer += dt;
    if (wanderTimer > 2.f) {
        wanderDir = -wanderDir;
        wanderTimer = 0.f;
    }

    float vx    = wanderDir * wanderSpeed;
    float prevX = x;

    // The Animal object updates horizontal position based on velocity
    x += vx * dt;

    // The Animal object applies gravity to vertical velocity and updates position
    vy += 900.f * dt;
    y  += vy * dt;

    // The Animal object resolves collisions with the TileMap
    resolveTileCollision(x, y, vx, vy, onGround, map);

    // The Animal object reverses direction if horizontal movement is blocked
    if ((vx > 0.f && x < prevX + vx * dt - 0.5f) ||
        (vx < 0.f && x > prevX + vx * dt + 0.5f))
        wanderDir = -wanderDir;
}

/**
 * @brief The dropMeat function creates a Food object representing the meat dropped by the Animal object.
 *
 * The dropMeat function checks the configured meat type and returns the
 * corresponding Food object with the correct quantity.
 *
 * @return The function returns a shared pointer to the Food object created as a drop
 */
std::shared_ptr<Food> Animal::dropMeat() const {
    // The function returns cooked meat if the Animal object is configured to drop cooked meat
    if (meatType == "CookedMeat")
        return std::make_shared<Food>(FoodItems::CookedMeat(meatAmount));

    // The function returns raw meat otherwise
    return std::make_shared<Food>(FoodItems::RawMeat(meatAmount));
}