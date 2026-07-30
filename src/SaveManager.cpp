/**
 * @file SaveManager.cpp
 * @brief Implements game state serialisation to and from a plain-text save file.
 *
 * Save file format (one value per line):
 * @code
 * playerPositionX
 * playerPositionY
 * health
 * hunger
 * sleep
 * <item1_serialised>
 * <item2_serialised>
 * ...
 * @endcode
 *
 * Item serialisation format:
 * - Food:  "name:qty:typeInt:hungerRestore:healthRestore"
 * - Block: "name:qty:typeInt"
 * - Other: "name:qty:typeInt"
 *
 * @author Group 46
 */
#include "SaveManager.h"
#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>

void SaveManager::save(Game& game) {
    SaveManager s;
    // Capture current player state into the DTO
    s.playerPositionX = game.getPlayer().getPositionX();
    s.playerPositionY = game.getPlayer().getPositionY();
    s.health          = game.getPlayer().getHealth();
    s.hunger          = game.getPlayer().getHungerLevel();
    s.sleep           = game.getPlayer().getSleepLevel();
    s.inventory       = game.getPlayer().getInventory().serialize();
    s.saveToFile("savegame.txt");
    std::cout << "[SaveManager] Game saved.\n";
}

void SaveManager::load(Game& game) {
    SaveManager s = SaveManager::loadFromFile("savegame.txt");

    // Restore player position and stats
    game.getPlayer().setPosition(s.playerPositionX, s.playerPositionY);
    game.getPlayer().setHealth(s.health);
    game.getPlayer().setHunger(s.hunger);
    game.getPlayer().setSleep(s.sleep);

    // Clear existing inventory before restoring from file
    Inventory& inv = game.getPlayer().getInventory();
    inv.clear();

    for (const auto& line : s.inventory) {
        // Split the colon-delimited line into parts
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ':'))
            parts.push_back(token);

        if (parts.size() < 3) continue; // skip malformed lines

        // Wrap stoi calls to gracefully handle corrupted save data
        try {
            std::string name    = parts[0];
            int         qty     = std::stoi(parts[1]);
            int         typeInt = std::stoi(parts[2]);
            ItemType    itemType = static_cast<ItemType>(typeInt);

            if (itemType == ItemType::FOOD && parts.size() >= 5) {
                // Restore Food with its hunger and health restore values
                int hungerR = std::stoi(parts[3]);
                int hpR     = std::stoi(parts[4]);
                inv.addItem(std::make_shared<Food>(name, qty, hungerR, hpR));
            } else if (itemType == ItemType::BLOCK) {
                inv.addItem(std::make_shared<Block>(name, qty));
            } else {
                // TOOL and MISC items — reconstructed as generic Items;
                // tool bonuses are re-applied at runtime from the item name
                inv.addItem(std::make_shared<Item>(name, qty, itemType));
            }
        } catch (const std::exception& e) {
            // Log and skip; do not crash on bad save data
            std::cerr << "[SaveManager] Skipping malformed line: "
                      << line << " (" << e.what() << ")\n";
        }
    }

    std::cout << "[SaveManager] Game loaded.\n";
}

void SaveManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "[SaveManager] Cannot write to " << filename << "\n";
        return;
    }
    // Write header stats
    file << playerPositionX << "\n"
         << playerPositionY << "\n"
         << health          << "\n"
         << hunger          << "\n"
         << sleep           << "\n";
    // Write each serialised inventory item on its own line
    for (auto& item : inventory) file << item << "\n";
}

SaveManager SaveManager::loadFromFile(const std::string& filename) {
    SaveManager s;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "[SaveManager] No save file found.\n";
        return s; // return defaults
    }
    // Read the five header fields
    file >> s.playerPositionX >> s.playerPositionY
         >> s.health >> s.hunger >> s.sleep;
    file.ignore(); // consume the newline after sleep

    // Read remaining lines as inventory entries
    std::string line;
    while (std::getline(file, line))
        if (!line.empty()) s.inventory.push_back(line);

    return s;
}
