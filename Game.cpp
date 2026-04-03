/**
 * @file Game.cpp
 * @brief Implements the Game controller: physics, survival, mining, combat, and crafting.
 *
 * This class file declares the engine for the game, where many aspects of
 * the game is managed. Game loop, events, load/save states, physics, game
 * mechanics (like block drops), game actions are handled here.
 * @author Group 46
 */
#include "Game.h"
#include "CollisionHelper.h"
#include <cmath>
#include <cstdlib>

Game::Game() = default;

void Game::emit(Event e, const std::string& detail) {
    pendingEvents.push_back({e, detail});
}

bool Game::nearBlock(const std::string& blockType, float radius) const {
    float px = player.getPositionX();
    float py = player.getPositionY();

    // Narrow the search to tiles within a bounding box around the player
    int rMin = std::max(0,          (int)((py - radius) / TILE_SIZE));
    int rMax = std::min(WORLD_ROWS-1,(int)((py + radius) / TILE_SIZE));
    int cMin = std::max(0,          (int)((px - radius) / TILE_SIZE));
    int cMax = std::min(WORLD_COLS-1,(int)((px + radius) / TILE_SIZE));

    for (int r = rMin; r <= rMax; ++r)
        for (int c = cMin; c <= cMax; ++c)
            if (world.getBlock(r, c) == blockType) {
                // Precise circular distance check from player to block centre
                float bx = c * TILE_SIZE + TILE_SIZE / 2.f;
                float by = r * TILE_SIZE + TILE_SIZE / 2.f;
                if (std::hypot(bx - px, by - py) <= radius) return true;
            }
    return false;
}

float Game::getMineTime() const {
    // getMineSpeedMult() is const — no cast required
    return BASE_MINE_TIME * player.getMineSpeedMult();
}

bool Game::isNearCraftingTable() const { return nearBlock("CraftingTable", 120.f); }
bool Game::isNearCampfire()      const { return nearBlock("Campfire",      120.f); }

// ── Tick helpers ──────────────────────────────────────────────────────────────

void Game::tickPhysics(float dt) {
    float px = player.getPositionX();
    float py = player.getPositionY();

    // Horizontal movement — halved when exhausted
    vx = 0.f;
    if (input.moveLeft)  vx = -MOVE_SPEED;
    if (input.moveRight) vx =  MOVE_SPEED;
    if (player.getSleep().isEmpty()) vx *= 0.5f; // exhaustion penalty

    px += vx * dt;
    // Clamp to world horizontal bounds (ensure the player is in the bounds of the game)
    px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

    // Coyote time: keep jump available briefly after walking off a ledge
    if (onGround) coyoteTimer = COYOTE_TIME;
    else          coyoteTimer = std::max(0.f, coyoteTimer - dt);

    // Jump — edge-triggered; variable height via early release
    if (input.jumpPressed && coyoteTimer > 0.f) {
        vy = JUMP_VY;
        onGround  = false;
        coyoteTimer = 0.f;
    }
    // Variable jump height: releasing Space early cuts upward momentum
    if (!input.spaceHeld && vy < 0.f) vy += 600.f * dt;

    // Apply gravity and vertical movement
    vy += GRAVITY * dt;
    py += vy * dt;

    // Resolve against tile grid
    TileMap tm = world.getTileMap();
    resolveTileCollision(px, py, vx, vy, onGround, tm);

    // Void death — player fell below the world
    if (py > WORLD_ROWS * TILE_SIZE) emit(Event::PlayerDiedVoid);

    player.setPosition(px, py);
}

