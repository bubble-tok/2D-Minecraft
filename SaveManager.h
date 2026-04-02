#pragma once
#include <string>
#include <vector>

class Game;

class SaveManager {
public:
    float playerPositionX = 0.f;
    float playerPositionY = 0.f;
    int   health          = 100;
    int   hunger          = 100;
    int   sleep           = 100;  ///< Added: sleep level persistence
    std::vector<std::string> inventory;

    void saveToFile(const std::string& filename);
    static SaveManager loadFromFile(const std::string& filename);
    static void save(Game& game);
    static void load(Game& game);
};
