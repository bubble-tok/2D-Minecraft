/**
 * @file World.cpp
 * @brief Implements terrain generation, block management, and entity simulation.
 *
 * @author Group 46
 */
#include "World.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

World::World() {
    // Initialise every tile to air ("")
    blockMap.assign(WORLD_ROWS, std::vector<std::string>(WORLD_COLS, ""));
    buildTerrain();
    spawnDefaultEntities();
}

int World::blockMaxDurability(const std::string& type) const {
    // Soft materials break in 2 hits; wood in 5; stone/gold in 8
    if (type == "Grass")  return 2;
    if (type == "Dirt")   return 2;
    if (type == "Sand")   return 2;
    if (type == "Wood")   return 5;
    if (type == "Stone")  return 8;
    if (type == "Gold")   return 8;
    return 3; // default for placed blocks (CraftingTable, Bed, Campfire, etc.)
}

bool World::hitBlock(int row, int col) {
    if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
    if (blockMap[row][col].empty()) return false;

    auto key = std::make_pair(row, col);

    // Initialise durability on first hit
    if (durabilityMap.find(key) == durabilityMap.end())
        durabilityMap[key] = blockMaxDurability(blockMap[row][col]);

    if (--durabilityMap[key] <= 0) {
        // Block fully mined — remove from grid and durability tracker
        blockMap[row][col] = "";
        durabilityMap.erase(key);
        return true;
    }
    return false; // block still standing
}

int World::getBlockDurability(int row, int col) const {
    auto key = std::make_pair(row, col);
    auto it  = durabilityMap.find(key);
    if (it != durabilityMap.end()) return it->second;
    // No hits recorded yet — return the full durability for this block type
    const std::string& type = getBlock(row, col);
    if (type.empty()) return 0;
    return blockMaxDurability(type);
}

int World::surfaceRowAt(int col) const {
    // Reproduce the same height formula used in buildTerrain()
    float fx = (float)col / WORLD_COLS;
    float h  =  2.0f * std::sin(fx * 6.28f * 1.5f)
              + 1.0f * std::sin(fx * 6.28f * 3.7f)
              + 0.5f * std::sin(fx * 6.28f * 8.1f);
    int r = GROUND_ROW + (int)std::round(h * 0.7f);
    return std::max(GROUND_ROW - 2, std::min(GROUND_ROW + 2, r));
}

void World::placeTree(int col, int height, const std::vector<int>& groundY) {
    if (col < 0 || col >= WORLD_COLS) return;
    int base = groundY[col] - 1; // one tile above the surface

    // Place wood trunk tiles from base upward
    for (int r = base; r > base - height; --r)
        if (r >= 0 && blockMap[r][col].empty())
            blockMap[r][col] = "Wood";

    int top = base - height; // topmost trunk tile

    // Single-wide tip
    if (top >= 0) blockMap[top][col] = "Leaves";

    // 3-wide row below tip
    if (top + 1 >= 0)
        for (int dc = -1; dc <= 1; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS)
                blockMap[top+1][col+dc] = "Leaves";

    // 5-wide canopy rows
    if (top + 2 >= 0)
        for (int dc = -2; dc <= 2; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS)
                blockMap[top+2][col+dc] = "Leaves";

    // Sparse bottom leaves row (don't overwrite existing blocks)
    if (top + 3 >= 0)
        for (int dc = -2; dc <= 2; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS && blockMap[top+3][col+dc].empty())
                blockMap[top+3][col+dc] = "Leaves";
}

