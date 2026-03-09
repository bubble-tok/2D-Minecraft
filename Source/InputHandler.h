#pragma once


class InputHandler {
public:
    // Keyboard state (set by the game loop each frame)
    bool moveLeft   = false;
    bool moveRight  = false;
    bool jump       = false;
    bool attack     = false;
    bool eat        = false;
    bool placeItem  = false;
    bool mineHeld   = false;  ///< true while LMB is held on a block
    bool saveGame   = false;
    bool loadGame   = false;

    // Hotbar slot selection (0–8)
    int  selectedSlot = 0;

    /// Reset all one-shot actions (called after each frame processes them).
    void resetActions() {
        jump      = false;
        attack    = false;
        eat       = false;
        placeItem = false;
        saveGame  = false;
        loadGame  = false;
    }

    
    void onKeyPressed(int sfmlKey) {
        // SFML key codes (replace with sf::Keyboard::* when integrating SFML)
        // A=0, D=3, W=22, Space=57, E=4, F=5, Q=16, S=18, F5=63, F9=67
        switch (sfmlKey) {
            case 0:  moveLeft  = true; break; // A
            case 3:  moveRight = true; break; // D
            case 57: jump      = true; break; // Space
            case 4:  eat       = true; break; // E
            case 5:  attack    = true; break; // F  (or left-click)
            case 16: placeItem = true; break; // Q  (or right-click)
            case 63: saveGame  = true; break; // F5
            case 67: loadGame  = true; break; // F9
        }
        // Number keys 1–9 for hotbar
        if (sfmlKey >= 27 && sfmlKey <= 35)
            selectedSlot = sfmlKey - 27;
    }

    void onKeyReleased(int sfmlKey) {
        switch (sfmlKey) {
            case 0: moveLeft  = false; break;
            case 3: moveRight = false; break;
        }
    }
};
