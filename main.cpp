// Main function(temporary)
#include "Game.h"
#include <iostream>

int main()
{
    Game game;
	/*Test code to demonstrate before save
    game.getPlayer().setPosition(120, 340);
    game.getPlayer().setHealth(80);
    game.getPlayer().setHunger(60);

    std::cout << "Before saving\n";
    */

    std::cout << "Player position: "
        << game.getPlayer().getPositionX() << ", "
        << game.getPlayer().getPositionY() << "\n";

    std::cout << "Player health: " << game.getPlayer().getHealth() << "\n";
    std::cout << "Player hunger: " << game.getPlayer().getHunger() << "\n";

    game.saveGame();
	/* test code to demonstrate after save, change player state
    std::cout << "\nSaved game\n";
    game.getPlayer().setPosition(0, 0);
    game.getPlayer().setHealth(10);
    game.getPlayer().setHunger(5);

    std::cout << "\nAfter changing player state\n";

    std::cout << "Player position: "
        << game.getPlayer().getPositionX() << ", "
        << game.getPlayer().getPositionY() << "\n";

    std::cout << "Player health: " << game.getPlayer().getHealth() << "\n";
    std::cout << "Player hunger: " << game.getPlayer().getHunger() << "\n";
    */
    game.loadGame();

    std::cout << "\nLoaded game\n";

    std::cout << "Player position: "
        << game.getPlayer().getPositionX() << ", "
        << game.getPlayer().getPositionY() << "\n";

    std::cout << "Player health: " << game.getPlayer().getHealth() << "\n";
    std::cout << "Player hunger: " << game.getPlayer().getHunger() << "\n";


}