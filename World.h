#pragma once
#include "CollisionHelper.h"
#include "Entity.h"
#include "Animal.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <utility>

/**
 * @class World
 * @brief Represents the game world including the tile map and all entities.
 *
 * Manages block placement, terrain generation, entity spawning, and
 * per-frame updates. Supports dynamic zombie spawning based on how
 * far the player has explored.
 */
class World {
private:
    std::vector<std::vector<std::string>> blockMap; ///< 2D grid of block type names
    std::vector<std::shared_ptr<Animal>> animals;   ///< Living animals
    std::vector<std::shared_ptr<Zombie>> zombies;   ///< Living zombies

    /// Remaining hits per tile before it breaks. Populated on first hit.
    std::map<std::pair<int,int>, int> durabilityMap;

    float spawnTimer    = 0.f;
    float spawnInterval = 8.f;
    int   maxZombies    = 12;

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
     * @brief Builds the initial terrain across the full world width.
     *
     * Generates ground layers, deep stone, cave sections, above-ground
     * structures, and scatters Gold ore for the player to discover.
     */
    void buildTerrain() {
        // Full ground stack across entire world
        for (int c = 0; c < WORLD_COLS; ++c) {
            blockMap[GROUND_ROW][c]      = "Grass";
            blockMap[GROUND_ROW + 1][c]  = "Dirt";
            blockMap[GROUND_ROW + 2][c]  = "Dirt";
            blockMap[GROUND_ROW + 3][c]  = "Stone";
            blockMap[GROUND_ROW + 4][c]  = "Stone";
            blockMap[GROUND_ROW + 5][c]  = "Stone";
            blockMap[GROUND_ROW + 6][c]  = "Stone";
            blockMap[GROUND_ROW + 7][c]  = "Stone";
            blockMap[GROUND_ROW + 8][c]  = "Stone";
            blockMap[GROUND_ROW + 9][c]  = "Stone";
            blockMap[GROUND_ROW + 10][c] = "Stone";
        }

        // Small hill near start
        for (int r = GROUND_ROW - 2; r <= GROUND_ROW; ++r)
            blockMap[r][8] = (r == GROUND_ROW - 2) ? "Grass" : "Dirt";

        // Wooden structure near start
        blockMap[GROUND_ROW - 1][15] = "Wood";
        blockMap[GROUND_ROW - 2][15] = "Wood";
        blockMap[GROUND_ROW - 1][16] = "Wood";

        // Stone platform
        for (int c = 20; c < 24; ++c)
            blockMap[GROUND_ROW - 3][c] = "Stone";

        // Cave 1: mid-world shallow cave
        for (int r = GROUND_ROW + 3; r <= GROUND_ROW + 6; ++r)
            for (int c = 35; c <= 50; ++c)
                blockMap[r][c] = "";

        // Cave 2: far right, deeper
        for (int r = GROUND_ROW + 4; r <= GROUND_ROW + 8; ++r)
            for (int c = 75; c <= 95; ++c)
                blockMap[r][c] = "";

        // Cave 3: very deep, far right
        for (int r = GROUND_ROW + 6; r <= GROUND_ROW + 10; ++r)
            for (int c = 100; c <= 115; ++c)
                blockMap[r][c] = "";

        // Gold ore — scattered at increasing depth further right
        blockMap[GROUND_ROW + 3][10]  = "Gold";
        blockMap[GROUND_ROW + 4][12]  = "Gold";
        blockMap[GROUND_ROW + 2][28]  = "Gold";
        blockMap[GROUND_ROW + 3][29]  = "Gold";
        blockMap[GROUND_ROW + 4][27]  = "Gold";
        blockMap[GROUND_ROW + 4][70]  = "Gold";
        blockMap[GROUND_ROW + 5][72]  = "Gold";
        blockMap[GROUND_ROW + 6][68]  = "Gold";
        blockMap[GROUND_ROW + 7][105] = "Gold";
        blockMap[GROUND_ROW + 8][110] = "Gold";

        // Second wooden structure mid-world
        for (int r = GROUND_ROW - 3; r <= GROUND_ROW - 1; ++r)
            blockMap[r][55] = "Wood";
        blockMap[GROUND_ROW - 3][56] = "Wood";

        // Stone arch far right
        for (int c = 80; c <= 84; ++c)
            blockMap[GROUND_ROW - 2][c] = "Stone";
        blockMap[GROUND_ROW - 3][80] = "Stone";
        blockMap[GROUND_ROW - 3][84] = "Stone";
    }

    /**
     * @brief Spawns the initial set of animals and zombies spread across the world.
     */
    void spawnDefaultEntities() {
        float gs = (GROUND_ROW * TILE_SIZE) - TILE_SIZE;

        animals.push_back(std::make_shared<Animal>(300.f,  gs));
        animals.push_back(std::make_shared<Animal>(550.f,  gs));
        animals.push_back(std::make_shared<Animal>(1400.f, gs));
        animals.push_back(std::make_shared<Animal>(2200.f, gs));

        zombies.push_back(std::make_shared<Zombie>(900.f,  gs));
        zombies.push_back(std::make_shared<Zombie>(1200.f, gs));
        zombies.push_back(std::make_shared<Zombie>(2000.f, gs));
        zombies.push_back(std::make_shared<Zombie>(3000.f, gs));
    }

