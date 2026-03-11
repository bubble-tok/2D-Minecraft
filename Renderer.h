/**
 * @file Renderer.h
 * @brief Defines the Renderer class responsible for drawing the game world, entities, player, and HUD.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "World.h"
#include "Inventory.h"

/**
 * @class Renderer
 * @brief Handles all visual rendering for the game.
 *
 * The Renderer class draws the background, world tiles, entities,
 * player character, mining overlays, and heads-up display (HUD).
 * It also manages font loading and helper drawing functions.
 */
class Renderer {
private:
    sf::RenderWindow& window; ///< Reference to the SFML render window
    sf::Font          font;   ///< Font used for on-screen text
    bool              fontLoaded = false; ///< True if a font was successfully loaded

    static const int TILE = 40; ///< Tile size in pixels

    /**
     * @brief Returns the base display color for a block type.
     *
     * @param type Name of the block type
     * @return Color associated with the block
     */
    sf::Color blockColor(const std::string& type) {
        if (type == "Grass")  return sf::Color(106, 127, 60);
        if (type == "Dirt")   return sf::Color(107, 76, 42);
        if (type == "Stone")  return sf::Color(120, 120, 120);
        if (type == "Wood")   return sf::Color(139, 105, 20);
        if (type == "Sand")   return sf::Color(212, 180, 131);
        return sf::Color(100, 100, 100);
    }

    /**
     * @brief Returns the highlight color for the top of a block.
     *
     * Currently only grass uses a distinct top color.
     *
     * @param type Name of the block type
     * @return Highlight color for the block top
     */
    sf::Color blockTopColor(const std::string& type) {
        if (type == "Grass") return sf::Color(74, 124, 63);
        return blockColor(type);
    }

    /**
     * @brief Draws a text string to the window.
     *
     * If no font is loaded, this function does nothing.
     *
     * @param str Text content to draw
     * @param x X-coordinate of the text position
     * @param y Y-coordinate of the text position
     * @param size Character size in pixels
     * @param color Fill color of the text
     */
    void drawText(const std::string& str, float x, float y,
                  unsigned size = 14, sf::Color color = sf::Color::White) {
        if (!fontLoaded) return;
        sf::Text text(str, font, size);
        text.setPosition(x, y);
        text.setFillColor(color);
        window.draw(text);
    }

    /**
     * @brief Draws a progress/status bar.
     *
     * @param x X-coordinate of the bar
     * @param y Y-coordinate of the bar
     * @param w Width of the bar
     * @param h Height of the bar
     * @param value Current value
     * @param maxVal Maximum value
     * @param fill Fill color of the foreground bar
     * @param bg Background color of the bar
     */
    void drawBar(float x, float y, float w, float h,
                 float value, float maxVal, sf::Color fill,
                 sf::Color bg = sf::Color(40, 40, 40)) {
        sf::RectangleShape back({ w, h });
        back.setPosition(x, y);
        back.setFillColor(bg);
        window.draw(back);

        float pct = (maxVal > 0) ? value / maxVal : 0.f;
        sf::RectangleShape bar({ w * pct, h });
        bar.setPosition(x, y);
        bar.setFillColor(fill);
        window.draw(bar);
    }

public:
    /**
     * @brief Constructs a Renderer and attempts to load a font.
     *
     * The constructor tries several common system font paths across
     * Linux, Windows, and macOS.
     *
     * @param win Reference to the SFML render window
     */
    explicit Renderer(sf::RenderWindow& win) : window(win) {
        for (auto& path : {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
            "C:/Windows/Fonts/consola.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "/System/Library/Fonts/Menlo.ttc"
        }) {
            if (font.loadFromFile(path)) { fontLoaded = true; break; }
        }
    }

    /**
     * @brief Draws the background of the scene.
     */
    void drawBackground() {
        window.clear(sf::Color(135, 206, 235));
    }

