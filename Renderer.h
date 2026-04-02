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
 * All world-space draw methods (drawWorld, drawEntities, drawPlayer,
 * drawMineOverlay) use raw world coordinates. The caller is responsible
 * for setting an appropriate SFML view before invoking these methods so
 * that the view transform handles camera offset for both X and Y.
 *
 * drawBackground() and drawHUD() must be called while a screen-space
 * view (origin at 0,0) is active.
 */
class Renderer {
private:
    sf::RenderWindow& window;
    sf::Font          font;
    bool              fontLoaded = false;

    static const int TILE = 40;

    sf::Color blockColor(const std::string& type) {
        if (type == "Grass")  return sf::Color(106, 127, 60);
        if (type == "Dirt")   return sf::Color(107, 76, 42);
        if (type == "Stone")  return sf::Color(120, 120, 120);
        if (type == "Wood")   return sf::Color(139, 105, 20);
        if (type == "Sand")   return sf::Color(212, 180, 131);
        if (type == "Gold")   return sf::Color(255, 200, 0);
        return sf::Color(100, 100, 100);
    }

    sf::Color blockTopColor(const std::string& type) {
        if (type == "Grass") return sf::Color(74, 124, 63);
        return blockColor(type);
    }

    void drawText(const std::string& str, float x, float y,
                  unsigned size = 14, sf::Color color = sf::Color::White) {
        if (!fontLoaded) return;
        sf::Text text(str, font, size);
        text.setPosition(x, y);
        text.setFillColor(color);
        window.draw(text);
    }

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
     * @brief Clears the window with the sky colour.
     * Call while the world-space view is active.
     */
    void drawBackground() {
        window.clear(sf::Color(135, 206, 235));
    }

    /**
     * @brief Draws all solid tiles in the world using world coordinates.
     *
     * Frustum-culls tiles that are outside the current view viewport.
     * The SFML view (set by the caller) provides the camera transform —
     * no manual camX subtraction is needed here.
     *
     * @param world World to draw
     */
    void drawWorld(const World& world) {
        // Determine visible column range from the current view
        sf::View view = window.getView();
        float viewLeft  = view.getCenter().x - view.getSize().x / 2.f;
        float viewRight = view.getCenter().x + view.getSize().x / 2.f;
        int colMin = std::max(0, (int)(viewLeft  / TILE) - 1);
        int colMax = std::min((int)world.getCols(), (int)(viewRight / TILE) + 2);

        const auto& blocks = world.getBlockMap();
        for (int row = 0; row < (int)blocks.size(); ++row) {
            for (int col = colMin; col < colMax; ++col) {
                const std::string& type = blocks[row][col];
                if (type.empty()) continue;

                float wx = (float)(col * TILE);
                float wy = (float)(row * TILE);

                sf::RectangleShape tile({ (float)TILE - 1, (float)TILE - 1 });
                tile.setPosition(wx, wy);
                tile.setFillColor(blockColor(type));
                tile.setOutlineThickness(1.f);
                tile.setOutlineColor(sf::Color(0, 0, 0, 60));
                window.draw(tile);

                if (type == "Grass") {
                    sf::RectangleShape top({ (float)TILE - 1, 5.f });
                    top.setPosition(wx, wy);
                    top.setFillColor(blockTopColor(type));
                    window.draw(top);
                }

                drawText(type.substr(0, 2), wx + 4, wy + TILE - 16, 10,
                         sf::Color(0, 0, 0, 100));
            }
        }
    }

