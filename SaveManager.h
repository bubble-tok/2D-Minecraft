#pragma once
#include <string>
#include <vector>

class Game; 

// SaveManager handles saving and loading game state to/from a file
class SaveManager {
public:
    //Stored player data 
    // These variables hold the information we want to save about the player's current game state
    float playerPositionX = 0.f;
    float playerPositionY = 0.f;
    int   health          = 100;
    int   hunger          = 100;
    std::vector<std::string> inventory;// Inventory items serialized as strings

    void saveToFile(const std::string& filename);   // Save the current SaveManager data to a file
    static SaveManager loadFromFile(const std::string& filename); // Load saved data from a file and return a SaveManager object containing the loaded values
    static void save(Game& game);  // Extract game data like player position, health, inventory from the Game object and write it to a save file
    static void load(Game& game); // Load saved data from file and apply it to the Game object
};
