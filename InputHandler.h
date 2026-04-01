#pragma once
#include <SFML/Window/Keyboard.hpp>

/**
 * @class InputHandler
 * @brief Tracks per-frame keyboard input state for the game loop.
 *
 * Continuous actions (moveLeft, moveRight, mineHeld) are set/cleared
 * on key press/release. One-shot actions (jump, attack, eat, etc.) are
 * set on press and must be cleared via resetActions() after processing.
 */
class InputHandler {
public:
    // Continuous state
    bool moveLeft   = false;
    bool moveRight  = false;
    bool mineHeld   = false; ///< true while LMB is held on a block

    // One-shot actions (cleared each frame by resetActions)
    bool jump       = false;
    bool attack     = false;
    bool eat        = false;
    bool placeItem  = false;
    bool saveGame   = false;
    bool loadGame   = false;

    // Hotbar slot selection (0–8)
    int  selectedSlot = 0;

    /// Reset all one-shot actions. Call once per frame after processing.
    void resetActions() {
        jump      = false;
        attack    = false;
        eat       = false;
        placeItem = false;
        saveGame  = false;
        loadGame  = false;
    }

    /**
     * @brief Update state from an SFML key-pressed event.
     *
     * Uses sf::Keyboard::Key enum values — these are stable across
     * platforms and match what sf::Event::KeyPressed delivers.
     *
     * @param key The sf::Keyboard::Key from the event
     */
    void onKeyPressed(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::A:     moveLeft  = true;  break;
            case sf::Keyboard::D:     moveRight = true;  break;
            case sf::Keyboard::Space: jump      = true;  break;
            case sf::Keyboard::E:     eat       = true;  break;
            case sf::Keyboard::F:     attack    = true;  break;
            case sf::Keyboard::Q:     placeItem = true;  break;
            case sf::Keyboard::F5:    saveGame  = true;  break;
            case sf::Keyboard::F9:    loadGame  = true;  break;
            // Hotbar: keys Num1–Num9
            case sf::Keyboard::Num1:  selectedSlot = 0;  break;
            case sf::Keyboard::Num2:  selectedSlot = 1;  break;
            case sf::Keyboard::Num3:  selectedSlot = 2;  break;
            case sf::Keyboard::Num4:  selectedSlot = 3;  break;
            case sf::Keyboard::Num5:  selectedSlot = 4;  break;
            case sf::Keyboard::Num6:  selectedSlot = 5;  break;
            case sf::Keyboard::Num7:  selectedSlot = 6;  break;
            case sf::Keyboard::Num8:  selectedSlot = 7;  break;
            case sf::Keyboard::Num9:  selectedSlot = 8;  break;
            default: break;
        }
    }

    /**
     * @brief Update state from an SFML key-released event.
     * @param key The sf::Keyboard::Key from the event
     */
    void onKeyReleased(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::A:     moveLeft  = false; break;
            case sf::Keyboard::D:     moveRight = false; break;
            case sf::Keyboard::Space: jump      = false; break;
            default: break;
        }
    }
};
