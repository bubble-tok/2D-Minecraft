/**
 * @file SaveManager.h
 * @brief Declares SaveManager for serialising and restoring complete game state.
 *
 * @author Group 46
 */
#pragma once
#include <string>
#include <vector>

class Game;

/**
 * @class SaveManager
 * @brief Handles reading and writing complete game state to a plain-text save file.
 *
 * Save format sections: header fields, WORLDMAP, ENTITIES, INVENTORY.
 * The static save() and load() methods are the primary public interface;
 * instance members are a plain data-transfer object used internally.
 *
 * @author Group 46
 */
class SaveManager {
public:
    // ── Player core ───────────────────────────────────────────────────────────
    float playerPositionX = 0.f;   ///< Saved player X coordinate in pixels.
    float playerPositionY = 0.f;   ///< Saved player Y coordinate in pixels.
    int   health          = 100;   ///< Saved player HP.
    int   hunger          = 100;   ///< Saved player hunger level.
    int   sleep           = 100;   ///< Saved player sleep level.

    // ── Time ─────────────────────────────────────────────────────────────────
    float dayTime  = 0.15f; ///< Normalised day-cycle position [0, 1).
    int   dayCount = 1;     ///< Total days elapsed since game start.

    // ── Physics / survival timers ─────────────────────────────────────────────
    float playerVY       = 0.f;  ///< Player vertical velocity at save time.
    bool  playerOnGround = true; ///< Whether the player was grounded at save time.
    float hungerTimer    = 0.f;  ///< Hunger drain timer accumulator.
    float sleepTimer     = 0.f;  ///< Sleep drain timer accumulator.

    // ── Inventory ─────────────────────────────────────────────────────────────
    int selectedSlot = 0;                     ///< Active hotbar slot index.
    std::vector<std::string> inventory;       ///< Serialised inventory item strings.

    // ── World ─────────────────────────────────────────────────────────────────
    std::vector<std::vector<std::string>> worldBlocks; ///< Full tile block map.
    std::vector<std::string>              entityData;  ///< Serialised entity lines.

    /**
     * @brief Writes this SaveManager's data to a file.
     * @param filename Path to the save file to write.
     */
    void saveToFile(const std::string& filename);

    /**
     * @brief Reads a save file into a new SaveManager instance.
     * @param filename Path to the save file to read.
     * @return Populated SaveManager; default values if the file cannot be opened.
     */
    static SaveManager loadFromFile(const std::string& filename);

    /**
     * @brief Captures the current game state and writes it to "savegame.txt".
     * @param game The running Game instance to snapshot.
     */
    static void save(Game& game);

    /**
     * @brief Reads "savegame.txt" and restores the game state.
     * @param game The running Game instance to restore into.
     */
    static void load(Game& game);
};
