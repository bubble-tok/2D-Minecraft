#pragma once
#include "Entity.h"
#include "Food.h"
#include <memory>
#include <string>


class Animal : public Entity {
private:
    std::string meatType;   ///< Name of food item dropped on death
    int         meatAmount; ///< How many units to drop
    float       wanderSpeed;
    float       wanderDir;  ///< 1.0 = right, -1.0 = left
    float       wanderTimer;

public:
    Animal(float x, float y,
           const std::string& meatType = "RawMeat",
           int meatAmount = 1,
           int hp = 20)
        : Entity(x, y, hp),
          meatType(meatType), meatAmount(meatAmount),
          wanderSpeed(30.f), wanderDir(1.f), wanderTimer(0.f) {}

  
    void update(float deltaTime) override {
        if (!alive) return;
        wanderTimer += deltaTime;
        if (wanderTimer > 2.f) {          // change direction every 2s
            wanderDir  = -wanderDir;
            wanderTimer = 0.f;
        }
        x += wanderDir * wanderSpeed * deltaTime;
    }

    
    std::shared_ptr<Food> dropMeat() const {
        // Hardcode nutrition values based on meat type
        if (meatType == "CookedMeat")
            return std::make_shared<Food>(FoodItems::CookedMeat(meatAmount));
        return std::make_shared<Food>(FoodItems::RawMeat(meatAmount));
    }

    std::string getMeatType() const { return meatType; }
	int getMeatAmount() const { return meatAmount; }
};
