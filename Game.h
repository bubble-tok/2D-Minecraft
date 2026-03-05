// header for game.h
#pragma once
#include "Player.h"
#include "World.h"

class Game {
private:
	Player player;
	World world;
public:
	void saveGame();
	void loadGame();
	Player& getPlayer() {
		return player;
	}
	World& getWorld() {
		return world;
	}
};