    /**
     * @brief Attempts to spawn a zombie ahead of the player.
     *
     * Spawns off-screen ahead of the player. In cave zones,
     * zombies spawn at cave floor level for extra difficulty.
     *
     * @param playerX Player's current world X coordinate
     */
    void trySpawnZombie(float playerX) {
        if ((int)zombies.size() >= maxZombies) return;

        float gs = (GROUND_ROW * TILE_SIZE) - TILE_SIZE;
        float spawnX = playerX + 400.f + (rand() % 200);
        if (spawnX > (WORLD_COLS - 2) * TILE_SIZE) return;

        float spawnY = gs;
        int col = (int)(spawnX / TILE_SIZE);
        if (col >= 35 && col <= 95)
            spawnY = (GROUND_ROW + 5) * TILE_SIZE - TILE_SIZE;

        zombies.push_back(std::make_shared<Zombie>(spawnX, spawnY));
    }

public:
    /**
     * @brief Returns the default max durability for a block type.
     * Matches the values used in BlockItems namespace.
     */
    int blockMaxDurability(const std::string& type) const {
        if (type == "Grass")  return 2;
        if (type == "Dirt")   return 2;
        if (type == "Sand")   return 2;
        if (type == "Wood")   return 5;
        if (type == "Stone")  return 8;
        if (type == "Gold")   return 8;
        return 3;
    }

    /**
     * @brief Hits a block once, reducing its durability.
     *
     * Initialises durability from blockMaxDurability() on first hit.
     * Removes the block and clears its durability entry when it reaches zero.
     *
     * @param row Tile row index
     * @param col Tile column index
     * @return True if the block was destroyed by this hit
     */
    bool hitBlock(int row, int col) {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
        if (blockMap[row][col].empty()) return false;

        auto key = std::make_pair(row, col);
        if (durabilityMap.find(key) == durabilityMap.end())
            durabilityMap[key] = blockMaxDurability(blockMap[row][col]);

        durabilityMap[key]--;
        if (durabilityMap[key] <= 0) {
            blockMap[row][col] = "";
            durabilityMap.erase(key);
            return true;
        }
        return false;
    }

    /**
     * @brief Returns remaining durability hits for a tile, or max if untouched.
     */
    int getBlockDurability(int row, int col) const {
        auto key = std::make_pair(row, col);
        auto it  = durabilityMap.find(key);
        if (it != durabilityMap.end()) return it->second;
        const std::string& type = getBlock(row, col);
        if (type.empty()) return 0;
        return blockMaxDurability(type);
    }

    /**
     * @brief Returns the block type at a given tile position.
     * @param row Tile row index
     * @param col Tile column index
     * @return Block name, or empty string if out of bounds or air
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
     * @param type Block type name
     * @return True if placed successfully, false if occupied or out of bounds
     */
    bool placeBlock(int row, int col, const std::string& type) {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
        if (!blockMap[row][col].empty()) return false;
        blockMap[row][col] = type;
        return true;
    }

    /** @brief Returns true if the tile at (row, col) is solid. */
    bool isSolid(int row, int col) const { return !getBlock(row, col).empty(); }

    /** @brief Returns true if the world-space position is inside a solid tile. */
    bool isSolidAt(float px, float py) const {
        return isSolid((int)(py / TILE_SIZE), (int)(px / TILE_SIZE));
    }

    /** @brief Returns a TileMap snapshot for use in collision resolution. */
    TileMap getTileMap() const {
        return { blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE };
    }

    /** @brief Spawns an animal into the world. */
    void spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }

    /** @brief Spawns a zombie into the world. */
    void spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

    /** @brief Removes all dead entities from the world. */
    void removeDeadEntities() {
        animals.erase(std::remove_if(animals.begin(), animals.end(),
            [](const auto& a) { return !a->isAlive(); }), animals.end());
        zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
            [](const auto& z) { return !z->isAlive(); }), zombies.end());
    }

    /**
     * @brief Updates all entities and handles dynamic zombie spawning.
     *
     * Zombie spawn interval shrinks the further right the player is,
     * making the game progressively harder.
     *
     * @param dt Delta time in seconds
     * @param player Reference to the player entity
     */
    void update(float dt, Entity& player) {
        TileMap tm = getTileMap();
        for (auto& a : animals) a->tick(dt, tm);
        for (auto& z : zombies) z->chaseAndAttack(player, tm, dt);
        removeDeadEntities();

        // Spawn interval shrinks from 8s at start to 3s at far right
        float progress = player.getX() / (WORLD_COLS * TILE_SIZE);
        float interval = 8.f - (progress * 5.f);
        interval = std::max(3.f, interval);

        spawnTimer += dt;
        if (spawnTimer >= interval) {
            spawnTimer = 0.f;
            trySpawnZombie(player.getX());
        }
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
