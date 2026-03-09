#include "SaveManager.h"
#include "Game.h"
#include <fstream>
#include <iostream>

void SaveManager::save(Game& game) {
    SaveManager s;
    s.playerPositionX = game.getPlayer().getPositionX();
    s.playerPositionY = game.getPlayer().getPositionY();
    s.health          = game.getPlayer().getHealth();
    s.hunger          = game.getPlayer().getHungerLevel();
    s.inventory       = game.getPlayer().getInventory().serialize();
    s.saveToFile("savegame.txt");
    std::cout << "[SaveManager] Game saved.\n";
}

void SaveManager::load(Game& game) {
    SaveManager s = SaveManager::loadFromFile("savegame.txt");
    game.getPlayer().setPosition(s.playerPositionX, s.playerPositionY);
    game.getPlayer().setHealth(s.health);
    game.getPlayer().setHunger(s.hunger);
    // Inventory restore: simple name/qty items only (extend for Food/Block subtypes)
    std::cout << "[SaveManager] Game loaded.\n";
}

void SaveManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) { std::cerr << "[SaveManager] Cannot write to " << filename << "\n"; return; }
    file << playerPositionX << "\n"
         << playerPositionY << "\n"
         << health          << "\n"
         << hunger          << "\n";
    for (auto& item : inventory) file << item << "\n";
}

SaveManager SaveManager::loadFromFile(const std::string& filename) {
    SaveManager s;
    std::ifstream file(filename);
    if (!file) { std::cerr << "[SaveManager] No save file found.\n"; return s; }
    file >> s.playerPositionX >> s.playerPositionY >> s.health >> s.hunger;
    file.ignore();
    std::string line;
    while (std::getline(file, line))
        if (!line.empty()) s.inventory.push_back(line);
    return s;
}
