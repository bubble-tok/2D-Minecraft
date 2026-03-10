#include "SaveManager.h"
#include "Game.h"
#include <fstream>
#include <iostream>

void SaveManager::save(Game& game) { // Save the current game state
    SaveManager s;// Create a temporary SaveManager object to store the data
    // Copy player position from the game
    s.playerPositionX = game.getPlayer().getPositionX();
    s.playerPositionY = game.getPlayer().getPositionY();
    // Copy player health and hunger values
    s.health          = game.getPlayer().getHealth();
    s.hunger          = game.getPlayer().getHungerLevel();
    // Serialize the player's inventory into a vector of strings
    // Each string represents an item and its properties
    s.inventory       = game.getPlayer().getInventory().serialize();
    s.saveToFile("savegame.txt");// Write the saved data to a file
    std::cout << "[SaveManager] Game saved.\n"; // Print confirmation message in terminal
}

void SaveManager::load(Game& game) { // Load a saved game state from file
    SaveManager s = SaveManager::loadFromFile("savegame.txt");  // Load data from file into a SaveManager object
    game.getPlayer().setPosition(s.playerPositionX, s.playerPositionY);  // Restore player position
    game.getPlayer().setHealth(s.health); // Restore health
    game.getPlayer().setHunger(s.hunger);  // Restore hunger level
    // Inventory restore: simple name/qty items only (extend for Food/Block subtypes)
    std::cout << "[SaveManager] Game loaded.\n";
}

void SaveManager::saveToFile(const std::string& filename) { // Write SaveManager data to a file
    std::ofstream file(filename); // Open file for writing
    if (!file) { std::cerr << "[SaveManager] Cannot write to " << filename << "\n"; return; }  // If file cannot be opened
    // Write player position, health and hunger level
    file << playerPositionX << "\n"
         << playerPositionY << "\n"
         << health          << "\n"
         << hunger          << "\n";
    for (auto& item : inventory) file << item << "\n";
}

SaveManager SaveManager::loadFromFile(const std::string& filename) {// Load SaveManager data from a file
    SaveManager s;  // Create SaveManager object to store loaded data
    std::ifstream file(filename);  // Open file for reading
    if (!file) { std::cerr << "[SaveManager] No save file found.\n"; return s; }  // If save file does not exist, return empty or default save data
    file >> s.playerPositionX >> s.playerPositionY >> s.health >> s.hunger; // Read player position, health, and hunger values
    file.ignore();
    std::string line;
    while (std::getline(file, line)) // Read inventory items line-by-line
        if (!line.empty()) s.inventory.push_back(line);
    return s;// Return the loaded save data
}
