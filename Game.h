#pragma once
#include "Player.h"
#include "World.h"
#include "InputHandler.h"
#include "SaveManager.h"
#include "CraftingSystem.h"
#include "CollisionHelper.h"
#include <cmath>
#include <string>
#include <functional>

/**
 * @struct MineState
 * @brief Tracks the block currently being mined and per-hit progress.
 */
struct MineState {
    int   row    = -1;
    int   col    = -1;
    float timer  = 0.f;  ///< Time accumulated toward the next hit
    bool  active = false;
};

/**
 * @class Game
 * @brief Central game controller.
 *
 * Owns the Player, World, InputHandler, CraftingSystem, and all
 * gameplay state (physics, survival timers, mining). main.cpp
 * feeds raw input and calls update() each frame; it receives
 * outcome events via the eventCallback.
 */
class Game {
public:
    /// Outcome events emitted during update() for main.cpp to act on.
    enum class Event {
        None,
        PlayerAte,
        PlayerWon,
        PlayerDiedStarvation,
        PlayerDiedVoid,
        PlayerDiedZombie,
        AnimalKilled,
        ZombieHit,
        ZombieKilled,
        NoAttackTarget,
        BlockMineHit,
        BlockMined,
        BlockPlaced,
        BlockPlaceFailed,
        Saved,
        Loaded,
        SleptWell,
        CraftedCookedMeat,
        CraftFailCookedMeat,
        CraftedGoldenApple,
        CraftFailGoldenApple,
        HungerWarning,
        SleepWarning,
    };

    /// Extra data carried with some events (e.g. item name, HP value).
    struct EventData {
        Event       type   = Event::None;
        std::string detail;   ///< Item name, block name, HP string, etc.
    };

private:
    Player         player;
    World          world;
    InputHandler   input;
    CraftingSystem craftingSystem;

    // ── Physics state ─────────────────────────────────────────────────────────
    float vy       = 0.f;
    float vx       = 0.f;
    bool  onGround = true;

    // ── Survival timers ───────────────────────────────────────────────────────
    static constexpr float HUNGER_TICK        = 4.f;
    static constexpr float HUNGER_TICK_MOVING = 2.f;
    static constexpr float SLEEP_TICK         = 8.f;
    static constexpr float SLEEP_TICK_MOVING  = 4.f;
    float hungerTimer = 0.f;
    float sleepTimer  = 0.f;

    // ── Combat ────────────────────────────────────────────────────────────────
    static constexpr float ATTACK_CD = 0.5f;
    float attackTimer = 0.f;

    // ── Screen shake ──────────────────────────────────────────────────────────
    float shakeTimer = 0.f;

    // ── Mining ────────────────────────────────────────────────────────────────
    static constexpr float MINE_TIME = 1.5f; ///< Seconds per durability hit
    MineState mine;

    // ── Pending events (cleared each frame) ──────────────────────────────────
    std::vector<EventData> pendingEvents;

