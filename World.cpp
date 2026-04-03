/**
 * @file World.cpp
 * @brief This class places all the blocks and spawns monsters into the world.
 *
 * This class allows for generating the terrain, as well as placing structures 
 * like trees. It also handles removing and adding blocks to the game world. 
 * Entities can also be spawned and removed as needed.
 * @author Group 46
 */


#include "World.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

World::World() {
    blockMap.assign(WORLD_ROWS, std::vector<std::string>(WORLD_COLS, ""));
    buildTerrain();
    spawnDefaultEntities();
}

int World::blockMaxDurability(const std::string& type) const {
    if (type == "Grass")  return 2;
    if (type == "Dirt")   return 2;
    if (type == "Sand")   return 2;
    if (type == "Wood")   return 5;
    if (type == "Stone")  return 8;
    if (type == "Gold")   return 8;
    return 3;
}

bool World::hitBlock(int row, int col) {
    if (row < 0 || row >= WORLD_ROWS || col < 0 || col >= WORLD_COLS) return false;
    if (blockMap[row][col].empty()) return false;
    auto key = std::make_pair(row, col);
    if (durabilityMap.find(key) == durabilityMap.end())
        durabilityMap[key] = blockMaxDurability(blockMap[row][col]);
    if (--durabilityMap[key] <= 0) {
        blockMap[row][col] = "";
        durabilityMap.erase(key);
        return true;
    }
    return false;
}

int World::getBlockDurability(int row, int col) const {
    auto key = std::make_pair(row, col);
    auto it  = durabilityMap.find(key);
    if (it != durabilityMap.end()) return it->second;
    const std::string& type = getBlock(row, col);
    if (type.empty()) return 0;
    return blockMaxDurability(type);
}

int World::surfaceRowAt(int col) const {
    float fx = (float)col / WORLD_COLS;
    float h  =  2.0f * std::sin(fx * 6.28f * 1.5f)
              + 1.0f * std::sin(fx * 6.28f * 3.7f)
              + 0.5f * std::sin(fx * 6.28f * 8.1f);
    int r = GROUND_ROW + (int)std::round(h * 0.7f);
    return std::max(GROUND_ROW - 2, std::min(GROUND_ROW + 2, r));
}

void World::placeTree(int col, int height, const std::vector<int>& groundY) {
    if (col < 0 || col >= WORLD_COLS) return;
    int base = groundY[col] - 1;
    for (int r = base; r > base - height; --r)
        if (r >= 0 && blockMap[r][col].empty())
            blockMap[r][col] = "Wood";
    int top = base - height;
    if (top >= 0 && col < WORLD_COLS)
        blockMap[top][col] = "Leaves";
    if (top + 1 >= 0)
        for (int dc = -1; dc <= 1; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS)
                blockMap[top+1][col+dc] = "Leaves";
    if (top + 2 >= 0)
        for (int dc = -2; dc <= 2; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS)
                blockMap[top+2][col+dc] = "Leaves";
    if (top + 3 >= 0)
        for (int dc = -2; dc <= 2; ++dc)
            if (col+dc >= 0 && col+dc < WORLD_COLS && blockMap[top+3][col+dc].empty())
                blockMap[top+3][col+dc] = "Leaves";
}

