#pragma once
#include "Player.h"
#include "World.h"
#include "InputHandler.h"
#include "SaveManager.h"
#include "CraftingSystem.h"


class Game {
private:
    Player         player;
    World          world;
    InputHandler   input;
    CraftingSystem craftingSystem;

public:
    
    void update(float deltaTime) {
        if (input.moveLeft)  player.move(-1, deltaTime);
        if (input.moveRight) player.move(1, deltaTime);
        if (input.jump)      player.jump();
        player.getInventory().selectSlot(input.selectedSlot);
        if (input.eat) player.eat();
        if (input.attack) {
            for (auto& animal : world.getAnimals()) {
                if (player.attack(*animal) && !animal->isAlive()) {
                    auto meat = animal->dropMeat();
                    player.pickUp(meat);
                }
            }
            for (auto& zombie : world.getZombies())
                player.attack(*zombie);
        }
        if (input.saveGame) saveGame();
        if (input.loadGame) loadGame();
        input.resetActions();
        world.update(deltaTime, player);
        // player.update() removed - physics and hunger handled in main.cpp
    }

    void saveGame() { SaveManager::save(*this); }
    void loadGame() { SaveManager::load(*this); }


    Player& getPlayer() { return player; }
    World& getWorld() { return world; }
    InputHandler& getInput() { return input; }
    CraftingSystem& getCrafting() { return craftingSystem; }
};