    /**
     * @brief Draws all living entities (animals and zombies) using world coordinates.
     * @param world World whose entities are drawn
     */
    void drawEntities(const World& world) {
        for (const auto& animal : world.getAnimals()) {
            if (!animal->isAlive()) continue;
            float wx = animal->getX();
            float wy = animal->getY();

            sf::RectangleShape body({ 32.f, 24.f });
            body.setPosition(wx + 4, wy + 8);
            body.setFillColor(sf::Color(244, 160, 160));
            body.setOutlineThickness(1.f);
            body.setOutlineColor(sf::Color(180, 80, 80));
            window.draw(body);

            drawBar(wx, wy - 8, (float)TILE, 4.f,
                    (float)animal->getHp(),
                    (float)animal->getHealth().getMaxHp(),
                    sf::Color(220, 50, 50));

            drawText("PIG", wx + 4, wy + 10, 10, sf::Color::White);
        }

        for (const auto& zombie : world.getZombies()) {
            if (!zombie->isAlive()) continue;
            float wx = zombie->getX();
            float wy = zombie->getY();

            sf::RectangleShape body({ 28.f, 36.f });
            body.setPosition(wx + 6, wy + 4);
            body.setFillColor(sf::Color(90, 138, 74));
            body.setOutlineThickness(1.f);
            body.setOutlineColor(sf::Color(40, 80, 30));
            window.draw(body);

            drawBar(wx, wy - 8, (float)TILE, 4.f,
                    (float)zombie->getHp(),
                    (float)zombie->getHealth().getMaxHp(),
                    sf::Color(50, 220, 50));

            drawText("ZMB", wx + 4, wy + 12, 10, sf::Color::White);
        }
    }

    /**
     * @brief Draws the player character using world coordinates.
     * @param player Player to draw
     */
    void drawPlayer(const Player& player) {
        float wx = player.getPositionX();
        float wy = player.getPositionY();

        sf::CircleShape shadow(14.f);
        shadow.setPosition(wx + 6, wy + 34);
        shadow.setFillColor(sf::Color(0, 0, 0, 50));
        shadow.setScale(1.f, 0.4f);
        window.draw(shadow);

        sf::RectangleShape body({ 28.f, 20.f });
        body.setPosition(wx + 6, wy + 18);
        body.setFillColor(sf::Color(74, 144, 217));
        body.setOutlineThickness(1.f);
        body.setOutlineColor(sf::Color(44, 94, 140));
        window.draw(body);

        sf::RectangleShape head({ 26.f, 24.f });
        head.setPosition(wx + 7, wy);
        head.setFillColor(sf::Color(244, 200, 122));
        head.setOutlineThickness(1.f);
        head.setOutlineColor(sf::Color(200, 150, 80));
        window.draw(head);

        sf::RectangleShape eyeL({ 5.f, 5.f }), eyeR({ 5.f, 5.f });
        eyeL.setPosition(wx + 11, wy + 8);
        eyeR.setPosition(wx + 22, wy + 8);
        eyeL.setFillColor(sf::Color(40, 40, 80));
        eyeR.setFillColor(sf::Color(40, 40, 80));
        window.draw(eyeL);
        window.draw(eyeR);
    }

