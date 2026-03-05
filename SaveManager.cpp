/*
* This file is to write game data to a file and load from a file and converting game states
*/
#include "Game.h"
#include "SaveManager.h"
#include <fstream>
#include <sstream>

void SaveManager::save(Game& game) {
	SaveManager save;
	save.playerPositionX = game.getPlayer().getPositionX();
	save.playerPositionY = game.getPlayer().getPositionY();
	save.health = game.getPlayer().getHealth();
	save.hunger = game.getPlayer().getHunger();
	save.saveToFile("savegame.txt");
}
void SaveManager::load(Game& game) {
	SaveManager save = SaveManager::loadFromfile("savegame.txt");
	game.getPlayer().setPosition(save.playerPositionX, save.playerPositionY);
	game.getPlayer().setHealth(save.health);
	game.getPlayer().setHunger(save.hunger);
}
void SaveManager::saveToFile(const std::string& filename) {
	std::ofstream file(filename);
	file << playerPositionX << std::endl;
	file << playerPositionY << std::endl;
	file << health << std::endl;
	file << hunger << std::endl;
	for (auto& item : inventory) {
		file << item << ",";
	}
	file.close();
}
//load the game state from a file
SaveManager SaveManager::loadFromfile(const std::string& filename) {
	SaveManager save;
	std::string items;
	std::ifstream file(filename);
	file >> save.playerPositionX;
	file >> save.playerPositionY;
	file >> save.health;
	file >> save.hunger;
	file >> items;
	std::stringstream ss(items);
	std::string item;
	while (getline(ss, item, ',')) {
		save.inventory.push_back(item);
	}
	file.close();
	return save;

}