void Game::tickSurvival(float dt) {
    bool moving = input.moveLeft || input.moveRight;

    // Hunger drain — faster while moving
    hungerTimer += dt;
    if (hungerTimer >= (moving ? HUNGER_TICK_MOVING : HUNGER_TICK)) {
        hungerTimer = 0.f;
        player.getHunger().decrease(1);
        if (player.getHunger().isEmpty()) {
            // Starving — lose 1 HP per hunger tick
            player.setHealth(player.getHealth() - 1);
            emit(Event::HungerWarning, std::to_string(player.getHealth()));
            if (player.getHealth() <= 0) emit(Event::PlayerDiedStarvation);
        }
    }

    // Sleep drain — twice as fast at Night; faster while moving
    float sTick = dayCycle.isNight()
        ? (moving ? SLEEP_TICK_MOVING * 0.5f : SLEEP_TICK * 0.5f)
        : (moving ? SLEEP_TICK_MOVING         : SLEEP_TICK);
    sleepTimer += dt;
    if (sleepTimer >= sTick) {
        sleepTimer = 0.f;
        player.getSleep().decrease(1);
        if (player.getSleep().isEmpty()) emit(Event::SleepWarning);
    }
}

void Game::tickMining(float dt) {
    if (!mine.active) return;

    // Cancel mining if the player has walked out of reach
    float bx = mine.col * TILE_SIZE + TILE_SIZE / 2.f;
    float by = mine.row * TILE_SIZE + TILE_SIZE / 2.f;
    float px = player.getPositionX() + TILE_SIZE / 2.f;
    float py = player.getPositionY() + TILE_SIZE / 2.f;
    if (std::hypot(bx - px, by - py) > REACH_DISTANCE) { mine = {}; return; }

    float mineTime = BASE_MINE_TIME * player.getMineSpeedMult();
    mine.timer += dt;
    if (mine.timer >= mineTime) {
        mine.timer = 0.f;
        std::string blk = world.getBlock(mine.row, mine.col);
        if (!blk.empty()) {
            if (world.hitBlock(mine.row, mine.col)) {
                // Block destroyed — handle drop and clear mine state
                handleBlockDrop(blk, mine.row, mine.col);
                mine = {};
            } else {
                // Block survived this hit — report progress
                emit(Event::BlockMineHit, blk);
            }
        } else {
            mine = {}; // block was already gone (placed/removed externally)
        }
    }
}

void Game::tickCombat(float dt) {
    if (attackTimer > 0.f) attackTimer -= dt;
    if (shakeTimer  > 0.f) shakeTimer  -= dt;

    // If a zombie landed a hit this frame, trigger shake and start iframe window
    if (world.wasHitByZombie()) {
        shakeTimer  = 0.25f;          // camera shake duration
        iframeTimer = IFRAME_DURATION; // block further damage for 0.6 s
    }

    if (player.getHealth() <= 0) emit(Event::PlayerDiedZombie);
}

void Game::handleBlockDrop(const std::string& blk, int row, int col) {
    auto& inv = player.getInventory();

    // If the primary drop can't fit, restore the block and warn the player
    auto tryAdd = [&](std::shared_ptr<Item> item) -> bool {
        if (inv.addItem(item)) return true;
        world.placeBlock(row, col, blk); // put the block back
        emit(Event::InventoryFull, blk);
        return false;
    };

    if (blk == "Gold") {
        if (!tryAdd(std::make_shared<Item>("Gold", 1, ItemType::MISC))) return;
        emit(Event::BlockMined, "Gold");

    } else if (blk == "Wood") {
        if (!tryAdd(std::make_shared<Block>("Wood", 1))) return;
        emit(Event::BlockMined, "Wood");
        // 30% chance to also drop an Apple from the trunk
        if (rand() % 100 < 30)
            if (inv.addItem(std::make_shared<Food>(FoodItems::Apple(1))))
                emit(Event::BlockMined, "Wood+Apple");

    } else if (blk == "Leaves") {
        // Leaves have a 20% chance to drop an Apple; no hard block drop
        if (rand() % 100 < 20) {
            if (inv.addItem(std::make_shared<Food>(FoodItems::Apple(1))))
                emit(Event::BlockMined, "Leaves+Apple");
            else
                emit(Event::BlockMined, "Leaves");
        } else {
            emit(Event::BlockMined, "Leaves");
        }

    } else if (blk == "CraftingTable") {
        if (!tryAdd(std::make_shared<Block>("CraftingTable", 1))) return;
        emit(Event::BlockMined, "CraftingTable");

    } else if (blk == "Campfire") {
        if (!tryAdd(std::make_shared<Tool>(ToolItems::Campfire(1)))) return;
        emit(Event::BlockMined, "Campfire");

    } else if (blk == "Bed") {
        if (!tryAdd(std::make_shared<Tool>(ToolItems::Bed(1)))) return;
        emit(Event::BlockMined, "Bed");

    } else {
        if (!tryAdd(std::make_shared<Block>(blk, 1))) return;
        emit(Event::BlockMined, blk);
    }
}

