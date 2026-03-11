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
 * @brief Manages the game world, including terrain blocks, animals, and zombies.
 *
 * The World class stores the tile-based terrain, handles entity spawning,
 * block placement/removal, collision-related tile queries, and updates
 * all world entities each frame.
 */
class World {
private:
    std::vector<std::vector<std::string>> blockMap; ///< 2D grid of terrain blocks where each string stores a block type
    std::vector<std::shared_ptr<Animal>> animals;   ///< List of animals currently in the world
    std::vector<std::shared_ptr<Zombie>> zombies;   ///< List of zombies currently in the world

public:
    /**
     * @brief Constructs the world.
     *
     * Initializes the block map, generates terrain, and spawns
     * the default world entities.
     */
    World() {
        blockMap.assign(WORLD_ROWS, std::vector<std::string>(WORLD_COLS, ""));
        buildTerrain();
        spawnDefaultEntities();
    }

    /**
     * @brief Generates the default terrain layout.
     *
     * Fills the world grid with base terrain layers such as grass,
     * dirt, stone, and a few predefined block structures.
     */
    void buildTerrain() {
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

    /**
     * @brief Spawns the default animals and zombies in the world.
     *
     * Entities are positioned so their bottom edges rest directly
     * on top of the ground surface.
     */
    void spawnDefaultEntities() {
        float groundSurface = (GROUND_ROW * TILE_SIZE) - TILE_SIZE;

        animals.push_back(std::make_shared<Animal>(300.f, groundSurface));
        animals.push_back(std::make_shared<Animal>(550.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(900.f, groundSurface));
        zombies.push_back(std::make_shared<Zombie>(1200.f, groundSurface));
    }

    /**
     * @brief Gets the block type at a specific tile coordinate.
     *
     * @param row Row index of the block
     * @param col Column index of the block
     * @return Block type as a string, or an empty string if out of bounds
     */
    std::string getBlock(int row, int col) const {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return "";
        return blockMap[row][col];
    }

    /**
     * @brief Removes a block from the world.
     *
     * @param row Row index of the block
     * @param col Column index of the block
     */
    void removeBlock(int row, int col) {
        if (row >= 0 && row < WORLD_ROWS && col >= 0 && col < WORLD_COLS)
            blockMap[row][col] = "";
    }

    /**
     * @brief Places a block in the world.
     *
     * Placement only succeeds if the target position is within bounds
     * and currently empty.
     *
     * @param row Row index where the block should be placed
     * @param col Column index where the block should be placed
     * @param type Block type to place
     * @return True if the block was placed successfully, otherwise false
     */
    bool placeBlock(int row, int col, const std::string& type) {
        if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
        if (!blockMap[row][col].empty()) return false;
        blockMap[row][col] = type;
        return true;
    }

    /**
     * @brief Checks whether a tile contains a solid block.
     *
     * @param row Row index of the tile
     * @param col Column index of the tile
     * @return True if the tile is solid, otherwise false
     */
    bool isSolid(int row, int col) const {
        return !getBlock(row, col).empty();
    }

    /**
     * @brief Checks whether a world position lies inside a solid tile.
     *
     * @param px X-coordinate in world space
     * @param py Y-coordinate in world space
     * @return True if the corresponding tile is solid, otherwise false
     */
    bool isSolidAt(float px, float py) const {
        return isSolid((int)(py / TILE_SIZE), (int)(px / TILE_SIZE));
    }

    /**
     * @brief Creates a TileMap view of the world for collision handling.
     *
     * @return TileMap structure containing block grid and dimensions
     */
    TileMap getTileMap() const {
        return { blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE };
    }

    /**
     * @brief Spawns an animal into the world.
     *
     * @param a Shared pointer to the animal to add
     */
    void spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }

    /**
     * @brief Spawns a zombie into the world.
     *
     * @param z Shared pointer to the zombie to add
     */
    void spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

    /**
     * @brief Removes all dead animals and zombies from the world.
     */
    void removeDeadEntities() {
        animals.erase(std::remove_if(animals.begin(), animals.end(),
            [](const auto& a) { return !a->isAlive(); }), animals.end());

        zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
            [](const auto& z) { return !z->isAlive(); }), zombies.end());
    }

    /**
     * @brief Updates all world entities for the current frame.
     *
     * Animals perform wandering behavior, zombies chase and attack
     * the player, and dead entities are removed afterward.
     *
     * @param dt Time elapsed since the last frame
     * @param player Reference to the player entity
     */
    void update(float dt, Entity& player) {
        TileMap tm = getTileMap();

        for (auto& a : animals)
            a->tick(dt, tm);

        for (auto& z : zombies)
            z->chaseAndAttack(player, tm, dt);

        removeDeadEntities();
    }

    /**
     * @brief Gets the world block map.
     *
     * @return Constant reference to the 2D block grid
     */
    const std::vector<std::vector<std::string>>& getBlockMap() const { return blockMap; }

    /**
     * @brief Gets the mutable list of animals.
     *
     * @return Reference to the animal container
     */
    std::vector<std::shared_ptr<Animal>>& getAnimals() { return animals; }

    /**
     * @brief Gets the mutable list of zombies.
     *
     * @return Reference to the zombie container
     */
    std::vector<std::shared_ptr<Zombie>>& getZombies() { return zombies; }

    /**
     * @brief Gets the read-only list of animals.
     *
     * @return Constant reference to the animal container
     */
    const std::vector<std::shared_ptr<Animal>>& getAnimals() const { return animals; }

    /**
     * @brief Gets the read-only list of zombies.
     *
     * @return Constant reference to the zombie container
     */
    const std::vector<std::shared_ptr<Zombie>>& getZombies() const { return zombies; }

    /**
     * @brief Gets the tile size in pixels.
     *
     * @return Tile size
     */
    int getTileSize() const { return TILE_SIZE; }

    /**
     * @brief Gets the index of the main ground row.
     *
     * @return Ground row index
     */
    int getGroundRow() const { return GROUND_ROW; }

    /**
     * @brief Gets the total number of world columns.
     *
     * @return Number of columns
     */
    int getCols() const { return WORLD_COLS; }

    /**
     * @brief Gets the total number of world rows.
     *
     * @return Number of rows
     */
    int getRows() const { return WORLD_ROWS; }
};