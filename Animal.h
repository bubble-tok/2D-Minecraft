#pragma once
#include "Entity.h"
#include "Food.h"
#include "CollisionHelper.h"
#include <memory>
#include <string>

class Animal : public Entity {
private:
    std::string meatType;
    int         meatAmount;
    float       wanderSpeed;
    float       wanderDir;
    float       wanderTimer;
    float       vy;
    bool        onGround;
public:
    Animal(float x, float y,
        const std::string& meatType = "RawMeat",
        int meatAmount = 1,
        int hp = 20)
        : Entity(x, y, hp),
        meatType(meatType), meatAmount(meatAmount),
        wanderSpeed(30.f), wanderDir(1.f), wanderTimer(0.f),
        vy(0.f), onGround(false) {
    }

    // Required base override - not used directly
    void update(float deltaTime) override {}

    // World-aware update - call this from World::update
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

    std::shared_ptr<Food> dropMeat() const {
        if (meatType == "CookedMeat")
            return std::make_shared<Food>(FoodItems::CookedMeat(meatAmount));
        return std::make_shared<Food>(FoodItems::RawMeat(meatAmount));
    }

    std::string getMeatType()   const { return meatType; }
    int         getMeatAmount() const { return meatAmount; }
};