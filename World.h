#pragma once
#include "CollisionHelper.h"
#include "Entity.h"
#include "Animal.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

/**
 * @class World
 * @brief Represents the game world including the tile map and all entities.
 *
 * Manages block placement, entity spawning, and per-frame updates
 * for animals and zombies. Also provides collision map access for physics.
 */
class World {
private:
    std::vector<std::vector<std::string>> blockMap; ///< 2D grid of block type names
    std::vector<std::shared_ptr<Animal>> animals;   ///< Living animals in the world
    std::vector<std::shared_ptr<Zombie>> zombies;   ///< Living zombies in the world

public:
    /**
     * @brief Constructs the world, builds terrain, and spawns default entities.
     */
    World() {
        blockMap.assign(WORLD_ROWS, std::vector<std::string>(WORLD_COLS, ""));
        buildTerrain();
        spawnDefaultEntities();
    }

    /**
     * @brief Builds the initial terrain layout.
     *
     * Places grass, dirt, and stone layers, adds above-ground structures,
     * and scatters Gold ore underground for the player to discover.
     */
    void buildTerrain() {
        // Ground layers across the whole world
        for (int c = 0; c < WORLD_COLS; ++c) {
            blockMap[GROUND_ROW][c]     = "Grass";
            blockMap[GROUND_ROW + 1][c] = "Dirt";
            blockMap[GROUND_ROW + 2][c] = "Dirt";
            blockMap[GROUND_ROW + 3][c] = "Stone";
            blockMap[GROUND_ROW + 4][c] = "Stone";
        }

        // Small hill at column 8
        for (int r = GROUND_ROW - 2; r <= GROUND_ROW; ++r)
            blockMap[r][8] = (r == GROUND_ROW - 2) ? "Grass" : "Dirt";

        // Wooden structure
        blockMap[GROUND_ROW - 1][15] = "Wood";
        blockMap[GROUND_ROW - 2][15] = "Wood";
        blockMap[GROUND_ROW - 1][16] = "Wood";

        // Stone platform
        for (int c = 20; c < 24; ++c)
            blockMap[GROUND_ROW - 3][c] = "Stone";

        // Gold ore scattered underground — mine down to find it!
        blockMap[GROUND_ROW + 2][28] = "Gold";
        blockMap[GROUND_ROW + 3][29] = "Gold";
        blockMap[GROUND_ROW + 4][27] = "Gold";
        blockMap[GROUND_ROW + 3][10] = "Gold";
        blockMap[GROUND_ROW + 4][35] = "Gold";
    }

    /**
     * @brief Spawns the default set of animals and zombies.
     *
     * Entities are placed so their feet sit exactly on the ground surface.
     */
    void spawnDefaultEntities() {
        float groundSurface = (GROUND_ROW * TILE_SIZE) - TILE_SIZE;

        animals.push_back(std::make_shared<Animal>(300.f, groundSurface));
        animals.push_back(std::make_shared<Animal>(550.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(900.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(1200.f, groundSurface));
    }

    /**
     * @brief Returns the block type at a given tile position.
     * @param row Tile row index
     * @param col Tile column index
     * @return Block name string, or empty string if out of bounds or empty
     */
    std::string getBlock(int row, int col) const {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return "";
        return blockMap[row][col];
    }

    /**
     * @brief Removes the block at a given tile position.
     * @param row Tile row index
     * @param col Tile column index
     */
    void removeBlock(int row, int col) {
        if (row >= 0 && row < WORLD_ROWS && col >= 0 && col < WORLD_COLS)
            blockMap[row][col] = "";
    }

    /**
     * @brief Places a block at a given tile position if the cell is empty.
     * @param row Tile row index
     * @param col Tile column index
     * @param type Block type name to place
     * @return True if placement succeeded, false if out of bounds or occupied
     */
    bool placeBlock(int row, int col, const std::string& type) {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
        if (!blockMap[row][col].empty()) return false;
        blockMap[row][col] = type;
        return true;
    }

    /**
     * @brief Returns true if the tile at the given position is solid.
     * @param row Tile row index
     * @param col Tile column index
     */
    bool isSolid(int row, int col) const {
        return !getBlock(row, col).empty();
    }

    /**
     * @brief Returns true if the world-space position falls inside a solid tile.
     * @param px World X coordinate
     * @param py World Y coordinate
     */
    bool isSolidAt(float px, float py) const {
        return isSolid((int)(py / TILE_SIZE), (int)(px / TILE_SIZE));
    }

    /**
     * @brief Returns a TileMap snapshot for use in collision resolution.
     */
    TileMap getTileMap() const {
        return { blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE };
    }

    /**
     * @brief Spawns an animal into the world.
     * @param a Shared pointer to the animal
     */
    void spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }

    /**
     * @brief Spawns a zombie into the world.
     * @param z Shared pointer to the zombie
     */
    void spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

    /**
     * @brief Removes all dead entities from the world.
     */
    void removeDeadEntities() {
        animals.erase(std::remove_if(animals.begin(), animals.end(),
            [](const auto& a) { return !a->isAlive(); }), animals.end());
        zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
            [](const auto& z) { return !z->isAlive(); }), zombies.end());
    }

    /**
     * @brief Updates all entities in the world each frame.
     * @param dt Delta time in seconds
     * @param player Reference to the player entity
     */
    void update(float dt, Entity& player) {
        TileMap tm = getTileMap();
        for (auto& a : animals) a->tick(dt, tm);
        for (auto& z : zombies) z->chaseAndAttack(player, tm, dt);
        removeDeadEntities();
    }

    const std::vector<std::vector<std::string>>& getBlockMap() const { return blockMap; }
    std::vector<std::shared_ptr<Animal>>& getAnimals() { return animals; }
    std::vector<std::shared_ptr<Zombie>>& getZombies() { return zombies; }
    const std::vector<std::shared_ptr<Animal>>& getAnimals() const { return animals; }
    const std::vector<std::shared_ptr<Zombie>>& getZombies() const { return zombies; }

    int getTileSize()  const { return TILE_SIZE; }
    int getGroundRow() const { return GROUND_ROW; }
    int getCols()      const { return WORLD_COLS; }
    int getRows()      const { return WORLD_ROWS; }
};