void Game::handleAttack() {
    if (attackTimer > 0.f) return; // still on cooldown
    attackTimer = ATTACK_CD;

    float px  = player.getPositionX();
    float py  = player.getPositionY();
    bool  hit = false;
    int   dmg = player.getTotalAttackDamage(); // base + sword bonus

    // Check animals first
    for (auto& a : world.getAnimals()) {
        if (!a->isAlive()) continue;
        if (std::hypot(a->getX() - px, a->getY() - py) < 90.f) {
            player.attack(*a, dmg);
            hit = true;
            if (!a->isAlive()) {
                auto meat = a->dropMeat();
                if (player.pickUp(meat)) emit(Event::AnimalKilled, meat->getName());
                else                     emit(Event::InventoryFull, meat->getName());
            } else {
                emit(Event::AnimalHit, std::to_string(a->getHp()));
            }
        }
    }

    // Then zombies
    for (auto& z : world.getZombies()) {
        if (!z->isAlive()) continue;
        if (std::hypot(z->getX() - px, z->getY() - py) < 90.f) {
            player.attack(*z, dmg);
            hit = true;
            if (!z->isAlive()) emit(Event::ZombieKilled);
            else               emit(Event::ZombieHit, std::to_string(z->getHp()));
        }
    }

    if (!hit) emit(Event::NoAttackTarget);
    world.removeDeadEntities();
}

void Game::handleEat() {
    auto sel = player.getInventory().getSelectedItem();

    if (!sel) return;                             // nothing selected
    if (sel->getType() != ItemType::FOOD) return; // not food
    if (sel->getName() == "RawMeat") { emit(Event::PlayerAteRaw); return; }

    // Capture the GoldenApple flag BEFORE eat() removes the item from the slot
    bool isGA = (sel->getName() == "GoldenApple");

    if (player.eat()) {
        if (isGA)
            emit(Event::PlayerWon);  // win fires immediately and unambiguously
        else
            emit(Event::PlayerAte);
    }
}

// ── Public API ────────────────────────────────────────────────────────────────

void Game::update(float dt) {
    //pendingEvents.clear();

    // Track day changes for NewDay event
    int prevDay = dayCycle.getDayCount();
    dayCycle.update(dt);
    if (dayCycle.getDayCount() != prevDay)
        emit(Event::NewDay, std::to_string(dayCycle.getDayCount()));

    tickPhysics(dt);
    tickSurvival(dt);
    tickMining(dt);

    // Tick down iframe window and update the player's invincible flag BEFORE
    // world.update() so zombies calling takeDamage() this frame see it
    if (iframeTimer > 0.f) {
        iframeTimer -= dt;
        player.setInvincible(iframeTimer > 0.f);
    } else {
        player.setInvincible(false);
    }

    world.update(dt, player);  // zombie AI runs here; takeDamage() called here
    tickCombat(dt);             // records hit, starts new iframe window if hit landed

    // Deduplicate death events — keep only the first one if multiple fired
    bool deathSeen = false;
    for (auto it = pendingEvents.begin(); it != pendingEvents.end(); ) {
        bool isDeath = (it->type == Event::PlayerDiedStarvation ||
                        it->type == Event::PlayerDiedVoid       ||
                        it->type == Event::PlayerDiedZombie);
        if (isDeath) {
            if (deathSeen) { it = pendingEvents.erase(it); continue; }
            deathSeen = true;
        }
        ++it;
    }
}

