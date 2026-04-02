/**
 * @file Animal.h
 * @brief Declares the Animal class — a passive wandering entity.
 *
 * @author Group 46
 */
#pragma once
#include "Entity.h"
#include "Food.h"
#include "CollisionHelper.h"
#include <memory>
#include <string>

/**
 * @class Animal
 * @brief A passive entity that wanders the surface and drops meat when killed.
 *
 * Animals move back and forth at a fixed wanderSpeed, reversing direction
 * every ~2 seconds or when they collide with terrain. They apply gravity and
 * resolve tile collisions each tick. On death they drop a RawMeat Food item.
 *
 * @author Group 46
 */
class Animal : public Entity {
public:
    /**
     * @brief Constructs an Animal at the given world position.
     * @param x          Initial X position in pixels.
     * @param y          Initial Y position in pixels.
     * @param meatType   Name of the Food item dropped on death. Defaults to "RawMeat".
     * @param meatAmount Quantity of meat dropped. Defaults to 1.
     * @param hp         Maximum hit points. Defaults to 20.
     */
    Animal(float x, float y,
           const std::string& meatType = "RawMeat",
           int meatAmount = 1,
           int hp = 20);

    /// update() is unused for Animal; physics run through tick() instead.
    void update(float deltaTime) override {}

    /**
     * @brief Advances the animal's wandering physics by one frame.
     *
     * Applies horizontal movement, gravity, and tile collision.
     * Reverses wanderDir when terrain blocks forward movement.
     *
     * @param deltaTime Elapsed time since the previous frame in seconds.
     * @param map       TileMap used for collision resolution.
     */
    void tick(float deltaTime, const TileMap& map);

    /**
     * @brief Creates and returns the meat Food item dropped on death.
     * @return Shared pointer to a Food item of the configured meatType.
     */
    std::shared_ptr<Food> dropMeat() const;

    /** @brief Returns the name of the meat item this animal drops. */
    std::string getMeatType()   const { return meatType; }

    /** @brief Returns the quantity of meat dropped on death. */
    int         getMeatAmount() const { return meatAmount; }

private:
    std::string meatType;    ///< Food name for the drop (e.g. "RawMeat").
    int         meatAmount;  ///< Number of meat items dropped on death.
    float       wanderSpeed; ///< Pixels per second horizontal movement speed.
    float       wanderDir;   ///< +1.0 = right, -1.0 = left.
    float       wanderTimer; ///< Accumulates time; direction reverses every 2 seconds.
    float       vy;          ///< Vertical velocity in pixels per second.
    bool        onGround;    ///< True when the animal is standing on a tile.
};
