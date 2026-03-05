/*
* This file implements the save and load functionality
*/
#include "Game.h"
#include "SaveManager.h"

void Game::saveGame() {
	SaveManager::save(*this);
}
void Game::loadGame() {
	SaveManager::load(*this);
}
