#pragma once
#include "Player.h"
#include "World.h"
#include "InputHandler.h"
#include "SaveManager.h"
#include "CraftingSystem.h"

// this is the Game controller class
// This class connects all major systems together
class Game {
private:
    Player         player; // this is the player character
    World          world; // The game world including blocks + entities
    InputHandler   input; // keyboard input
    CraftingSystem craftingSystem;

public:
    // it will update function called every frame, and deltaTime = time between frames
    void update(float deltaTime) {
        // Movement
        if (input.moveLeft)  player.move(-1, deltaTime); // If player presses left key
        if (input.moveRight) player.move(1, deltaTime); // If player presses right key
        if (input.jump)      player.jump(); // Jump

        // Hotbar selection
        player.getInventory().selectSlot(input.selectedSlot); // Select inventory slot

        // Eating
        if (input.eat) player.eat();

        // Place item (row/col placement is handled by mouse click in main.cpp)
        // input.placeItem is processed there directly

        // Attack nearest target (animal or zombie in range)
        if (input.attack) {
            for (auto& animal : world.getAnimals()) {
                // If attack succeeds and animal dies
                if (player.attack(*animal) && !animal->isAlive()) {
                    auto meat = animal->dropMeat(); // Drop meat item
                    player.pickUp(meat); // Player picks up dropped meat
                }
            }
            // Attack zombies
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

    void saveGame() { SaveManager::save(*this); } // save game
    void loadGame() { SaveManager::load(*this); } // load game


    Player& getPlayer() { return player; }
    World& getWorld() { return world; }
    InputHandler& getInput() { return input; }
    CraftingSystem& getCrafting() { return craftingSystem; }
};