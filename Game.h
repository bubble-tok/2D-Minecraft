/**
 * @file Game.h
 * @brief Declares the Game class the central engine for the game.
 *
 * This header file declares the engine for the game, where many aspects of
 * the game is managed. Game loop, events, load/save states, physics, game
 * mechanics, game actions are handled here.
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
 * When active is true, tickMining() accumulates time in timer until it
 * reaches the mine time threshold, then calls World::hitBlock().
 *
 * @author Group 46
 */
struct MineState {
    int   row    = -1;  ///< Tile row of the block being mined.
    int   col    = -1;  ///< Tile column of the block being mined.
    float timer  = 0.f; ///< Accumulated time toward the next hit.
    bool  active = false; ///< True while the player holds left click on a black (currently breaking)
};

/**
 * @class Game
 * @brief Central gameplay controller that owns all subsystems and emits typed events.
 *
 * Game owns Player, World, InputHandler, CraftingSystem, and DayCycle.
 * Each frame, update() ticks physics, survival, mining, entity simulation,
 * and combat in a defined order. Side-effects (player ate, block mined, died,
 * etc.) are communicated to main.cpp through a typed Event queue that is
 * cleared at the start of each update().
 *
 * The invincibility frame system prevents the player from taking damage more
 * than once every IFRAME_DURATION seconds regardless of how many zombies
 * are attacking simultaneously.
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
        PlayerAte,            ///< Player successfully ate a food item.
        PlayerAteRaw,         ///< Player tried to eat RawMeat (blocked).
        PlayerWon,            ///< Player ate a GoldenApple — win condition met.
        PlayerDiedStarvation, ///< Player HP reached 0 from hunger damage.
        PlayerDiedVoid,       ///< Player fell below the world boundary.
        PlayerDiedZombie,     ///< Player HP reached 0 from zombie damage.
        AnimalKilled,         ///< Player killed an animal (detail = meat name).
        AnimalHit,            ///< Player hit an animal (detail = remaining HP).
        ZombieHit,            ///< Player hit a zombie (detail = remaining HP).
        ZombieKilled,         ///< Player killed a zombie.
        NoAttackTarget,       ///< F pressed but no enemy in range.
        BlockMineHit,         ///< One mining hit registered (detail = block type).
        BlockMined,           ///< Block fully destroyed (detail = drop description).
        BlockPlaced,          ///< Block placed successfully (detail = block type).
        BlockPlaceFailed,     ///< Block could not be placed at target position.
        BlockOutOfReach,      ///< Block is too far away to mine or place.
        InventoryFull,        ///< Item could not be picked up — inventory is full.
        Saved,                ///< Game saved to disk.
        Loaded,               ///< Game loaded from disk.
        SleptWell,            ///< Player slept through the night (detail = new day count).
        SleepNeedsBed,        ///< Z pressed but no Bed in range.
        SleepWrongTime,       ///< Z pressed but it is not Night.
        NewDay,               ///< Day counter incremented (detail = new day count).
        CraftOK,              ///< Item crafted successfully (detail = item name).
        CraftFail,            ///< Crafting failed — missing ingredients.
        CraftNeedTable,       ///< Recipe requires a CraftingTable nearby.
        CraftNeedCampfire,    ///< Recipe requires a Campfire nearby.
        HungerWarning,        ///< Player is starving — HP draining (detail = current HP).
        SleepWarning,         ///< Player is exhausted — movement slowed.
    };

    /**
     * @struct EventData
     * @brief Pairs an Event code with an optional string detail.
     */
    struct EventData {
        Event       type   = Event::None; ///< Event type code.
        std::string detail;               ///< Optional context string (HP value, item name, etc.).
    };

    /// Maximum pixel distance for block mining and placing.
    static constexpr float REACH_DISTANCE = 120.f;

    /**
     * @brief Constructs the Game, initialising all subsystems to their defaults.
     */
    Game();

    /**
     * @brief Advances all game systems by one frame.
     *
     * Order of operations:
     * 1. Clear pending events from last frame.
     * 2. Tick DayCycle; emit NewDay if day count changed.
     * 3. tickPhysics — player movement and tile collision.
     * 4. tickSurvival — hunger and sleep drain.
     * 5. tickMining — per-hit progress toward block destruction.
     * 6. Set player invincibility flag based on iframeTimer.
     * 7. world.update() — animal/zombie AI; zombies call takeDamage() here.
     * 8. tickCombat — start iframe window if zombie hit landed.
     * 9. Deduplicate death events (keep only first).
     *
     * @param dt Delta time in seconds from the main loop clock.
     */
    void update(float dt);

    /**
     * @brief Starts mining the block at (row, col) if it is within reach.
     *
     * Sets MineState.active = true and emits BlockMineHit. Does nothing and
     * emits BlockOutOfReach if the distance exceeds REACH_DISTANCE.
     *
     * @param row       Tile row of the target block.
     * @param col       Tile column of the target block.
     * @param blockName Type string of the block (for the event detail).
     */
    void startMining(int row, int col, const std::string& blockName);

    /**
     * @brief Cancels active mining (called on left mouse button release).
     */
    void stopMining();

    /**
     * @brief Places the selected inventory item at (row, col) if within reach.
     *
     * Only Block items and placeable Tools (Bed, Campfire) can be placed.
     * Emits BlockPlaced on success, BlockOutOfReach or BlockPlaceFailed otherwise.
     *
     * @param row Tile row of the target position.
     * @param col Tile column of the target position.
     */
    void placeBlock(int row, int col);

    /// Triggers a melee attack — routes to handleAttack().
    void requestAttack();

    /// Eats the selected food item — routes to handleEat().
    void requestEat();

    /**
     * @brief Attempts to sleep. Requires a Bed nearby and Night phase.
     *
     * On success, fully restores the sleep bar and calls DayCycle::skipToMorning().
     */
    void requestSleep();

    /**
     * @brief Attempts to craft the named item from the player's inventory.
     *
     * Checks station proximity before delegating to CraftingSystem::craft().
     * Emits CraftOK or CraftFail (or CraftNeedTable / CraftNeedCampfire).
     *
     * @param itemName Recipe output name to craft.
     */
    void requestCraft(const std::string& itemName);

    /// Saves the game to disk. Emits Event::Saved.
    void saveGame();

    /// Loads the game from disk. Emits Event::Loaded.
    void loadGame();

    // ── Accessors ─────────────────────────────────────────────────────────────

    /** @brief Returns the event queue produced this frame. */
    const std::vector<EventData>& getEvents() const { return pendingEvents; }

    /** @brief Returns a mutable reference to the Player. */
    Player&         getPlayer()           { return player; }

    /** @brief Returns a mutable reference to the World. */
    World&          getWorld()            { return world; }

    /** @brief Returns a mutable reference to the InputHandler. */
    InputHandler&   getInput()            { return input; }

    /** @brief Returns a mutable reference to the CraftingSystem. */
    CraftingSystem& getCrafting()         { return craftingSystem; }

    /** @brief Returns a const reference to the DayCycle. */
    const DayCycle& getDayCycle()  const  { return dayCycle; }

    /** @brief Returns the remaining screen shake duration in seconds. */
    float           getShakeTimer() const { return shakeTimer; }

    /** @brief Returns the current mining state. */
    const MineState& getMineState() const { return mine; }

    /**
     * @brief Returns the time required for one mining hit in seconds.
     *
     * Calculated as BASE_MINE_TIME × player.getMineSpeedMult().
     *
     * @return Mining time per hit in seconds.
     */
    float getMineTime() const;

    /**
     * @brief Returns true if a CraftingTable block is within 120px of the player.
     * @return True if a CraftingTable is nearby.
     */
    bool isNearCraftingTable() const;

    /**
     * @brief Returns true if a Campfire block is within 120px of the player.
     * @return True if a Campfire is nearby.
     */
    bool isNearCampfire() const;

