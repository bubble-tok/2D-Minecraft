#SFML Survival Game

This project is a 2D minecraft-style game written in C++ using
SFML. The game includes systems for entities, 
monsters, animals, inventory management, crafting, hunger, and health. 
Players explore the world, gather resources, craft items, and try to survive.

--- 

##Features 
- Player movement and interaction - Entity system (animals,
monsters, blocks) 
- Inventory system 
- Crafting system 
- Hunger and health mechanics 
- Resource gathering 
- Basic world interactions

##Requirements 
- C++ Compiler (Visual Studio recommended on Windows) 
- SFML Library (version 2.5 or later)

Download SFML: https://www.sfml-dev.org/download.php

##How to Build (Windows / Visual Studio) 
1. Install SFML. 
2. Open the project solution in Visual Studio. 
3. Configure SFML paths.
Include Directory: SFML/include
Library Directory: SFML/lib
4. Build the project: Build → Build Solution

5. The executable will appear in: x64/Debug/ or x64/Release/

Linux / Mac Example 
``` Build g++ Source/*.cpp -o game -lsfml-graphics
-lsfml-window -lsfml-system ```

**Run**: ./game

Controls 
W / A / S / D : Move player 
E : Interact 
I : Open inventory 
C : Craft items 
ESC : Exit game

Project Structure Source/ 
- main.cpp 
- Game.cpp 
- Game.h 
- Entity.h 
-Animal.h 
- Monster.h 
- Inventory.h
- CraftingSystem.h 
- InputHandler.h

