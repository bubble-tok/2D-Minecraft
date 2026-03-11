#pragma once
#include <string>
#include <vector>

class Game;

/**
 * @class SaveManager
 * @brief Handles saving and loading the game state to and from files.
 *
 * The SaveManager stores player-related game data such as position,
 * health, hunger level, and inventory contents. It provides functions
 * for writing this data to a file and restoring it when loading a game.
 */
class SaveManager {
public:

    /**
     * @brief Player's saved X position in the world.
     */
    float playerPositionX = 0.f;

    /**
     * @brief Player's saved Y position in the world.
     */
    float playerPositionY = 0.f;

    /**
     * @brief Player's saved health value.
     */
    int health = 100;

    /**
     * @brief Player's saved hunger level.
     */
    int hunger = 100;

    /**
     * @brief Serialized inventory data.
     *
     * Each string represents a serialized item stored in the player's inventory.
     */
    std::vector<std::string> inventory;

    /**
     * @brief Writes the current SaveManager data to a file.
     *
     * Saves player position, health, hunger, and serialized inventory data.
     *
     * @param filename Name of the file to write save data to
     */
    void saveToFile(const std::string& filename);

    /**
     * @brief Loads save data from a file.
     *
     * Reads player position, health, hunger, and serialized inventory data
     * from the specified file and returns a SaveManager object containing
     * the loaded information.
     *
     * @param filename Name of the file to load save data from
     * @return SaveManager Object containing loaded game data
     */
    static SaveManager loadFromFile(const std::string& filename);

    /**
     * @brief Saves the current game state.
     *
     * Extracts relevant data (player position, health, hunger, inventory)
     * from the Game object and writes it to a save file.
     *
     * @param game Reference to the game instance to save
     */
    static void save(Game& game);

    /**
     * @brief Loads a saved game state into the Game object.
     *
     * Reads save data from file and restores player position,
     * health, hunger, and inventory values in the Game instance.
     *
     * @param game Reference to the game instance to restore
     */
    static void load(Game& game);
};