private:
    Player         player;         ///< The user-controlled entity.
    World          world;          ///< Tile grid and entity simulation.
    InputHandler   input;          ///< Keyboard state for this frame.
    CraftingSystem craftingSystem; ///< Recipe registry and craft executor.
    DayCycle       dayCycle;       ///< Time-of-day tracking.

    // ── Physics ───────────────────────────────────────────────────────────────
    float vy = 0.f;           ///< Player vertical velocity (pixels/s).
    float vx = 0.f;           ///< Player horizontal velocity (pixels/s).
    bool  onGround = true;    ///< True while player stands on a tile.
    float coyoteTimer = 0.f;  ///< Grace window after walking off an edge.

    static constexpr float COYOTE_TIME = 0.12f; ///< Coyote time window in seconds.
    static constexpr float GRAVITY     = 900.f;  ///< Downward acceleration (px/s²).
    static constexpr float JUMP_VY     = -420.f; ///< Initial vertical jump velocity.
    static constexpr float MOVE_SPEED  = 150.f;  ///< Horizontal movement speed (px/s).

    // ── Survival ──────────────────────────────────────────────────────────────
    static constexpr float HUNGER_TICK        = 4.f; ///< Seconds between hunger ticks (idle).
    static constexpr float HUNGER_TICK_MOVING = 2.f; ///< Seconds between hunger ticks (moving).
    static constexpr float SLEEP_TICK         = 8.f; ///< Seconds between sleep ticks (idle).
    static constexpr float SLEEP_TICK_MOVING  = 4.f; ///< Seconds between sleep ticks (moving).
    float hungerTimer = 0.f; ///< Time accumulator for hunger drain.
    float sleepTimer  = 0.f; ///< Time accumulator for sleep drain.

    // ── Combat / Mining ───────────────────────────────────────────────────────
    static constexpr float ATTACK_CD      = 0.5f;  ///< Player attack cooldown in seconds.
    static constexpr float BASE_MINE_TIME = 1.5f;  ///< Base seconds per mining hit.
    static constexpr float IFRAME_DURATION = 0.6f; ///< Invincibility window after being hit.

    float attackTimer = 0.f;  ///< Remaining attack cooldown.
    float shakeTimer  = 0.f;  ///< Remaining screen shake duration.
    float iframeTimer = 0.f;  ///< Remaining invincibility window after a zombie hit.
    MineState mine;            ///< State of the currently active mining action.

    std::vector<EventData> pendingEvents; ///< Events emitted this frame, consumed by main.cpp.

    /**
     * @brief Appends an event to the pending queue.
     * @param e      Event type.
     * @param detail Optional context string.
     */
    void emit(Event e, const std::string& detail = "");

    /**
     * @brief Returns true if a block of the given type exists within radius pixels.
     * @param blockType Block name to search for (e.g. "CraftingTable").
     * @param radius    Search radius in pixels. Defaults to 120.
     * @return True if a matching block is within range.
     */
    bool nearBlock(const std::string& blockType, float radius = 120.f) const;

    /**
     * @brief Handles player movement, jumping, gravity, and tile collision.
     * @param dt Delta time in seconds.
     */
    void tickPhysics(float dt);

    /**
     * @brief Drains hunger and sleep each tick; applies starvation HP loss.
     * @param dt Delta time in seconds.
     */
    void tickSurvival(float dt);

    /**
     * @brief Advances the mining timer and applies hits to the target block.
     * @param dt Delta time in seconds.
     */
    void tickMining(float dt);

    /**
     * @brief Ticks combat timers and starts iframe windows on zombie hits.
     * @param dt Delta time in seconds.
     */
    void tickCombat(float dt);

    /**
     * @brief Executes a melee attack against nearby animals and zombies.
     *
     * Respects the attack cooldown. Emits AnimalKilled/AnimalHit/ZombieKilled/
     * ZombieHit or NoAttackTarget depending on what is in range.
     */
    void handleAttack();

    /**
     * @brief Processes the player eating the selected food item.
     *
     * Blocks RawMeat, captures GoldenApple detection before consumption,
     * and emits PlayerWon or PlayerAte accordingly.
     */
    void handleEat();

    /**
     * @brief Resolves what item drops when a block is destroyed.
     *
     * If the inventory is full and the primary drop cannot fit, the block is
     * restored in the world and InventoryFull is emitted so the player knows.
     *
     * @param blk  Type name of the destroyed block.
     * @param row  Tile row (used to restore block if inventory is full).
     * @param col  Tile column (used to restore block if inventory is full).
     */
    void handleBlockDrop(const std::string& blk, int row, int col);
};