void World::buildTerrain() {
    std::vector<int> groundY(WORLD_COLS);
    for (int c = 0; c < WORLD_COLS; ++c) {
        float fx = (float)c / WORLD_COLS;
        float h  =  2.0f * std::sin(fx * 6.28f * 1.5f)
                  + 1.0f * std::sin(fx * 6.28f * 3.7f)
                  + 0.5f * std::sin(fx * 6.28f * 8.1f);
        groundY[c] = GROUND_ROW + (int)std::round(h * 0.7f);
        groundY[c] = std::max(GROUND_ROW-2, std::min(GROUND_ROW+2, groundY[c]));
    }

    for (int c = 0; c < WORLD_COLS; ++c) {
        int surf = groundY[c];
        for (int r = 0; r < WORLD_ROWS; ++r) {
            if      (r < surf)        blockMap[r][c] = "";
            else if (r == surf)       blockMap[r][c] = "Grass";
            else if (r <= surf + 2)   blockMap[r][c] = "Dirt";
            else                      blockMap[r][c] = "Stone";
        }
    }

    auto drillCave = [&](int startR, int startC, int length) {
        int r = startR, c = startC, dr = 0;
        for (int i = 0; i < length; ++i) {
            for (int rr = r; rr <= r+1; ++rr)
                if (rr >= 0 && rr < WORLD_ROWS && c >= 0 && c < WORLD_COLS)
                    blockMap[rr][c] = "";
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

    drillCave(GROUND_ROW+4, 28, 22);
    drillCave(GROUND_ROW+5, 55, 28);
    drillCave(GROUND_ROW+7, 85, 25);
    drillCave(GROUND_ROW+8,100, 15);

    auto placeGold = [&](int r, int c) {
        if (r >= 0 && r < WORLD_ROWS && c >= 0 && c < WORLD_COLS
            && blockMap[r][c] == "Stone")
            blockMap[r][c] = "Gold";
    };
    placeGold(GROUND_ROW+3,15); placeGold(GROUND_ROW+3,16);
    placeGold(GROUND_ROW+4,22);
    placeGold(GROUND_ROW+5,40); placeGold(GROUND_ROW+5,41);
    placeGold(GROUND_ROW+6,40);
    placeGold(GROUND_ROW+5,60); placeGold(GROUND_ROW+6,61);
    placeGold(GROUND_ROW+7,78); placeGold(GROUND_ROW+7,79);
    placeGold(GROUND_ROW+8,79);
    placeGold(GROUND_ROW+7,98); placeGold(GROUND_ROW+8,99);
    placeGold(GROUND_ROW+8,100);

    static const int treeCols[]    = {4,14,24,35,46,57,67,77,87,97,107,115};
    static const int treeHeights[] = {4, 3, 5, 4, 3, 5, 4, 3, 5, 4,  3,  4};
    for (int i = 0; i < 12; ++i)
        placeTree(treeCols[i], treeHeights[i], groundY);
}

void World::spawnDefaultEntities() {
    auto groundPx = [&](int col) -> float {
        return (float)(surfaceRowAt(col) * TILE_SIZE) - TILE_SIZE;
    };
    animals.push_back(std::make_shared<Animal>(  7*TILE_SIZE, groundPx( 7)));
    animals.push_back(std::make_shared<Animal>( 20*TILE_SIZE, groundPx(20)));
    animals.push_back(std::make_shared<Animal>( 50*TILE_SIZE, groundPx(50)));
    animals.push_back(std::make_shared<Animal>( 80*TILE_SIZE, groundPx(80)));
    animals.push_back(std::make_shared<Animal>(105*TILE_SIZE, groundPx(105)));

    zombies.push_back(std::make_shared<Zombie>( 22*TILE_SIZE, groundPx(22)));
    zombies.push_back(std::make_shared<Zombie>( 45*TILE_SIZE, groundPx(45)));
    zombies.push_back(std::make_shared<Zombie>( 70*TILE_SIZE, groundPx(70)));
    zombies.push_back(std::make_shared<Zombie>( 90*TILE_SIZE, groundPx(90)));
}

void World::trySpawnZombie(float playerX) {
    if ((int)zombies.size() >= maxZombies) return;
    int col = (int)(playerX / TILE_SIZE) + 10 + rand() % 5;
    if (col >= WORLD_COLS - 2) return;
    float spawnY = (float)(surfaceRowAt(col) * TILE_SIZE) - TILE_SIZE;
    zombies.push_back(std::make_shared<Zombie>((float)col * TILE_SIZE, spawnY));
}

std::string World::getBlock(int row, int col) const {
    if (row<0||row>=WORLD_ROWS||col<0||col>=WORLD_COLS) return "";
    if (row>=(int)blockMap.size()||col>=(int)blockMap[row].size()) return "";
    return blockMap[row][col];
}

void World::removeBlock(int row, int col) {
    if (row>=0&&row<WORLD_ROWS&&col>=0&&col<WORLD_COLS)
        blockMap[row][col] = "";
}

bool World::placeBlock(int row, int col, const std::string& type) {
    if (row<0||row>=WORLD_ROWS||col<0||col>=WORLD_COLS) return false;
    if (!blockMap[row][col].empty()) return false;
    blockMap[row][col] = type;
    return true;
}

bool World::isSolid(int row, int col) const { return !getBlock(row,col).empty(); }
bool World::isSolidAt(float px, float py) const {
    return isSolid((int)(py/TILE_SIZE),(int)(px/TILE_SIZE));
}

TileMap World::getTileMap() const {
    return {blockMap, WORLD_ROWS, WORLD_COLS, TILE_SIZE};
}

void World::spawnAnimal(std::shared_ptr<Animal> a) { animals.push_back(a); }
void World::spawnZombie(std::shared_ptr<Zombie> z) { zombies.push_back(z); }

void World::removeDeadEntities() {
    animals.erase(std::remove_if(animals.begin(),animals.end(),
        [](const auto& a){return !a->isAlive();}),animals.end());
    zombies.erase(std::remove_if(zombies.begin(),zombies.end(),
        [](const auto& z){return !z->isAlive();}),zombies.end());
}

void World::separateEntities(Entity& player) {
    static constexpr float ENTITY_SEPARATION_DIST = 36.f;

    auto pushApart = [&](Entity& a, Entity& b) {
        float dx = b.getX() - a.getX();
        float dist = std::abs(dx);
        if (dist < ENTITY_SEPARATION_DIST && dist > 0.01f) {
            float overlap = ENTITY_SEPARATION_DIST - dist;
            float push = overlap * 0.5f;
            float dir = (dx > 0.f) ? 1.f : -1.f;

            float newAx = std::max(0.f,
                std::min((WORLD_COLS - 1) * (float)TILE_SIZE,
                    a.getX() - dir * push));
            float newBx = std::max(0.f,
                std::min((WORLD_COLS - 1) * (float)TILE_SIZE,
                    b.getX() + dir * push));
            a.setX(newAx);
            b.setX(newBx);
        }
        };

    // animal vs animal
    for (int i = 0; i < (int)animals.size(); ++i)
        for (int j = i + 1; j < (int)animals.size(); ++j)
            if (animals[i]->isAlive() && animals[j]->isAlive())
                pushApart(*animals[i], *animals[j]);

    // zombie vs zombie
    for (int i = 0; i < (int)zombies.size(); ++i)
        for (int j = i + 1; j < (int)zombies.size(); ++j)
            if (zombies[i]->isAlive() && zombies[j]->isAlive())
                pushApart(*zombies[i], *zombies[j]);

    // animal vs zombie
    for (auto& a : animals)
        for (auto& z : zombies)
            if (a->isAlive() && z->isAlive())
                pushApart(*a, *z);

    auto resolvePlayerVsEntity = [&](Entity& e) {
        float px = player.getX();
        float py = player.getY();
        float ex = e.getX();
        float ey = e.getY();

        // AABB overlap
        bool overlapX = px < ex + TILE_SIZE && px + TILE_SIZE > ex;
        bool overlapY = py < ey + TILE_SIZE && py + TILE_SIZE > ey;
        if (!overlapX || !overlapY) return;

        float playerBottom = py + TILE_SIZE;
        float playerTop = py;
        float playerRight = px + TILE_SIZE;
        float playerLeft = px;

        float entityTop = ey;
        float entityBottom = ey + TILE_SIZE;
        float entityLeft = ex;
        float entityRight = ex + TILE_SIZE;

        // overlap amounts
        float overlapFromTop = playerBottom - entityTop;      // player landing on entity
        float overlapFromLeft = playerRight - entityLeft;     // player hits entity from left
        float overlapFromRight = entityRight - playerLeft;    // player hits entity from right

        // Top landing check:
        // only if player is near top of entity, not deep inside it
        if (playerBottom > entityTop &&
            playerBottom < entityTop + 16.f &&
            playerTop < entityTop)
        {
            player.setY(entityTop - TILE_SIZE);

            // If your Entity/Player class has these, add them:
            // player.setVelocityY(0.f);
            // player.setOnGround(true);

            return;
        }

        // Otherwise side block only
        if (overlapFromLeft < overlapFromRight) {
            player.setX(entityLeft - TILE_SIZE);
        }
        else {
            player.setX(entityRight);
        }
        };

    for (auto& a : animals)
        if (a->isAlive())
            resolvePlayerVsEntity(*a);

    for (auto& z : zombies)
        if (z->isAlive())
            resolvePlayerVsEntity(*z);
}

void World::update(float dt, Entity& player) {
    zombieHitThisFrame = false;
    TileMap tm = getTileMap();
    for (auto& a : animals) a->tick(dt, tm);
    for (auto& z : zombies)
        if (z->chaseAndAttack(player, tm, dt)) zombieHitThisFrame = true;
    removeDeadEntities();

    // Push overlapping entities apart after all AI movement is resolved
    separateEntities(player);

    float progress = player.getX() / (WORLD_COLS * TILE_SIZE);
    float interval = std::max(5.f, 12.f - progress * 5.f);
    spawnTimer += dt;
    if (spawnTimer >= interval) { spawnTimer = 0.f; trySpawnZombie(player.getX()); }
}

const std::vector<std::vector<std::string>>& World::getBlockMap() const { return blockMap; }
std::vector<std::shared_ptr<Animal>>& World::getAnimals() { return animals; }
std::vector<std::shared_ptr<Zombie>>& World::getZombies() { return zombies; }
const std::vector<std::shared_ptr<Animal>>& World::getAnimals() const { return animals; }
const std::vector<std::shared_ptr<Zombie>>& World::getZombies() const { return zombies; }

bool World::wasHitByZombie() const { return zombieHitThisFrame; }
int  World::getTileSize()    const { return TILE_SIZE; }
int  World::getGroundRow()   const { return GROUND_ROW; }
int  World::getCols()        const { return WORLD_COLS; }
int  World::getRows()        const { return WORLD_ROWS; }

void World::setBlockMap(const std::vector<std::vector<std::string>>& bm) {
    blockMap = bm;
    durabilityMap.clear();
}

void World::clearEntitiesForLoad() {
    animals.clear();
    zombies.clear();
}

void World::addAnimalForLoad(float x, float y, int hp,
                              const std::string& meatType, int meatAmount) {
    auto a = std::make_shared<Animal>(x, y, meatType, meatAmount, 20);
    a->setHp(hp);
    animals.push_back(a);
}

void World::addZombieForLoad(float x, float y, int hp) {
    auto z = std::make_shared<Zombie>(x, y);
    z->setHp(hp);
    zombies.push_back(z);
}