void Game::startMining(int row, int col, const std::string& blockName) {
    // Distance check before starting
    float bx = col * TILE_SIZE + TILE_SIZE / 2.f;
    float by = row * TILE_SIZE + TILE_SIZE / 2.f;
    float px = player.getPositionX() + TILE_SIZE / 2.f;
    float py = player.getPositionY() + TILE_SIZE / 2.f;
    if (std::hypot(bx - px, by - py) > REACH_DISTANCE) {
        emit(Event::BlockOutOfReach);
        return;
    }
    mine = {row, col, 0.f, true};
    emit(Event::BlockMineHit, blockName);
}

void Game::stopMining() { mine.active = false; }

void Game::placeBlock(int row, int col) {
    // Distance check
    float bx = col * TILE_SIZE + TILE_SIZE / 2.f;
    float by = row * TILE_SIZE + TILE_SIZE / 2.f;
    float px = player.getPositionX() + TILE_SIZE / 2.f;
    float py = player.getPositionY() + TILE_SIZE / 2.f;
    if (std::hypot(bx - px, by - py) > REACH_DISTANCE) {
        emit(Event::BlockOutOfReach);
        return;
    }

    auto item = player.getInventory().getSelectedItem();
    if (!item) { emit(Event::BlockPlaceFailed); return; }

    // Determine what tile name to place based on item type
    std::string tileName;
    if (item->getType() == ItemType::BLOCK) {
        tileName = item->getName();
    } else if (item->getType() == ItemType::TOOL) {
        Tool* t = dynamic_cast<Tool*>(item.get());
        if (t && (t->getToolType() == ToolType::Bed ||
                  t->getToolType() == ToolType::Campfire))
            tileName = item->getName();
        else if (item->getName() == "CraftingTable")
            tileName = "CraftingTable";
    }
    if (tileName.empty()) { emit(Event::BlockPlaceFailed); return; }

    if (world.placeBlock(row, col, tileName)) {
        player.getInventory().removeItem(item->getName(), 1);
        emit(Event::BlockPlaced, tileName);
    } else {
        emit(Event::BlockPlaceFailed);
    }
}

void Game::requestAttack() { handleAttack(); }
void Game::requestEat()    { handleEat(); }

void Game::requestSleep() {
    if (!nearBlock("Bed", 120.f)) { emit(Event::SleepNeedsBed);  return; }
    if (!dayCycle.isNight())      { emit(Event::SleepWrongTime); return; }

    player.getSleep().sleep();      // fully restore sleep bar
    dayCycle.skipToMorning();       // advance time to next morning
    emit(Event::SleptWell, std::to_string(dayCycle.getDayCount()));
}

void Game::requestCraft(const std::string& itemName) {
    bool hasTable    = isNearCraftingTable();
    bool hasCampfire = isNearCampfire();

    // Station checks before delegating to CraftingSystem
    if (itemName == "CookedMeat" && !hasCampfire) { emit(Event::CraftNeedCampfire); return; }
    if (craftingSystem.requiresTable(itemName) && !hasTable) { emit(Event::CraftNeedTable); return; }

    if (craftingSystem.craft(itemName, player.getInventory(), hasTable, hasCampfire))
        emit(Event::CraftOK,   itemName);
    else
        emit(Event::CraftFail, itemName);
}

void Game::saveGame() { SaveManager::save(*this); emit(Event::Saved); }
void Game::loadGame() { SaveManager::load(*this); emit(Event::Loaded); }
