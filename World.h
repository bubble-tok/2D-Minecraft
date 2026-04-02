/**
 * @file World.h
 * @brief Declares the World class — tile map owner, terrain generator, and entity simulator.
 *
 * @author Group 46
 */
#pragma once
#include "CollisionHelper.h"
#include "Entity.h"
#include "Animal.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <utility>

/**
 * @class World
 * @brief Owns the tile grid, all living entities, and world simulation logic.
 *
 * World is responsible for:
 * - Procedural terrain generation (buildTerrain): rolling hills, caves, gold veins, trees.
 * - Block read/write access with per-tile durability tracking (durabilityMap).
 * - Animal and Zombie entity simulation each frame.
 * - Dynamic zombie spawning that scales with player exploration progress.
 * - Exporting a lightweight TileMap view for use by the collision system.
 *
 * The block grid is a 2D vector of strings where an empty string represents air.
 *
 * @author Group 46
 */
class World {
public:
    /**
     * @brief Constructs the world: allocates the block grid, builds terrain,
     *        and spawns default entities.
     */
    World();

    /**
     * @brief Generates the entire terrain procedurally.
     *
     * Uses a sum-of-sines heightmap to create rolling hills, then layers
     * Grass, Dirt, and Stone. Drills winding cave tunnels and places gold
     * veins only on existing Stone tiles. Spawns 12 trees at fixed columns.
     */
    void buildTerrain();

    /**
     * @brief Spawns the initial set of Animals and Zombies at surface positions.
     */
    void spawnDefaultEntities();

    /**
     * @brief Returns the block type name at the given tile coordinates.
     * @param row Tile row (Y axis).
     * @param col Tile column (X axis).
     * @return Block type string, or "" if out-of-bounds or air.
     */
    std::string getBlock(int row, int col) const;

    /**
     * @brief Clears the block at (row, col) unconditionally.
     * @param row Tile row.
     * @param col Tile column.
     */
    void removeBlock(int row, int col);

    /**
     * @brief Places a named block at (row, col) if the tile is currently empty.
     * @param row  Tile row.
     * @param col  Tile column.
     * @param type Block type name to place.
     * @return True if placed; false if the tile was already occupied or out-of-bounds.
     */
    bool placeBlock(int row, int col, const std::string& type);

    /**
     * @brief Returns true if (row, col) contains any non-empty block.
     * @param row Tile row.
     * @param col Tile column.
     * @return True if the tile is occupied.
     */
    bool isSolid(int row, int col) const;

    /**
     * @brief Returns true if the world position (px, py) maps to a solid tile.
     * @param px World X in pixels.
     * @param py World Y in pixels.
     * @return True if the tile at that pixel position is occupied.
     */
    bool isSolidAt(float px, float py) const;

    /**
     * @brief Deals one mining hit to the block at (row, col).
     *
     * Looks up the block's max durability on first hit and tracks remaining
     * hits in durabilityMap. When durability reaches 0, removes the block
     * and erases the durability entry.
     *
     * @param row Tile row.
     * @param col Tile column.
     * @return True if this hit destroyed the block; false if it survived.
     */
    bool hitBlock(int row, int col);

    /**
     * @brief Returns the remaining durability hits for the block at (row, col).
     *
     * If no hits have been taken yet, returns the maximum durability.
     *
     * @param row Tile row.
     * @param col Tile column.
     * @return Hits remaining before the block is destroyed.
     */
    int getBlockDurability(int row, int col) const;

    /**
     * @brief Returns the maximum durability (total hits to break) for a block type.
     * @param type Block type name.
     * @return Max hit count; defaults to 3 for unknown types.
     */
    int blockMaxDurability(const std::string& type) const;

    /**
     * @brief Constructs and returns a lightweight TileMap view for collision.
     *
     * The returned TileMap holds a const reference to blockMap — it must not
     * outlive the World object.
     *
     * @return TileMap struct referencing the internal block grid.
     */
    TileMap getTileMap() const;

    /**
     * @brief Adds an Animal to the world.
     * @param a Shared pointer to the Animal to add.
     */
    void spawnAnimal(std::shared_ptr<Animal> a);