    /**
     * @brief Draws a darkening overlay and progress bar on the block being mined.
     *
     * Uses world coordinates — caller must have the world-space view active.
     *
     * @param row      Tile row of the block being mined
     * @param col      Tile column of the block being mined
     * @param progress Overall mining progress [0, 1] across all durability hits
     */
    void drawMineOverlay(int row, int col, float progress) {
        float wx = (float)(col * TILE);
        float wy = (float)(row * TILE);

        sf::RectangleShape overlay({ (float)TILE, (float)TILE });
        overlay.setPosition(wx, wy);
        overlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(180 * progress)));
        window.draw(overlay);

        drawBar(wx + 4, wy + TILE - 10, TILE - 8, 6,
                progress, 1.f,
                sf::Color(255, 200, 0), sf::Color(40, 40, 40));
    }

    /**
     * @brief Draws the full game HUD in screen space.
     *
     * Must be called while a screen-space view (origin 0,0) is active.
     *
     * @param player       The player whose stats are displayed
     * @param log          Recent event log messages
     * @param selectedSlot Currently selected hotbar slot index
     */
    void drawHUD(Player& player, const std::vector<std::string>& log,
                 int selectedSlot) {
        float winW = (float)window.getSize().x;
        float winH = (float)window.getSize().y;

        // ── Stats panel (top-left) ────────────────────────────────────────────
        sf::RectangleShape panel({ 160.f, 115.f });
        panel.setPosition(8, 8);
        panel.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(panel);

        drawText("HP:     " + std::to_string(player.getHealth()), 14, 10, 13);
        drawBar(14, 26, 140, 8, (float)player.getHealth(), 100.f,
                sf::Color(220, 50, 50), sf::Color(40, 40, 40));

        drawText("HUNGER: " + std::to_string(player.getHungerLevel()), 14, 44, 13);
        drawBar(14, 60, 140, 8, (float)player.getHungerLevel(), 100.f,
                sf::Color(230, 130, 30), sf::Color(40, 40, 40));

        drawText("SLEEP:  " + std::to_string(player.getSleepLevel()), 14, 78, 13);
        drawBar(14, 94, 140, 8, (float)player.getSleepLevel(), 100.f,
                sf::Color(100, 160, 230), sf::Color(40, 40, 40));

        // ── Hotbar (bottom-center) ────────────────────────────────────────────
        const int SLOTS = 9;
        const int SZ    = 44;
        float hotbarX = (winW - SLOTS * (SZ + 2)) / 2.f;
        float hotbarY = winH - SZ - 12;

        for (int i = 0; i < SLOTS; ++i) {
            float sx = hotbarX + i * (SZ + 2);
            bool  sel = (i == selectedSlot);

            sf::RectangleShape slot({ (float)SZ, (float)SZ });
            slot.setPosition(sx, hotbarY);
            slot.setFillColor(sel ? sf::Color(80, 160, 80, 200) : sf::Color(30, 30, 30, 200));
            slot.setOutlineThickness(sel ? 2.f : 1.f);
            slot.setOutlineColor(sel ? sf::Color(140, 255, 140) : sf::Color(80, 80, 80));
            window.draw(slot);

            auto item = player.getInventory().getSlot(i);
            if (item) {
                sf::Color iconColor = sf::Color(120, 90, 40);
                if (item->getType() == ItemType::FOOD)  iconColor = sf::Color(200, 80, 80);
                if (item->getName() == "Gold")          iconColor = sf::Color(255, 200, 0);
                if (item->getName() == "GoldenApple")   iconColor = sf::Color(255, 215, 0);

                sf::RectangleShape icon({ 26.f, 26.f });
                icon.setPosition(sx + 9, hotbarY + 6);
                icon.setFillColor(iconColor);
                icon.setOutlineThickness(1.f);
                icon.setOutlineColor(sf::Color(0, 0, 0, 80));
                window.draw(icon);

                drawText(std::to_string(item->getQuantity()),
                         sx + SZ - 14, hotbarY + SZ - 16, 11, sf::Color::Yellow);
                drawText(item->getName().substr(0, 2), sx + 11, hotbarY + 14, 11);
            }

            drawText(std::to_string(i + 1), sx + 2, hotbarY + 2, 9,
                     sf::Color(150, 150, 150));
        }

        // ── Action hints (bottom-right) ───────────────────────────────────────
        sf::RectangleShape hints({ 240.f, 106.f });
        hints.setPosition(winW - 248, winH - 114);
        hints.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(hints);

        drawText("[A/D] Move   [Space] Jump",       winW - 244, winH - 110, 11);
        drawText("[LClick] Mine  [RClick] Place",    winW - 244, winH - 96,  11);
        drawText("[E] Eat   [F] Attack   [Z] Sleep", winW - 244, winH - 82,  11);
        drawText("[F5] Save   [F9] Load",             winW - 244, winH - 68,  11);
        drawText("[C] Craft CookedMeat",              winW - 244, winH - 54,  11);
        drawText("[G] Craft GoldenApple -> WIN!",     winW - 244, winH - 40,  11,
                 sf::Color(255, 215, 0));

        // ── Event log (top-right) ─────────────────────────────────────────────
        sf::RectangleShape logPanel({ 260.f, (float)(log.size() * 16 + 12) });
        logPanel.setPosition(winW - 268, 8);
        logPanel.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(logPanel);

        for (int i = 0; i < (int)log.size(); ++i) {
            sf::Color c = (i == (int)log.size() - 1)
                ? sf::Color::White : sf::Color(160, 160, 160);
            drawText(log[i], winW - 264, 14 + i * 16, 11, c);
        }
    }
};
