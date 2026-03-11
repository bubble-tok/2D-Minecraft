#pragma once
#include "Entity.h"
#include "Food.h"
#include "CollisionHelper.h"
#include <memory>
#include <string>

/**
 * @class Animal
 * @brief Represents a passive animal entity that wanders and drops meat when killed.
 *
 * Animals move around the world with simple wandering behavior and interact with
 * the tile map for collision detection. When the animal dies, it can drop a Food
 * item representing meat.
 */
class Animal : public Entity {
private:
    std::string meatType;  ///< Type of meat dropped when the animal dies
    int         meatAmount;///< Amount of meat dropped
    float       wanderSpeed; ///< Horizontal speed when wandering
    float       wanderDir;   ///< Current wandering direction (-1 = left, 1 = right)
    float       wanderTimer; ///< Timer controlling how often the animal changes direction
    float       vy;          ///< Vertical velocity (used for gravity)
    bool        onGround;    ///< Indicates if the animal is currently on the ground

public:

    /**
     * @brief Constructs an Animal entity.
     *
     * @param x Initial x-position in the world
     * @param y Initial y-position in the world
     * @param meatType Type of meat dropped when the animal dies (default "RawMeat")
     * @param meatAmount Amount of meat dropped (default 1)
     * @param hp Initial health of the animal (default 20)
     */
    Animal(float x, float y,
        const std::string& meatType = "RawMeat",
        int meatAmount = 1,
        int hp = 20)
        : Entity(x, y, hp),
        meatType(meatType), meatAmount(meatAmount),
        wanderSpeed(30.f), wanderDir(1.f), wanderTimer(0.f),
        vy(0.f), onGround(false) {
    }

    /**
     * @brief Required override from Entity.
     *
     * This function exists to satisfy the base class interface but is not used
     * directly. Instead, the world calls tick() which includes map collision logic.
     *
     * @param deltaTime Time elapsed since the last frame
     */
    void update(float deltaTime) override {}

    /**
     * @brief Updates the animal's behavior and physics within the world.
     *
     * Handles wandering movement, gravity, and tile collision detection.
     * The animal periodically changes direction and reacts to walls.
     *
     * @param deltaTime Time elapsed since the last frame
     * @param map Reference to the TileMap used for collision detection
     */
    void tick(float deltaTime, const TileMap& map) {
        if (!alive) return;

        wanderTimer += deltaTime;
        if (wanderTimer > 2.f) {
            wanderDir = -wanderDir;
            wanderTimer = 0.f;
        }

        float vx = wanderDir * wanderSpeed;
        float prevX = x;

        x += vx * deltaTime;
        vy += 900.f * deltaTime;
        y += vy * deltaTime;

        resolveTileCollision(x, y, vx, vy, onGround, map);

        // If X was blocked by a wall, flip wander direction
        if ((vx > 0.f && x < prevX + vx * deltaTime - 0.5f) ||
            (vx < 0.f && x > prevX + vx * deltaTime + 0.5f))
            wanderDir = -wanderDir;
    }

    /**
     * @brief Generates the meat dropped by the animal.
     *
     * Creates a Food item representing the meat dropped when the animal dies.
     *
     * @return Shared pointer to the Food item dropped
     */
    std::shared_ptr<Food> dropMeat() const {
        if (meatType == "CookedMeat")
            return std::make_shared<Food>(FoodItems::CookedMeat(meatAmount));
        return std::make_shared<Food>(FoodItems::RawMeat(meatAmount));
    }

    /**
     * @brief Gets the type of meat dropped by the animal.
     *
     * @return The meat type as a string
     */
    std::string getMeatType()   const { return meatType; }

    /**
     * @brief Gets the amount of meat dropped.
     *
     * @return Quantity of meat dropped
     */
    int         getMeatAmount() const { return meatAmount; }
};