    /**
     * @brief Adds a Zombie to the world.
     * @param z Shared pointer to the Zombie to add.
     */
    void spawnZombie(std::shared_ptr<Zombie> z);

    /**
     * @brief Removes all dead entities from the animal and zombie vectors.
     *
     * Called after combat resolution each frame to keep the entity lists clean.
     */
    void removeDeadEntities();

    /**
     * @brief Simulates all entities for one frame and handles zombie spawning.
     *
     * Ticks all Animal and Zombie AI, records whether any zombie hit the player
     * this frame (wasHitByZombie()), removes dead entities, and attempts a
     * dynamic zombie spawn based on a timer and player position.
     *
     * @param dt     Delta time in seconds.
     * @param player The player entity (used for zombie AI targeting and spawn position).
     */
    void update(float dt, Entity& player);

    /**
     * @brief Returns true if any zombie dealt damage to the player last frame.
     *
     * Read by Game::tickCombat() to trigger screen shake and iframe window.
     *
     * @return True if a zombie hit landed in the most recent update() call.
     */
    bool wasHitByZombie() const { return zombieHitThisFrame; }

    /**
     * @brief Returns the tile row of the terrain surface at the given column.
     *
     * Uses the same sum-of-sines formula as buildTerrain() to reconstruct
     * the surface height without storing it separately.
     *
     * @param col Tile column index.
     * @return Tile row of the topmost solid terrain tile at this column.
     */
    int surfaceRowAt(int col) const;

    /** @brief Returns a const reference to the full block grid. */
    const std::vector<std::vector<std::string>>& getBlockMap() const;

    /** @brief Returns a mutable reference to the animal list. */
    std::vector<std::shared_ptr<Animal>>& getAnimals();

    /** @brief Returns a mutable reference to the zombie list. */
    std::vector<std::shared_ptr<Zombie>>& getZombies();

    /** @brief Returns a const reference to the animal list. */
    const std::vector<std::shared_ptr<Animal>>& getAnimals() const;

    /** @brief Returns a const reference to the zombie list. */
    const std::vector<std::shared_ptr<Zombie>>& getZombies() const;

    /** @brief Returns the tile size in pixels. */
    int getTileSize()  const { return TILE_SIZE; }

    /** @brief Returns the nominal ground row index. */
    int getGroundRow() const { return GROUND_ROW; }

    /** @brief Returns the world width in tiles. */
    int getCols()      const { return WORLD_COLS; }

    /** @brief Returns the world height in tiles. */
    int getRows()      const { return WORLD_ROWS; }

private:
    std::vector<std::vector<std::string>> blockMap;   ///< 2D tile grid; "" = air.
    std::vector<std::shared_ptr<Animal>>  animals;    ///< All living passive animals.
    std::vector<std::shared_ptr<Zombie>>  zombies;    ///< All living hostile zombies.
    std::map<std::pair<int,int>, int>     durabilityMap; ///< Per-tile remaining hits.

    float spawnTimer         = 0.f;  ///< Accumulates time between zombie spawns.
    float spawnInterval      = 8.f;  ///< Base seconds between spawns (decreases further right).
    int   maxZombies         = 12;   ///< Maximum concurrent zombies in the world.
    bool  zombieHitThisFrame = false;///< Set to true if a zombie hit landed this update().

    /**
     * @brief Places a tree at the given column using the provided ground heights.
     *
     * Plants a Wood trunk from the surface upward and places layered Leaves
     * in a diamond/pyramid pattern above the trunk tip.
     *
     * @param col     Tile column for the trunk base.
     * @param height  Number of trunk tiles (determines tree height).
     * @param groundY Per-column surface row array from buildTerrain().
     */
    void placeTree(int col, int height, const std::vector<int>& groundY);

    /**
     * @brief Attempts to spawn a Zombie ahead of the player's current position.
     *
     * Spawns 10–15 tiles to the right of the player if the zombie cap
     * has not been reached.
     *
     * @param playerX Player's current world X position in pixels.
     */
    void trySpawnZombie(float playerX);
};
