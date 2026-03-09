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
        // Movement
        if (input.moveLeft)  player.move(-1, deltaTime);
        if (input.moveRight) player.move(1, deltaTime);
        if (input.jump)      player.jump();

        // Hotbar selection
        player.getInventory().selectSlot(input.selectedSlot);

        // Eating
        if (input.eat) player.eat();

        // Place item (row/col placement is handled by mouse click in main.cpp)
        // input.placeItem is processed there directly

        // Attack nearest target (animal or zombie in range)
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

        // Mining (held)
        // In SFML loop: create a Block reference from the targeted world block
        // and pass to player.mineBlock(); on return true → pickUp + remove from world

        // Save / Load
        if (input.saveGame) saveGame();
        if (input.loadGame) loadGame();

        input.resetActions();

        // Update world entities
        world.update(deltaTime, player);
        player.update(deltaTime);
    }

    void saveGame() { SaveManager::save(*this); }
    void loadGame() { SaveManager::load(*this); }


    Player& getPlayer() { return player; }
    World& getWorld() { return world; }
    InputHandler& getInput() { return input; }
    CraftingSystem& getCrafting() { return craftingSystem; }
};