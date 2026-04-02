/**
 * @file SaveManager.h
 * @brief Declares SaveManager for serialising and restoring game state.
 *
 * @author Group 46
 */
#pragma once
#include <string>
#include <vector>

class Game;

/**
 * @class SaveManager
 * @brief Handles reading and writing game save data to disk.
 *
 * Save data is stored in a plain-text file ("savegame.txt") with one value
 * per line: position, stats, then a list of serialised inventory items.
 *
 * The static save() and load() methods are the primary public interface;
 * the instance members are a plain data transfer object used internally.
 *
 * @author Group 46
 */
class SaveManager {
public:
    float playerPositionX = 0.f; ///< Saved player X coordinate in pixels.
    float playerPositionY = 0.f; ///< Saved player Y coordinate in pixels.
    int   health          = 100; ///< Saved player HP.
    int   hunger          = 100; ///< Saved player hunger level.
    int   sleep           = 100; ///< Saved player sleep level.
    std::vector<std::string> inventory; ///< Serialised inventory item strings.

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
     *
     * Clears the player's inventory before restoring it. Malformed inventory
     * lines are skipped with a warning rather than crashing.
     *
     * @param game The running Game instance to restore into.
     */
    static void load(Game& game);
};
