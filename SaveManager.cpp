#include "SaveManager.h"
#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>

void SaveManager::save(Game& game) {
    SaveManager s;
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
    game.getPlayer().setPosition(s.playerPositionX, s.playerPositionY);
    game.getPlayer().setHealth(s.health);
    game.getPlayer().setHunger(s.hunger);
    game.getPlayer().setSleep(s.sleep);

    // Restore inventory from serialized strings.
    // Format for Food:  name:qty:type:hungerRestore:healthRestore
    // Format for Block: name:qty:type
    // Format for Item:  name:qty:type
    Inventory& inv = game.getPlayer().getInventory();
    inv.clear();

    for (const auto& line : s.inventory) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ':'))
            parts.push_back(token);

        if (parts.size() < 3) continue;

        std::string name     = parts[0];
        int         qty      = std::stoi(parts[1]);
        int         typeInt  = std::stoi(parts[2]);
        ItemType    itemType = static_cast<ItemType>(typeInt);

        if (itemType == ItemType::FOOD && parts.size() >= 5) {
            int hungerR = std::stoi(parts[3]);
            int hpR     = std::stoi(parts[4]);
            inv.addItem(std::make_shared<Food>(name, qty, hungerR, hpR));
        } else if (itemType == ItemType::BLOCK) {
            inv.addItem(std::make_shared<Block>(name, qty));
        } else {
            inv.addItem(std::make_shared<Item>(name, qty, itemType));
        }
    }

    std::cout << "[SaveManager] Game loaded.\n";
}

void SaveManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) { std::cerr << "[SaveManager] Cannot write to " << filename << "\n"; return; }
    file << playerPositionX << "\n"
         << playerPositionY << "\n"
         << health          << "\n"
         << hunger          << "\n"
         << sleep           << "\n";
    for (auto& item : inventory) file << item << "\n";
}

SaveManager SaveManager::loadFromFile(const std::string& filename) {
    SaveManager s;
    std::ifstream file(filename);
    if (!file) { std::cerr << "[SaveManager] No save file found.\n"; return s; }
    file >> s.playerPositionX >> s.playerPositionY
         >> s.health >> s.hunger >> s.sleep;
    file.ignore();
    std::string line;
    while (std::getline(file, line))
        if (!line.empty()) s.inventory.push_back(line);
    return s;
}
