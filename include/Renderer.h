/**
 * @file Renderer.h
 * @brief Declares the Renderer class for all game visual output.
 *
 * @author Group 46
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "World.h"
#include "DayCycle.h"

/**
 * @class Renderer
 * @brief Handles all rendering: background, world tiles, entities, player, and HUD.
 *
 * Renderer wraps an sf::RenderWindow and provides high-level draw methods
 * called once per frame from main.cpp's runGame() loop. World-space methods
 * assume a world-space SFML View has been set by the caller. Screen-space
 * methods (drawBackground, drawHUD) set or assume screen-space views internally.
 *
 * @author Group 46
 */
class Renderer {
public:
    /**
     * @brief Constructs the Renderer and attempts to load a monospace font.
     *
     * Tries several common system font paths. If none succeed, fontLoaded
     * remains false and all text drawing is silently skipped.
     *
     * @param win Reference to the main SFML render window.
     */
    explicit Renderer(sf::RenderWindow& win);

    /**
     * @brief Draws the sky, stars, sun/moon, and night darkness overlay.
     *
     * Must be called first each frame (clears the window). Uses screen-space
     * coordinates; camX/camY are used only for star parallax scrolling.
     *
     * @param cycle The current DayCycle for sky colour and celestial positions.
     * @param camX  Camera X offset in pixels (for star parallax).
     * @param camY  Camera Y offset in pixels (unused currently).
     */
    void drawBackground(const DayCycle& cycle, float camX, float camY);

    /**
     * @brief Draws all visible world tiles within the current camera view.
     *
     * Culls columns outside the view frustum for performance. Delegates
     * to drawWoodTile() and drawLeavesTile() for organic-looking terrain.
     *
     * @param world The World to render.
     */
    void drawWorld(const World& world);

    /**
     * @brief Draws all living Animal and Zombie entities with health bars.
     * @param world The World whose entity lists to render.
     */
    void drawEntities(const World& world);

    /**
     * @brief Draws the player character sprite with a drop shadow.
     * @param player The Player to render.
     */
    void drawPlayer(const Player& player);

    /**
     * @brief Draws a darkening overlay and progress bar over a block being mined.
     * @param row      Tile row of the block.
     * @param col      Tile column of the block.
     * @param progress Overall mining progress [0, 1].
     */
    void drawMineOverlay(int row, int col, float progress);

    /**
     * @brief Draws a coloured outline around the tile under the mouse cursor.
     *
     * White outline = within reach; red outline = out of reach.
     *
     * @param row     Tile row.
     * @param col     Tile column.
     * @param inReach True if the tile is within REACH_DISTANCE of the player.
     */
    void drawTileHighlight(int row, int col, bool inReach);

    /**
     * @brief Draws the full HUD: stats panel, clock, hotbar, hints, and event log.
     *
     * Must be called with a screen-space SFML View active.
     *
     * @param player       The Player for stat values and inventory.
     * @param log          Event log lines (newest at back).
     * @param selectedSlot Currently selected hotbar slot index.
     * @param cycle        DayCycle for the clock display.
     */
    void drawHUD(Player& player, const std::vector<std::string>& log,
                 int selectedSlot, const DayCycle& cycle);

private:
    sf::RenderWindow& window;    ///< Reference to the main render window.
    sf::Font          font;      ///< Loaded monospace font for all text.
    bool              fontLoaded = false; ///< False if no system font was found.

    static const int TILE = 40; ///< Tile size in pixels (mirrors TILE_SIZE constant).

    /**
     * @brief Returns the base fill colour for a named block type.
     * @param type Block type name.
     * @return SFML Color for the block face.
     */
    sf::Color blockColor(const std::string& type) const;

    /**
     * @brief Returns the top-face accent colour for a block type (used by Grass).
     * @param type Block type name.
     * @return SFML Color for the top strip.
     */
    sf::Color blockTopColor(const std::string& type) const;

    /**
     * @brief Draws a text string at the given screen position.
     * @param str   String to draw.
     * @param x     Screen X.
     * @param y     Screen Y.
     * @param size  Character size. Defaults to 14.
     * @param color Text colour. Defaults to white.
     */
    void drawText(const std::string& str, float x, float y,
                  unsigned size = 14, sf::Color color = sf::Color::White);

    /**
     * @brief Draws a filled progress bar with a background.
     * @param x      Left edge.
     * @param y      Top edge.
     * @param w      Total width.
     * @param h      Height.
     * @param value  Current value.
     * @param maxVal Maximum value (determines fill fraction).
     * @param fill   Fill colour.
     * @param bg     Background colour. Defaults to dark grey.
     */
    void drawBar(float x, float y, float w, float h,
                 float value, float maxVal, sf::Color fill,
                 sf::Color bg = sf::Color(40, 40, 40));

    /**
     * @brief Draws a Wood trunk tile with vertical grain lines.
     * @param wx World X pixel position of the tile's top-left corner.
     * @param wy World Y pixel position of the tile's top-left corner.
     */
    void drawWoodTile(float wx, float wy);

    /**
     * @brief Draws a Leaves canopy tile using overlapping circle blobs.
     * @param wx World X pixel position of the tile's top-left corner.
     * @param wy World Y pixel position of the tile's top-left corner.
     */
    void drawLeavesTile(float wx, float wy);
};
