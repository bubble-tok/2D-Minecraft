#include "SaveManager.h"
#include "Game.h"
#include <fstream>
#include <iostream>

/**
 * @brief Saves the current game state to a file.
 *
 * Copies relevant player data from the game, including position,
 * health, hunger, and serialized inventory contents, then writes
 * the data to the default save file.
 *
 * @param game Reference to the current game instance
 */
void SaveManager::save(Game& game) {
    SaveManager s; ///< Temporary SaveManager object used to store save data

    /// Copy player position from the game
    s.playerPositionX = game.getPlayer().getPositionX();
    s.playerPositionY = game.getPlayer().getPositionY();

    /// Copy player health and hunger values
    s.health          = game.getPlayer().getHealth();
    s.hunger          = game.getPlayer().getHungerLevel();

    /// Serialize the player's inventory into a vector of strings
    s.inventory       = game.getPlayer().getInventory().serialize();

    /// Write the saved data to a file
    s.saveToFile("savegame.txt");

    /// Print confirmation message
    std::cout << "[SaveManager] Game saved.\n";
}

/**
 * @brief Loads a saved game state from a file.
 *
 * Restores player position, health, and hunger values from the
 * default save file. Inventory restoration is not fully implemented
 * here and would need extension for subtype reconstruction.
 *
 * @param game Reference to the game instance being restored
 */
void SaveManager::load(Game& game) {
    SaveManager s = SaveManager::loadFromFile("savegame.txt");  ///< Load save data from file

    /// Restore player position
    game.getPlayer().setPosition(s.playerPositionX, s.playerPositionY);

    /// Restore player health
    game.getPlayer().setHealth(s.health);

    /// Restore player hunger
    game.getPlayer().setHunger(s.hunger);

    /**
     * @note Inventory restoration currently supports only serialized
     * item strings and would need to be extended for reconstructing
     * Food, Block, and other item subtypes.
     */
    std::cout << "[SaveManager] Game loaded.\n";
}

/**
 * @brief Writes the current SaveManager data to a file.
 *
 * Saves player position, health, hunger, and inventory contents
 * in plain text format.
 *
 * @param filename Name of the file to write save data to
 */
void SaveManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename); ///< Output file stream

    /// Handle file open failure
    if (!file) {
        std::cerr << "[SaveManager] Cannot write to " << filename << "\n";
        return;
    }

    /// Write player position, health, and hunger values
    file << playerPositionX << "\n"
         << playerPositionY << "\n"
         << health          << "\n"
         << hunger          << "\n";

    /// Write serialized inventory items line by line
    for (auto& item : inventory)
        file << item << "\n";
}

/**
 * @brief Loads save data from a file.
 *
 * Reads player position, health, hunger, and serialized inventory
 * entries from the specified file.
 *
 * @param filename Name of the file to read save data from
 * @return SaveManager Object containing the loaded save data
 */
SaveManager SaveManager::loadFromFile(const std::string& filename) {
    SaveManager s;      ///< SaveManager object used to store loaded data
    std::ifstream file(filename); ///< Input file stream

    /// Handle missing save file
    if (!file) {
        std::cerr << "[SaveManager] No save file found.\n";
        return s;
    }

    /// Read player position, health, and hunger values
    file >> s.playerPositionX >> s.playerPositionY >> s.health >> s.hunger;
    file.ignore();

    std::string line;

    /// Read inventory items line by line
    while (std::getline(file, line))
        if (!line.empty())
            s.inventory.push_back(line);

    /// Return the loaded save data
    return s;
}