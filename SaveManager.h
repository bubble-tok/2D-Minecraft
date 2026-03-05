// header file for save manager class
#pragma once
#include <string>
#include <vector>

class Game;
class SaveManager {
public:
	float playerPositionX;
	float playerPositionY;
	int health;
	int hunger;
	std::vector<std::string> inventory;
	void saveToFile(const std::string& filename);
	static SaveManager loadFromfile(const std::string& filename);
	static void save(Game& game);
	static void load(Game& game);
};