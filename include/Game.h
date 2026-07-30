/**
 * @file Game.h
 * @brief Declares the Game class the central engine for the game.
 *
 * @author Group 46
 */
#pragma once
#include "Player.h"
#include "World.h"
#include "InputHandler.h"
#include "SaveManager.h"
#include "CraftingSystem.h"
#include "DayCycle.h"
#include "Tool.h"
#include <vector>
#include <string>

/**
 * @struct MineState
 * @brief Tracks the block currently being mined and per-hit progress.
 *
 * @author Group 46
 */
struct MineState {
    int   row    = -1;
    int   col    = -1;
    float timer  = 0.f;
    bool  active = false;
};

/**
 * @class Game
 * @brief Central gameplay controller that owns all subsystems and emits typed events.
 *
 * @author Group 46
 */
class Game {
public:
    /**
     * @enum Event
     * @brief Typed event codes emitted by Game and consumed by main.cpp each frame.
     */
    enum class Event {
        None,
        PlayerAte,
        PlayerAteRaw,
        PlayerWon,
        PlayerDiedStarvation,
        PlayerDiedVoid,
        PlayerDiedZombie,
        AnimalKilled,
        AnimalHit,
        ZombieHit,
        ZombieKilled,
        NoAttackTarget,
        BlockMineHit,
        BlockMined,
        BlockPlaced,
        BlockPlaceFailed,
        BlockOutOfReach,
        InventoryFull,
        Saved,
        Loaded,
        SleptWell,
        SleepNeedsBed,
        SleepWrongTime,
        NewDay,
        CraftOK,
        CraftFail,
        CraftNeedTable,
        CraftNeedCampfire,
        HungerWarning,
        SleepWarning,
    };

    /**
     * @struct EventData
     * @brief Pairs an Event code with an optional string detail.
     */
    struct EventData {
        Event       type   = Event::None;
        std::string detail;
    };

    static constexpr float REACH_DISTANCE = 120.f;

    Game();

    void update(float dt);

    void startMining(int row, int col, const std::string& blockName);
    void stopMining();
    void placeBlock(int row, int col);
    void requestAttack();
    void requestEat();
    void requestSleep();
    void requestCraft(const std::string& itemName);
    void saveGame();
    void loadGame();

    // ── Accessors ─────────────────────────────────────────────────────────────

    const std::vector<EventData>& getEvents()    const;
    Player&         getPlayer();
    World&          getWorld();
    InputHandler&   getInput();
    CraftingSystem& getCrafting();
    const DayCycle& getDayCycle()  const;
    DayCycle&       getDayCycle();
    float           getShakeTimer() const;
    const MineState& getMineState() const;
    float getMineTime() const;
    bool isNearCraftingTable() const;
    bool isNearCampfire() const;

    // ── Save/load state accessors ──────────────────────────────────────────────

    float getHungerTimer()    const;
    float getSleepTimer()     const;
    float getPlayerVY()       const;
    bool  getPlayerOnGround() const;

    void  setHungerTimer(float t);
    void  setSleepTimer(float t);
    void  setPlayerVY(float v);
    void  setPlayerOnGround(bool g);

private:
    Player         player;
    World          world;
    InputHandler   input;
    CraftingSystem craftingSystem;
    DayCycle       dayCycle;

    float vy = 0.f;
    float vx = 0.f;
    bool  onGround = true;
    float coyoteTimer = 0.f;

    static constexpr float COYOTE_TIME = 0.12f;
    static constexpr float GRAVITY     = 900.f;
    static constexpr float JUMP_VY     = -420.f;
    static constexpr float MOVE_SPEED  = 150.f;

    static constexpr float HUNGER_TICK        = 4.f;
    static constexpr float HUNGER_TICK_MOVING = 2.f;
    static constexpr float SLEEP_TICK         = 8.f;
    static constexpr float SLEEP_TICK_MOVING  = 4.f;
    float hungerTimer = 0.f;
    float sleepTimer  = 0.f;

    static constexpr float ATTACK_CD      = 0.5f;
    static constexpr float BASE_MINE_TIME = 1.5f;
    static constexpr float IFRAME_DURATION = 0.6f;

    float attackTimer = 0.f;
    float shakeTimer  = 0.f;
    float iframeTimer = 0.f;
    MineState mine;

    std::vector<EventData> pendingEvents;

    void emit(Event e, const std::string& detail = "");
    bool nearBlock(const std::string& blockType, float radius = 120.f) const;
    void tickPhysics(float dt);
    void tickSurvival(float dt);
    void tickMining(float dt);
    void tickCombat(float dt);
    void handleAttack();
    void handleEat();
    void handleBlockDrop(const std::string& blk, int row, int col);
};
