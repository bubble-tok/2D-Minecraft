#pragma once
#include "CollisionHelper.h"
#include "Entity.h"
#include "Animal.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>


// World class manages terrain blocks, animals and zombies
class World {
private:
    std::vector<std::vector<std::string>> blockMap; // 2D grid of blocks (strings represent block type)
    std::vector<std::shared_ptr<Animal>> animals; // List of animals in world
    std::vector<std::shared_ptr<Zombie>> zombies;  // List of zombies
public:
    World() { // Constructor
        blockMap.assign(WORLD_ROWS, std::vector<std::string>(WORLD_COLS, ""));
        buildTerrain(); // Generate terrain
        spawnDefaultEntities(); // Spawn animals/zombies
    }

    void buildTerrain() { // Generate terrain blocks
        for (int c = 0; c < WORLD_COLS; ++c) {
            blockMap[GROUND_ROW][c] = "Grass";
            blockMap[GROUND_ROW + 1][c] = "Dirt";
            blockMap[GROUND_ROW + 2][c] = "Dirt";
            blockMap[GROUND_ROW + 3][c] = "Stone";
            blockMap[GROUND_ROW + 4][c] = "Stone";
        }
        for (int r = GROUND_ROW - 2; r <= GROUND_ROW; ++r)
            blockMap[r][8] = (r == GROUND_ROW - 2) ? "Grass" : "Dirt";

        blockMap[GROUND_ROW - 1][15] = "Wood";
        blockMap[GROUND_ROW - 2][15] = "Wood";
        blockMap[GROUND_ROW - 1][16] = "Wood";

        for (int c = 20; c < 24; ++c)
            blockMap[GROUND_ROW - 3][c] = "Stone";
    }

    void spawnDefaultEntities() {
        // Spawn just above ground surface (GROUND_ROW - 1) * TILE_SIZE would be inside the tile.
        // Place entity so its bottom edge sits exactly on top of GROUND_ROW tile.
        float groundSurface = (GROUND_ROW * TILE_SIZE) - TILE_SIZE;  // top-left Y so feet touch ground

        animals.push_back(std::make_shared<Animal>(300.f, groundSurface));
        animals.push_back(std::make_shared<Animal>(550.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(900.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(1200.f, groundSurface));
    }

    std::string getBlock(int row, int col) const {  // Get block at location
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return "";
        return blockMap[row][col];
    }

    void removeBlock(int row, int col) { // Remove block
        if (row >= 0 && row < WORLD_ROWS && col >= 0 && col < WORLD_COLS)
            blockMap[row][col] = "";
    }

    bool placeBlock(int row, int col, const std::string& type) {  // Place block
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
        if (!blockMap[row][col].empty()) return false;
        blockMap[row][col] = type;
        return true;
    }

    bool isSolid(int row, int col) const {
        return !getBlock(row, col).empty();
    }

    bool isSolidAt(float px, float py) const {
        return isSolid((int)(py / TILE_SIZE), (int)(px / TILE_SIZE));
    }

    TileMap getTileMap() const {
        return { blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE };
    }

    void spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }
    void spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

    void removeDeadEntities() {  // Remove dead animals/zombies
        animals.erase(std::remove_if(animals.begin(), animals.end(),
            [](const auto& a) { return !a->isAlive(); }), animals.end());
        zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
            [](const auto& z) { return !z->isAlive(); }), zombies.end());
    }

    void update(float dt, Entity& player) { // Update entities
        TileMap tm = getTileMap();
        for (auto& a : animals) a->tick(dt, tm);               // use tick() not update()
        for (auto& z : zombies) z->chaseAndAttack(player, tm, dt);
        removeDeadEntities(); // Clean up anything killed during this frame
    }

    const std::vector<std::vector<std::string>>& getBlockMap()  const { return blockMap; }
    std::vector<std::shared_ptr<Animal>>& getAnimals() { return animals; }
    std::vector<std::shared_ptr<Zombie>>& getZombies() { return zombies; }
    const std::vector<std::shared_ptr<Animal>>& getAnimals()   const { return animals; }
    const std::vector<std::shared_ptr<Zombie>>& getZombies()   const { return zombies; }

    int getTileSize()  const { return TILE_SIZE; } // Return tile size in pixels
    int getGroundRow() const { return GROUND_ROW; } // Return the main ground row index
    int getCols()      const { return WORLD_COLS; } // Return total number of columns in the world
    int getRows()      const { return WORLD_ROWS; } // Return total number of rows in the world
};