    void emit(Event e, const std::string& detail = "") {
        pendingEvents.push_back({ e, detail });
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    void tickPhysics(float dt) {
        float px = player.getPositionX();
        float py = player.getPositionY();

        vx = 0.f;
        if (input.moveLeft)  vx = -150.f;
        if (input.moveRight) vx =  150.f;
        if (player.getSleep().isEmpty()) vx *= 0.5f;

        px += vx * dt;
        px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

        if (input.jump && onGround) { vy = -420.f; onGround = false; }
        vy += 900.f * dt;
        py += vy * dt;

        TileMap tm = world.getTileMap();
        resolveTileCollision(px, py, vx, vy, onGround, tm);

        if (py > WORLD_ROWS * TILE_SIZE)
            emit(Event::PlayerDiedVoid);

        player.setPosition(px, py);
    }

    void tickSurvival(float dt) {
        bool isMoving = (input.moveLeft || input.moveRight);

        hungerTimer += dt;
        float hTick = isMoving ? HUNGER_TICK_MOVING : HUNGER_TICK;
        if (hungerTimer >= hTick) {
            hungerTimer = 0.f;
            player.getHunger().decrease(1);
            if (player.getHunger().isEmpty()) {
                player.setHealth(player.getHealth() - 1);
                emit(Event::HungerWarning,
                     std::to_string(player.getHealth()));
                if (player.getHealth() <= 0)
                    emit(Event::PlayerDiedStarvation);
            }
        }

        sleepTimer += dt;
        float sTick = isMoving ? SLEEP_TICK_MOVING : SLEEP_TICK;
        if (sleepTimer >= sTick) {
            sleepTimer = 0.f;
            player.getSleep().decrease(1);
            if (player.getSleep().isEmpty())
                emit(Event::SleepWarning);
        }
    }

    void tickMining(float dt) {
        if (!mine.active) return;

        mine.timer += dt;
        if (mine.timer >= MINE_TIME) {
            mine.timer = 0.f;
            std::string blk = world.getBlock(mine.row, mine.col);
            if (!blk.empty()) {
                bool destroyed = world.hitBlock(mine.row, mine.col);
                if (destroyed) {
                    if (blk == "Gold") {
                        player.getInventory().addItem(
                            std::make_shared<Item>("Gold", 1, ItemType::MISC));
                        emit(Event::BlockMined, "Gold");
                    } else {
                        player.getInventory().addItem(
                            std::make_shared<Block>(blk, 1));
                        emit(Event::BlockMined, blk);
                    }
                    mine = {};
                } else {
                    emit(Event::BlockMineHit, blk);
                }
            } else {
                mine = {};
            }
        }
    }

    void tickCombat(float dt) {
        if (attackTimer > 0.f) attackTimer -= dt;
        if (shakeTimer  > 0.f) shakeTimer  -= dt;

        // Zombie proximity — screen shake
        float px = player.getPositionX();
        float py = player.getPositionY();
        for (auto& z : world.getZombies())
            if (std::hypot(z->getX() - px, z->getY() - py) < 42.f) {
                shakeTimer = 0.3f;
                break;
            }

        // Zombie death check
        if (player.getHealth() <= 0)
            emit(Event::PlayerDiedZombie);
    }

    void handleAttack() {
        if (attackTimer > 0.f) return;
        attackTimer = ATTACK_CD;

        float px  = player.getPositionX();
        float py  = player.getPositionY();
        bool  hit = false;

        for (auto& a : world.getAnimals()) {
            if (!a->isAlive()) continue;
            if (std::hypot(a->getX() - px, a->getY() - py) < 90.f) {
                player.attack(*a);
                hit = true;
                if (!a->isAlive()) {
                    auto meat = a->dropMeat();
                    player.pickUp(meat);
                    emit(Event::AnimalKilled, meat->getName());
                } else {
                    emit(Event::ZombieHit,
                         std::to_string(a->getHp()));  // reusing field for animal HP
                }
            }
        }
        for (auto& z : world.getZombies()) {
            if (!z->isAlive()) continue;
            if (std::hypot(z->getX() - px, z->getY() - py) < 90.f) {
                player.attack(*z);
                hit = true;
                if (!z->isAlive())
                    emit(Event::ZombieKilled);
                else
                    emit(Event::ZombieHit, std::to_string(z->getHp()));
            }
        }
        if (!hit) emit(Event::NoAttackTarget);
        world.removeDeadEntities();
    }

    void handleEat() {
        auto sel  = player.getInventory().getSelectedItem();
        bool isGA = sel && sel->getName() == "GoldenApple";
        if (player.eat()) {
            emit(isGA ? Event::PlayerWon : Event::PlayerAte);
        }
    }

public:
    Game() = default;

    /**
     * @brief Starts mining at tile (row, col).
     *
     * Called by main.cpp on left-mouse-button press.
     */
    void startMining(int row, int col, const std::string& blockName) {
        mine = { row, col, 0.f, true };
        emit(Event::BlockMineHit, blockName); // "starting to mine" notification
    }

    /** @brief Cancels any in-progress mining (LMB released). */
    void stopMining() { mine.active = false; }

    /**
     * @brief Places the selected block item at tile (row, col).
     * Called by main.cpp on right-mouse-button press.
     */
    void placeBlock(int row, int col) {
        auto item = player.getInventory().getSelectedItem();
        if (item && item->getType() == ItemType::BLOCK) {
            if (world.placeBlock(row, col, item->getName())) {
                player.getInventory().removeItem(item->getName(), 1);
                emit(Event::BlockPlaced, item->getName());
            } else {
                emit(Event::BlockPlaceFailed);
            }
        } else {
            emit(Event::BlockPlaceFailed);
        }
    }

    /** @brief Request a player attack (F key). Enforces cooldown internally. */
    void requestAttack() { handleAttack(); }

    /** @brief Request eat of selected item (E key). */
    void requestEat() { handleEat(); }

    /** @brief Restore sleep fully (Z key). */
    void requestSleep() {
        player.getSleep().sleep();
        emit(Event::SleptWell);
    }

    /** @brief Craft CookedMeat (C key). */
    void requestCraftCookedMeat() {
        if (craftingSystem.craft("CookedMeat", player.getInventory()))
            emit(Event::CraftedCookedMeat);
        else
            emit(Event::CraftFailCookedMeat);
    }

    /** @brief Craft GoldenApple (G key). */
    void requestCraftGoldenApple() {
        if (craftingSystem.craft("GoldenApple", player.getInventory()))
            emit(Event::CraftedGoldenApple);
        else
            emit(Event::CraftFailGoldenApple);
    }

    /**
     * @brief Main update — called once per frame by main.cpp.
     *
     * Runs physics, survival, mining, combat, and world simulation.
     * Populates pendingEvents with outcomes for main.cpp to consume.
     *
     * @param dt Delta time in seconds
     */
    void update(float dt) {
        pendingEvents.clear();

        tickPhysics(dt);
        tickSurvival(dt);
        tickMining(dt);
        tickCombat(dt);

        world.update(dt, player);
    }

    /**
     * @brief Returns (and clears) all events emitted during the last update().
     */
    const std::vector<EventData>& getEvents() const { return pendingEvents; }

    void saveGame() { SaveManager::save(*this); emit(Event::Saved); }
    void loadGame() { SaveManager::load(*this); emit(Event::Loaded); }

    // ── Accessors ─────────────────────────────────────────────────────────────
    Player&         getPlayer()   { return player; }
    World&          getWorld()    { return world; }
    InputHandler&   getInput()    { return input; }
    CraftingSystem& getCrafting() { return craftingSystem; }

    float getShakeTimer()  const { return shakeTimer; }

    /// Mining progress accessors for the renderer overlay.
    const MineState& getMineState() const { return mine; }
};
