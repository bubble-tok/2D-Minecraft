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
 * @brief Owns the tile map, entities, terrain generation, and simulation.
 */
class World {
public:
    World();

    void buildTerrain();
    void spawnDefaultEntities();

    // Block access
    std::string getBlock(int row, int col) const;
    void        removeBlock(int row, int col);
    bool        placeBlock(int row, int col, const std::string& type);
    bool        isSolid(int row, int col) const;
    bool        isSolidAt(float px, float py) const;
    bool        hitBlock(int row, int col);
    int         getBlockDurability(int row, int col) const;
    int         blockMaxDurability(const std::string& type) const;

    TileMap getTileMap() const;

    // Entities
    void spawnAnimal(std::shared_ptr<Animal> a);
    void spawnZombie(std::shared_ptr<Zombie> z);
    void removeDeadEntities();
    void update(float dt, Entity& player);
    bool wasHitByZombie() const;

    // Terrain helpers
    int surfaceRowAt(int col) const;

    // Accessors
    const std::vector<std::vector<std::string>>& getBlockMap() const;
    std::vector<std::shared_ptr<Animal>>& getAnimals();
    std::vector<std::shared_ptr<Zombie>>& getZombies();
    const std::vector<std::shared_ptr<Animal>>& getAnimals() const;
    const std::vector<std::shared_ptr<Zombie>>& getZombies() const;

    int getTileSize()  const;
    int getGroundRow() const;
    int getCols()      const;
    int getRows()      const;

    // Save/load helpers
    void setBlockMap(const std::vector<std::vector<std::string>>& bm);
    void clearEntitiesForLoad();
    void addAnimalForLoad(float x, float y, int hp,
                          const std::string& meatType, int meatAmount);
    void addZombieForLoad(float x, float y, int hp);

private:
    std::vector<std::vector<std::string>> blockMap;
    std::vector<std::shared_ptr<Animal>> animals;
    std::vector<std::shared_ptr<Zombie>> zombies;
    std::map<std::pair<int,int>, int>    durabilityMap;

    float spawnTimer         = 0.f;
    float spawnInterval      = 8.f;
    int   maxZombies         = 12;
    bool  zombieHitThisFrame = false;

    void placeTree(int col, int height, const std::vector<int>& groundY);
    void trySpawnZombie(float playerX);

    /**
     * @brief Pushes overlapping entities apart so they never visually stack.
     *
     * After all AI movement has been resolved each frame, iterates every
     * pair of living entities (animals vs animals, zombies vs zombies,
     * animals vs zombies, and entities vs player) and applies a horizontal
     * push-out if their centres are closer than ENTITY_SEPARATION_DIST.
     * Each entity is pushed by half the penetration depth in opposite
     * directions, keeping the separation symmetric.
     *
     * @param player The player entity (also participates in separation).
     */
    void separateEntities(Entity& player);
};