    /**
     * @brief Draws the world block map.
     *
     * Renders visible tiles based on the camera offset.
     *
     * @param world Reference to the world being rendered
     * @param camX Horizontal camera offset
     */
    void drawWorld(const World& world, float camX) {
        const auto& blocks = world.getBlockMap();
        for (int row = 0; row < (int)blocks.size(); ++row) {
            for (int col = 0; col < (int)blocks[row].size(); ++col) {
                const std::string& type = blocks[row][col];
                if (type.empty()) continue;

                float sx = col * TILE - camX;
                float sy = row * TILE;
                if (sx < -TILE || sx > window.getSize().x + TILE) continue;

                sf::RectangleShape tile({ (float)TILE - 1, (float)TILE - 1 });
                tile.setPosition(sx, sy);
                tile.setFillColor(blockColor(type));
                tile.setOutlineThickness(1.f);
                tile.setOutlineColor(sf::Color(0, 0, 0, 60));
                window.draw(tile);

                if (type == "Grass") {
                    sf::RectangleShape top({ (float)TILE - 1, 5.f });
                    top.setPosition(sx, sy);
                    top.setFillColor(blockTopColor(type));
                    window.draw(top);
                }

                drawText(type.substr(0, 2), sx + 4, sy + TILE - 16, 10, sf::Color(0,0,0,100));
            }
        }
    }

    /**
     * @brief Draws all living world entities.
     *
     * Includes animals and zombies, along with their health bars
     * and labels.
     *
     * @param world Reference to the world being rendered
     * @param camX Horizontal camera offset
     */
    void drawEntities(const World& world, float camX) {
        for (const auto& animal : world.getAnimals()) {
            if (!animal->isAlive()) continue;
            float sx = animal->getX() - camX;

            sf::RectangleShape body({ 32.f, 24.f });
            body.setPosition(sx + 4, animal->getY() + 8);
            body.setFillColor(sf::Color(244, 160, 160));
            body.setOutlineThickness(1.f);
            body.setOutlineColor(sf::Color(180, 80, 80));
            window.draw(body);

            drawBar(sx, animal->getY() - 8, (float)TILE, 4.f,
                    (float)animal->getHp(), (float)animal->getHealth().getMaxHp(),
                    sf::Color(220, 50, 50));

            drawText("PIG", sx + 4, animal->getY() + 10, 10, sf::Color::White);
        }

        for (const auto& zombie : world.getZombies()) {
            if (!zombie->isAlive()) continue;
            float sx = zombie->getX() - camX;

            sf::RectangleShape body({ 28.f, 36.f });
            body.setPosition(sx + 6, zombie->getY() + 4);
            body.setFillColor(sf::Color(90, 138, 74));
            body.setOutlineThickness(1.f);
            body.setOutlineColor(sf::Color(40, 80, 30));
            window.draw(body);

            drawBar(sx, zombie->getY() - 8, (float)TILE, 4.f,
                    (float)zombie->getHp(), (float)zombie->getHealth().getMaxHp(),
                    sf::Color(50, 220, 50));

            drawText("ZMB", sx + 4, zombie->getY() + 12, 10, sf::Color::White);
        }
    }

    /**
     * @brief Draws the player character.
     *
     * @param player Reference to the player object
     * @param camX Horizontal camera offset
     */
    void drawPlayer(const Player& player, float camX) {
        float sx = player.getPositionX() - camX;
        float sy = player.getPositionY();

        sf::CircleShape shadow(14.f);
        shadow.setPosition(sx + 6, sy + 34);
        shadow.setFillColor(sf::Color(0, 0, 0, 50));
        shadow.setScale(1.f, 0.4f);
        window.draw(shadow);

        sf::RectangleShape body({ 28.f, 20.f });
        body.setPosition(sx + 6, sy + 18);
        body.setFillColor(sf::Color(74, 144, 217));
        body.setOutlineThickness(1.f);
        body.setOutlineColor(sf::Color(44, 94, 140));
        window.draw(body);

        sf::RectangleShape head({ 26.f, 24.f });
        head.setPosition(sx + 7, sy);
        head.setFillColor(sf::Color(244, 200, 122));
        head.setOutlineThickness(1.f);
        head.setOutlineColor(sf::Color(200, 150, 80));
        window.draw(head);

        sf::RectangleShape eyeL({ 5.f, 5.f }), eyeR({ 5.f, 5.f });
        eyeL.setPosition(sx + 11, sy + 8);
        eyeR.setPosition(sx + 22, sy + 8);
        eyeL.setFillColor(sf::Color(40, 40, 80));
        eyeR.setFillColor(sf::Color(40, 40, 80));
        window.draw(eyeL);
        window.draw(eyeR);
    }

