/**
 * @file InputHandler.cpp
 * @brief Implements InputHandler keyboard state tracking.
 *
 * Handles input from the player, to perform actions in game.
 * Allow player to change hotbar slots, movement, attacking, save/load
 * @author Group 46
 */

#include "InputHandler.h"

void InputHandler::beginFrame() {
    // Clear all one-shot flags — they will be re-set if the key is pressed this frame
    jumpPressed = false;
    attack      = false;
    eat         = false;
    placeItem   = false;
    saveGame    = false;
    loadGame    = false;
}

void InputHandler::resetActions() {
    // Delegates to beginFrame for backward compatibility
    beginFrame();
}

void InputHandler::onKeyPressed(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::A:     moveLeft   = true;  break;
        case sf::Keyboard::D:     moveRight  = true;  break;

        case sf::Keyboard::Space:
            // jumpPressed is an edge trigger — only fire on the first press,
            // not every frame while Space is held
            if (!spaceHeld) jumpPressed = true;
            spaceHeld = true;
            break;

        case sf::Keyboard::F:     attack    = true;   break;
        case sf::Keyboard::F5:    saveGame  = true;   break;
        case sf::Keyboard::F9:    loadGame  = true;   break;

        // Hotbar slot selection: Num1–Num9 map to slots 0–8
        case sf::Keyboard::Num1:  selectedSlot = 0;   break;
        case sf::Keyboard::Num2:  selectedSlot = 1;   break;
        case sf::Keyboard::Num3:  selectedSlot = 2;   break;
        case sf::Keyboard::Num4:  selectedSlot = 3;   break;
        case sf::Keyboard::Num5:  selectedSlot = 4;   break;
        case sf::Keyboard::Num6:  selectedSlot = 5;   break;
        case sf::Keyboard::Num7:  selectedSlot = 6;   break;
        case sf::Keyboard::Num8:  selectedSlot = 7;   break;
        case sf::Keyboard::Num9:  selectedSlot = 8;   break;

        default: break;
    }
}

void InputHandler::onKeyReleased(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::A:     moveLeft  = false; break;
        case sf::Keyboard::D:     moveRight = false; break;
        case sf::Keyboard::Space: spaceHeld = false; break; // re-enables next jumpPressed
        default: break;
    }
}
