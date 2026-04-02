/**
 * @file Animal.h
 * @brief The Animal.h file declares the Animal class, which represents a passive wandering entity.
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
 * @brief The Animal class represents a passive entity that wanders the surface and drops meat when killed.
 *
 * The Animal class moves back and forth at a fixed wanderSpeed, reverses direction
 * every ~2 seconds or when the Animal object collides with terrain, applies gravity,
 * and resolves tile collisions during each tick. When the Animal object dies, the
 * Animal class drops a RawMeat Food item.
 *
 * @author Group 46
 */
class Animal : public Entity {
public:
    /**
     * @brief The constructor creates an Animal object at the given world position.
     *
     * @param x The x parameter represents the initial X position in pixels.
     * @param y The y parameter represents the initial Y position in pixels.
     * @param meatType The meatType parameter represents the name of the Food item dropped on death. The default value is "RawMeat".
     * @param meatAmount The meatAmount parameter represents the quantity of meat dropped. The default value is 1.
     * @param hp The hp parameter represents the maximum hit points. The default value is 20.
     */
    Animal(float x, float y,
           const std::string& meatType = "RawMeat",
           int meatAmount = 1,
           int hp = 20);

    /**
     * @brief The update function overrides the Entity update function.
     *
     * The Animal class does not use the update function because the Animal class
     * runs physics through the tick function instead.
     *
     * @param deltaTime The deltaTime parameter represents the elapsed time since the previous frame.
     */
    void update(float deltaTime) override {}

    /**
     * @brief The tick function advances the wandering physics of the Animal object by one frame.
     *
     * The tick function applies horizontal movement, gravity, and tile collision.
     * The tick function also reverses wanderDir when terrain blocks forward movement.
     *
     * @param deltaTime The deltaTime parameter represents the elapsed time since the previous frame in seconds.
     * @param map The map parameter represents the TileMap used for collision resolution.
     */
    void tick(float deltaTime, const TileMap& map);

    /**
     * @brief The dropMeat function creates and returns the Food item dropped on death.
     *
     * @return The function returns a shared pointer to a Food item of the configured meatType.
     */
    std::shared_ptr<Food> dropMeat() const;

    /**
     * @brief The getMeatType function returns the name of the meat item dropped by the Animal object.
     *
     * @return The function returns the meat type as a string.
     */
    std::string getMeatType() const { return meatType; }

    /**
     * @brief The getMeatAmount function returns the quantity of meat dropped when the Animal object dies.
     *
     * @return The function returns the number of meat items dropped.
     */
    int getMeatAmount() const { return meatAmount; }

private:
    std::string meatType;    ///< The meatType variable stores the Food name for the drop (for example, "RawMeat").
    int         meatAmount;  ///< The meatAmount variable stores the number of meat items dropped on death.
    float       wanderSpeed; ///< The wanderSpeed variable stores the horizontal movement speed in pixels per second.
    float       wanderDir;   ///< The wanderDir variable stores the movement direction (+1.0 = right, -1.0 = left).
    float       wanderTimer; ///< The wanderTimer variable stores elapsed time and triggers direction reversal every 2 seconds.
    float       vy;          ///< The vy variable stores the vertical velocity in pixels per second.
    bool        onGround;    ///< The onGround variable indicates whether the Animal object is standing on a tile.
};