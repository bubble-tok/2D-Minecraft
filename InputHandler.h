/**
 * @file InputHandler.h
 * @brief Handles player input states and key events.
 */

#pragma once

/**
 * @class InputHandler
 * @brief Stores and processes player input states for each frame.
 *
 * This class tracks keyboard actions such as movement, attacking,
 * eating, mining, and hotbar selection. The game loop updates these
 * values each frame and resets one-shot actions after processing them.
 */
class InputHandler {
public:

    /// True when the player is holding the move-left key
    bool moveLeft   = false;

    /// True when the player is holding the move-right key
    bool moveRight  = false;

    /// True when the player triggers a jump action
    bool jump       = false;

    /// True when the player performs an attack action
    bool attack     = false;

    /// True when the player consumes food
    bool eat        = false;

    /// True when the player places an item or block
    bool placeItem  = false;

    /// True while the left mouse button is held on a block for mining
    bool mineHeld   = false;

    /// True when the player requests to save the game
    bool saveGame   = false;

    /// True when the player requests to load the game
    bool loadGame   = false;

    /// Currently selected hotbar slot (range 0–8)
    int selectedSlot = 0;

    /**
     * @brief Resets one-shot actions after they are processed.
     *
     * Continuous inputs such as movement are not reset here.
     * This function should be called once per frame after
     * input actions have been handled.
     */
    void resetActions() {
        jump      = false;
        attack    = false;
        eat       = false;
        placeItem = false;
        saveGame  = false;
        loadGame  = false;
    }

    /**
     * @brief Handles a key press event.
     *
     * Updates the corresponding input state based on the
     * pressed key code.
     *
     * @param sfmlKey Integer representing the pressed key code
     *
     * @note When integrating with SFML, replace these integers
     * with `sf::Keyboard::*` values.
     */
    void onKeyPressed(int sfmlKey) {

        // SFML key codes (replace with sf::Keyboard::* when integrating SFML)
        // A=0, D=3, W=22, Space=57, E=4, F=5, Q=16, S=18, F5=63, F9=67
        switch (sfmlKey) {
            case 0:  moveLeft  = true; break;  ///< A key
            case 3:  moveRight = true; break;  ///< D key
            case 57: jump      = true; break;  ///< Space key
            case 4:  eat       = true; break;  ///< E key
            case 5:  attack    = true; break;  ///< F key or left mouse button
            case 16: placeItem = true; break;  ///< Q key or right mouse button
            case 63: saveGame  = true; break;  ///< F5 key
            case 67: loadGame  = true; break;  ///< F9 key
        }

        /// Number keys 1–9 for hotbar selection
        if (sfmlKey >= 27 && sfmlKey <= 35)
            selectedSlot = sfmlKey - 27;
    }

    /**
     * @brief Handles a key release event.
     *
     * Used to stop continuous movement actions when keys are released.
     *
     * @param sfmlKey Integer representing the released key code
     */
    void onKeyReleased(int sfmlKey) {
        switch (sfmlKey) {
            case 0: moveLeft  = false; break;
            case 3: moveRight = false; break;
        }
    }
};