    /**
     * @brief Draws a mining progress overlay on a block.
     *
     * @param row Row index of the block
     * @param col Column index of the block
     * @param progress Mining progress in the range [0, 1]
     * @param camX Horizontal camera offset
     */
    void drawMineOverlay(int row, int col, float progress, float camX) {
        float sx = col * TILE - camX;
        float sy = row * TILE;

        sf::RectangleShape overlay({ (float)TILE, (float)TILE });
        overlay.setPosition(sx, sy);
        overlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(180 * progress)));
        window.draw(overlay);

        drawBar(sx + 4, sy + TILE - 10, TILE - 8, 6,
                progress, 1.f,
                sf::Color(255, 200, 0), sf::Color(40, 40, 40));
    }

    /**
     * @brief Draws the game HUD.
     *
     * The HUD includes player stats, hotbar, action hints,
     * and an event log.
     *
     * @param player Reference to the player
     * @param log List of recent event log messages
     * @param selectedSlot Currently selected hotbar slot
     */
    void drawHUD(Player& player, const std::vector<std::string>& log,
                 int selectedSlot) {
        float winW = (float)window.getSize().x;
        float winH = (float)window.getSize().y;

        sf::RectangleShape panel({ 160.f, 80.f });
        panel.setPosition(8, 8);
        panel.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(panel);

        drawText("HP:     " + std::to_string(player.getHealth()), 14, 14, 13);
        drawBar(14, 30, 140, 10, (float)player.getHealth(), 100.f,
                sf::Color(220, 50, 50), sf::Color(40, 40, 40));

        drawText("HUNGER: " + std::to_string(player.getHungerLevel()), 14, 44, 13);
        drawBar(14, 46, 140, 10, (float)player.getHungerLevel(), 100.f,
                sf::Color(230, 130, 30), sf::Color(40, 40, 40));

        drawText("SLEEP:  " + std::to_string(player.getSleepLevel()), 14, 60, 13);
        drawBar(14, 72, 140, 10, (float)player.getSleepLevel(), 100.f,
                sf::Color(100, 160, 230), sf::Color(40, 40, 40));

        const int SLOTS = 9;
        const int SZ    = 44;
        float hotbarX = (winW - SLOTS * (SZ + 2)) / 2.f;
        float hotbarY = winH - SZ - 12;

        for (int i = 0; i < SLOTS; ++i) {
            float sx = hotbarX + i * (SZ + 2);
            bool sel = (i == selectedSlot);

            sf::RectangleShape slot({ (float)SZ, (float)SZ });
            slot.setPosition(sx, hotbarY);
            slot.setFillColor(sel ? sf::Color(80, 160, 80, 200) : sf::Color(30, 30, 30, 200));
            slot.setOutlineThickness(sel ? 2.f : 1.f);
            slot.setOutlineColor(sel ? sf::Color(140, 255, 140) : sf::Color(80, 80, 80));
            window.draw(slot);

            auto item = player.getInventory().getSlot(i);
            if (item) {
                sf::RectangleShape icon({ 26.f, 26.f });
                icon.setPosition(sx + 9, hotbarY + 6);
                icon.setFillColor(item->getType() == ItemType::FOOD
                    ? sf::Color(200, 80, 80)
                    : sf::Color(120, 90, 40));
                icon.setOutlineThickness(1.f);
                icon.setOutlineColor(sf::Color(0,0,0,80));
                window.draw(icon);

                drawText(std::to_string(item->getQuantity()), sx + SZ - 14, hotbarY + SZ - 16, 11, sf::Color::Yellow);
                drawText(item->getName().substr(0, 2), sx + 11, hotbarY + 14, 11);
            }

            drawText(std::to_string(i + 1), sx + 2, hotbarY + 2, 9, sf::Color(150, 150, 150));
        }

        sf::RectangleShape hints({ 220.f, 80.f });
        hints.setPosition(winW - 228, winH - 90);
        hints.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(hints);

        drawText("[A/D] Move   [Space] Jump", winW - 224, winH - 86, 11);
        drawText("[LClick] Mine  [RClick] Place", winW - 224, winH - 72, 11);
        drawText("[E] Eat   [F] Attack", winW - 224, winH - 58, 11);
        drawText("[F5] Save  [F9] Load", winW - 224, winH - 44, 11);
        drawText("[1-9] Hotbar slot", winW - 224, winH - 30, 11);

        sf::RectangleShape logPanel({ 260.f, (float)(log.size() * 16 + 12) });
        logPanel.setPosition(winW - 268, 8);
        logPanel.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(logPanel);

        for (int i = 0; i < (int)log.size(); ++i) {
            sf::Color c = (i == (int)log.size() - 1) ? sf::Color::White : sf::Color(160, 160, 160);
            drawText(log[i], winW - 264, 14 + i * 16, 11, c);
        }
    }
};