void World::buildTerrain() {
    // ── Step 1: compute surface height for every column ───────────────────────
    std::vector<int> groundY(WORLD_COLS);
    for (int c = 0; c < WORLD_COLS; ++c) {
        float fx = (float)c / WORLD_COLS;
        // Three-frequency sum-of-sines gives organic rolling hills
        float h =  2.0f * std::sin(fx * 6.28f * 1.5f)
                 + 1.0f * std::sin(fx * 6.28f * 3.7f)
                 + 0.5f * std::sin(fx * 6.28f * 8.1f);
        groundY[c] = GROUND_ROW + (int)std::round(h * 0.7f);
        groundY[c] = std::max(GROUND_ROW-2, std::min(GROUND_ROW+2, groundY[c]));
    }

    // ── Step 2: fill block grid from the surface downward ────────────────────
    for (int c = 0; c < WORLD_COLS; ++c) {
        int surf = groundY[c];
        for (int r = 0; r < WORLD_ROWS; ++r) {
            if      (r < surf)        blockMap[r][c] = "";       // air
            else if (r == surf)       blockMap[r][c] = "Grass";  // surface
            else if (r <= surf + 2)   blockMap[r][c] = "Dirt";   // topsoil
            else                      blockMap[r][c] = "Stone";  // bedrock layer
        }
    }

    // ── Step 3: carve winding cave tunnels ────────────────────────────────────
    auto drillCave = [&](int startR, int startC, int length) {
        int r = startR, c = startC, dr = 0;
        for (int i = 0; i < length; ++i) {
            // Carve a 2-tile-tall passage
            for (int rr = r; rr <= r+1; ++rr)
                if (rr >= 0 && rr < WORLD_ROWS && c >= 0 && c < WORLD_COLS)
                    blockMap[rr][c] = "";
            // Randomly adjust vertical direction every 5 tiles
            if (i % 5 == 0) {
                int newDr = dr + (rand() % 3 - 1);
                newDr = std::max(-1, std::min(1, newDr));
                if (r + newDr > groundY[c] + 3) dr = newDr;
            }
            r = std::max(groundY[c]+3, std::min(WORLD_ROWS-2, r+dr));
            c = std::max(0, std::min(WORLD_COLS-1, c+1));
            if (c >= WORLD_COLS-1) break;
        }
    };

    // Four caves spread across the map at increasing depth
    drillCave(GROUND_ROW+4,  28, 22);
    drillCave(GROUND_ROW+5,  55, 28);
    drillCave(GROUND_ROW+7,  85, 25);
    drillCave(GROUND_ROW+8, 100, 15);

    // ── Step 4: place gold veins (only on existing Stone tiles) ───────────────
    auto placeGold = [&](int r, int c) {
        if (r >= 0 && r < WORLD_ROWS && c >= 0 && c < WORLD_COLS
            && blockMap[r][c] == "Stone")
            blockMap[r][c] = "Gold";
    };
    // Scattered veins across left, middle, and right sections of the map
    placeGold(GROUND_ROW+3,15); placeGold(GROUND_ROW+3,16);
    placeGold(GROUND_ROW+4,22);
    placeGold(GROUND_ROW+5,40); placeGold(GROUND_ROW+5,41);
    placeGold(GROUND_ROW+6,40);
    placeGold(GROUND_ROW+5,60); placeGold(GROUND_ROW+6,61);
    placeGold(GROUND_ROW+7,78); placeGold(GROUND_ROW+7,79);
    placeGold(GROUND_ROW+8,79);
    placeGold(GROUND_ROW+7,98); placeGold(GROUND_ROW+8,99);
    placeGold(GROUND_ROW+8,100);

    // ── Step 5: plant trees at fixed columns ──────────────────────────────────
    static const int treeCols[]    = {4,14,24,35,46,57,67,77,87,97,107,115};
    static const int treeHeights[] = {4, 3, 5, 4, 3, 5, 4, 3, 5, 4,  3,  4};
    for (int i = 0; i < 12; ++i)
        placeTree(treeCols[i], treeHeights[i], groundY);
}

