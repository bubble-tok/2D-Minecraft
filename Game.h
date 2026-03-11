/**
 * @file Game.h
 * @brief Defines the Game controller class that coordinates all major game systems.
 */

#pragma once
#include "Player.h"
#include "World.h"
#include "InputHandler.h"
#include "SaveManager.h"
#include "CraftingSystem.h"

/**
 * @class Game
 * @brief Central controller that connects and updates all game systems.
 *
 * The Game class manages interactions between the player, world,
 * input handling, crafting system, and saving/loading functionality.
 * It is responsible for updating game logic each frame.
 */
class Game {
private:
    Player         player;         ///< Player character controlled by the user
    World          world;          ///< Game world containing blocks and entities
    InputHandler   input;          ///< Handles keyboard and input states
    CraftingSystem craftingSystem; ///< Manages crafting recipes and crafting logic

public:

    /**
     * @brief Updates the entire game state for a single frame.
     *
     * Handles player input, combat, crafting interactions, world updates,
     * and saving/loading triggers.
     *
     * @param deltaTime Time elapsed since the last frame update
     */
    void update(float deltaTime) {

        /// Handle player movement input
        if (input.moveLeft)  player.move(-1, deltaTime);
        if (input.moveRight) player.move(1, deltaTime);
        if (input.jump)      player.jump();

        /// Update selected inventory hotbar slot
        player.getInventory().selectSlot(input.selectedSlot);

        /// Handle eating action
        if (input.eat) player.eat();

        /**
         * @note Item placement is handled by mouse clicks in main.cpp.
         * The Game class only coordinates high-level logic.
         */

        /// Attack nearest animals
        if (input.attack) {
            for (auto& animal : world.getAnimals()) {

                /// If attack succeeds and the animal dies
                if (player.attack(*animal) && !animal->isAlive()) {
                    auto meat = animal->dropMeat(); ///< Animal drops meat
                    player.pickUp(meat);            ///< Player collects dropped meat
                }
            }

            /// Attack zombies
            for (auto& zombie : world.getZombies())
                player.attack(*zombie);
        }

        /**
         * @note Mining is processed in the SFML main loop by creating a Block
         * reference from the targeted world tile and passing it to player.mineBlock().
         */

        /// Save or load the game if requested
        if (input.saveGame) saveGame();
        if (input.loadGame) loadGame();

        /// Reset one-frame input actions
        input.resetActions();

        /// Update world entities and player state
        world.update(deltaTime, player);
        player.update(deltaTime);
    }

    /**
     * @brief Saves the current game state.
     */
    void saveGame() { SaveManager::save(*this); }

    /**
     * @brief Loads a saved game state.
     */
    void loadGame() { SaveManager::load(*this); }

    /**
     * @brief Gets a reference to the player object.
     * @return Reference to the Player
     */
    Player& getPlayer() { return player; }

    /**
     * @brief Gets a reference to the world object.
     * @return Reference to the World
     */
    World& getWorld() { return world; }

    /**
     * @brief Gets a reference to the input handler.
     * @return Reference to the InputHandler
     */
    InputHandler& getInput() { return input; }

    /**
     * @brief Gets a reference to the crafting system.
     * @return Reference to the CraftingSystem
     */
    CraftingSystem& getCrafting() { return craftingSystem; }
};