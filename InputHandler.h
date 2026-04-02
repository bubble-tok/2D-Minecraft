/**
 * @file InputHandler.h
 * @brief Declares the InputHandler class for keyboard state tracking.
 *
 * @author Group 46
 */

#pragma once
#include <SFML/Window/Keyboard.hpp>

/**
 * @class InputHandler
 * @brief Tracks per-frame keyboard input state for the game loop.
 *
 * Separates continuous state (held keys) from one-shot edge triggers:
 * - moveLeft / moveRight are set on press and cleared on release.
 * - jumpPressed is a one-frame edge trigger: true only on the first frame
 *   Space is pressed (guarded by the spaceHeld flag).
 * - spaceHeld is continuous and used by physics for variable-height jumping.
 *
 * beginFrame() must be called at the start of each frame to clear one-shot
 * flags before new SFML events are polled.
 *
 * @author Group 46
 */
class InputHandler {
public:
    bool moveLeft    = false; ///< True while the A key is held.
    bool moveRight   = false; ///< True while the D key is held.
    bool jumpPressed = false; ///< True for exactly one frame when Space is first pressed.
    bool spaceHeld   = false; ///< True while Space is physically held down.
    bool mineHeld    = false; ///< True while left mouse button is held on a block.
    bool attack      = false; ///< One-shot: true when F is pressed.
    bool eat         = false; ///< One-shot: true when E is pressed.
    bool placeItem   = false; ///< One-shot: true when Q is pressed.
    bool saveGame    = false; ///< One-shot: true when F5 is pressed.
    bool loadGame    = false; ///< One-shot: true when F9 is pressed.
    int  selectedSlot = 0;   ///< Hotbar slot index (0–8) controlled by Num1–Num9.

    /**
     * @brief Clears all one-shot flags at the start of each frame.
     *
     * Must be called before polling SFML events so that flags set during the
     * previous frame are reset before new events update them.
     */
    void beginFrame();

    /**
     * @brief Updates input state from an SFML key-pressed event.
     *
     * Sets continuous flags (moveLeft, moveRight, spaceHeld) and one-shot
     * edge triggers (jumpPressed) based on the key that was pressed.
     * jumpPressed uses spaceHeld to ensure it fires only once per press.
     *
     * @param key The sf::Keyboard::Key value from the SFML event.
     */
    void onKeyPressed(sf::Keyboard::Key key);

    /**
     * @brief Updates input state from an SFML key-released event.
     *
     * Clears continuous flags for A, D, and Space when those keys are released.
     *
     * @param key The sf::Keyboard::Key value from the SFML event.
     */
    void onKeyReleased(sf::Keyboard::Key key);

    /**
     * @brief Alias for beginFrame() provided for backward compatibility.
     */
    void resetActions();
};