void World::spawnDefaultEntities() {
    // Helper: world-Y pixel just above the surface at the given column
    auto groundPx = [&](int col) -> float {
        return (float)(surfaceRowAt(col) * TILE_SIZE) - TILE_SIZE;
    };

    // Five passive animals spread across the map
    animals.push_back(std::make_shared<Animal>(  7*TILE_SIZE, groundPx( 7)));
    animals.push_back(std::make_shared<Animal>( 20*TILE_SIZE, groundPx(20)));
    animals.push_back(std::make_shared<Animal>( 50*TILE_SIZE, groundPx(50)));
    animals.push_back(std::make_shared<Animal>( 80*TILE_SIZE, groundPx(80)));
    animals.push_back(std::make_shared<Animal>(105*TILE_SIZE, groundPx(105)));

    // Four initial zombies at mid-to-far positions
    zombies.push_back(std::make_shared<Zombie>( 22*TILE_SIZE, groundPx(22)));
    zombies.push_back(std::make_shared<Zombie>( 45*TILE_SIZE, groundPx(45)));
    zombies.push_back(std::make_shared<Zombie>( 70*TILE_SIZE, groundPx(70)));
    zombies.push_back(std::make_shared<Zombie>( 90*TILE_SIZE, groundPx(90)));
}

void World::trySpawnZombie(float playerX) {
    if ((int)zombies.size() >= maxZombies) return; // zombie cap reached

    // Spawn 10-15 tiles ahead of the player so they approach from the right
    int col = (int)(playerX / TILE_SIZE) + 10 + rand() % 5;
    if (col >= WORLD_COLS - 2) return; // don't spawn outside the world

    float spawnY = (float)(surfaceRowAt(col) * TILE_SIZE) - TILE_SIZE;
    zombies.push_back(std::make_shared<Zombie>((float)col * TILE_SIZE, spawnY));
}

std::string World::getBlock(int row, int col) const {
    if (row<0||row>=WORLD_ROWS||col<0||col>=WORLD_COLS) return "";
    return blockMap[row][col];
}

void World::removeBlock(int row, int col) {
    if (row>=0&&row<WORLD_ROWS&&col>=0&&col<WORLD_COLS)
        blockMap[row][col] = "";
}

bool World::placeBlock(int row, int col, const std::string& type) {
    if (row<0||row>=WORLD_ROWS||col<0||col>=WORLD_COLS) return false;
    if (!blockMap[row][col].empty()) return false; // tile already occupied
    blockMap[row][col] = type;
    return true;
}

bool World::isSolid(int row, int col) const { return !getBlock(row,col).empty(); }

bool World::isSolidAt(float px, float py) const {
    return isSolid((int)(py/TILE_SIZE), (int)(px/TILE_SIZE));
}

TileMap World::getTileMap() const {
    return {blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE};
}

void World::spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }
void World::spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

void World::removeDeadEntities() {
    // Erase-remove idiom for animals and zombies
    animals.erase(std::remove_if(animals.begin(), animals.end(),
        [](const auto& a){ return !a->isAlive(); }), animals.end());
    zombies.erase(std::remove_if(zombies.begin(), zombies.end(),
        [](const auto& z){ return !z->isAlive(); }), zombies.end());
}

void World::update(float dt, Entity& player) {
    zombieHitThisFrame = false; // reset hit flag at start of each frame

    TileMap tm = getTileMap();

    // Tick all animals (wander AI + physics)
    for (auto& a : animals) a->tick(dt, tm);

    // Tick all zombies (chase + attack); record if any hit landed
    for (auto& z : zombies)
        if (z->chaseAndAttack(player, tm, dt)) zombieHitThisFrame = true;

    removeDeadEntities();

    // Dynamic spawn: spawn rate increases as player moves right
    float progress = player.getX() / (WORLD_COLS * TILE_SIZE);
    float interval = std::max(3.f, 8.f - progress * 5.f); // 8s near start → 3s at far right
    spawnTimer += dt;
    if (spawnTimer >= interval) {
        spawnTimer = 0.f;
        trySpawnZombie(player.getX());
    }
}

const std::vector<std::vector<std::string>>& World::getBlockMap() const { return blockMap; }
std::vector<std::shared_ptr<Animal>>& World::getAnimals()       { return animals; }
std::vector<std::shared_ptr<Zombie>>& World::getZombies()       { return zombies; }
const std::vector<std::shared_ptr<Animal>>& World::getAnimals() const { return animals; }
const std::vector<std::shared_ptr<Zombie>>& World::getZombies() const { return zombies; }
