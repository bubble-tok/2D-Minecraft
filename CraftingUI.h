/**
 * @file CraftingUI.h
 * @brief This header declares CraftingUI, the graphical crafting panel overlay.
 *
 * @author Group 46
 */
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "CraftingSystem.h"
#include "Inventory.h"

/**
 * @class CraftingUI
 * @brief This class renders and handles input for the in-game crafting panel.
 *
 * Opened and closed with the Tab key. The panel has two sub-panels:
 * - Left: scrollable recipe list with a green/red craftability indicator dot.
 * - Right: selected recipe detail with ingredient icons, owned/needed counts,
 *          station badges, output preview, and a CRAFT button.
 *
 * All geometry is defined as constexpr class members so the layout is
 * computed at compile time without any runtime layout passes.
 *
 * @author Group 46
 */
class CraftingUI {
public:
    bool isOpen = false; ///< True while the panel is visible.

    /**
     * @brief Constructs the CraftingUI.
     * @param font       SFML font to use for all text rendering.
     * @param fontLoaded True if the font was loaded successfully.
     */
    CraftingUI(sf::Font& font, bool fontLoaded);

    /// Opens the crafting panel.
    void open();

    /// Closes the crafting panel.
    void close();

    /// Toggles the panel open/closed.
    void toggle();

    /**
     * @brief Handles a left-click at the given screen position.
     *
     * Routes clicks to: recipe list rows (selects), scroll arrows (scrolls),
     * close button (closes), or CRAFT button (returns true to trigger craft).
     *
     * @param mouse   Screen-space mouse position.
     * @param recipes Full recipe list from CraftingSystem.
     * @return True if the CRAFT button was clicked; false otherwise.
     */
    bool handleClick(sf::Vector2i mouse, const std::vector<Recipe>& recipes);

    /**
     * @brief Scrolls the recipe list by the given mouse-wheel delta.
     * @param delta   Positive = scroll up, negative = scroll down.
     * @param recipes Full recipe list (used to clamp the scroll offset).
     */
    void handleScroll(float delta, const std::vector<Recipe>& recipes);

    /**
     * @brief Returns the output name of the currently selected recipe.
     * @param recipes Full recipe list from CraftingSystem.
     * @return Output name string, or "" if the list is empty.
     */
    std::string getSelectedRecipeName(const std::vector<Recipe>& recipes) const;

    /**
     * @brief Draws the full crafting panel to the window.
     *
     * Should be called in screen-space (no world camera offset).
     *
     * @param w          SFML render window.
     * @param recipes    Full recipe list for display.
     * @param inv        Player inventory (used for ingredient availability checks).
     * @param hasTable   True if a CraftingTable is nearby.
     * @param hasCampfire True if a Campfire is nearby.
     * @param mouse      Current screen-space mouse position (for hover effects).
     */
    void draw(sf::RenderWindow& w,
              const std::vector<Recipe>& recipes,
              const Inventory& inv,
              bool hasTable, bool hasCampfire,
              sf::Vector2i mouse);

private:
    sf::Font& font;       ///< Reference to the shared game font.
    bool      fontLoaded; ///< False when font is unavailable; text calls are skipped.
    int       selectedRecipe = 0; ///< Index of the highlighted recipe in the list.
    int       scrollOffset   = 0; ///< First visible recipe index (for list scrolling).

    // ── Panel layout constants ────────────────────────────────────────────────
    static constexpr float PW      = 700.f;              ///< Panel width.
    static constexpr float PH      = 420.f;              ///< Panel height.
    static constexpr float PX      = (1024 - PW) / 2.f; ///< Panel left edge (centred).
    static constexpr float PY      = (600  - PH) / 2.f; ///< Panel top edge (centred).
    static constexpr float LIST_W  = 220.f;              ///< Recipe list column width.
    static constexpr float LIST_X  = PX + 16.f;         ///< Recipe list left edge.
    static constexpr float LIST_Y  = PY + 52.f;         ///< Recipe list top edge.
    static constexpr float LIST_H  = PH - 90.f;         ///< Recipe list height.
    static constexpr int   ROW_H   = 38;                 ///< Height of one recipe row.
    static constexpr int   VISIBLE = (int)(LIST_H / ROW_H); ///< Max visible rows.
    static constexpr float DETAIL_X = PX + LIST_W + 32.f;   ///< Detail panel left edge.
    static constexpr float DETAIL_Y = PY + 52.f;             ///< Detail panel top edge.
    static constexpr float DETAIL_W = PW - LIST_W - 48.f;   ///< Detail panel width.
    static constexpr float DETAIL_H = PH - 110.f;            ///< Detail panel height.

    /**
     * @brief Returns the display colour for an item of the given name and type.
     * @param name Item name (checked first for special cases like GoldenApple).
     * @param type Item type (used for category-level colour fallback).
     * @return SFML Color for the item icon background.
     */
    sf::Color itemColor(const std::string& name, ItemType type) const;

    /**
     * @brief Draws a text string at the given position.
     * @param w   Render window.
     * @param s   String to draw.
     * @param x   Screen X position.
     * @param y   Screen Y position.
     * @param sz  Character size in pixels.
     * @param col Text colour.
     */
    void drawText(sf::RenderWindow& w, const std::string& s,
                  float x, float y, unsigned sz, sf::Color col) const;

    /**
     * @brief Draws a filled rectangle with an optional outline.
     * @param w     Render window.
     * @param x     Left edge.
     * @param y     Top edge.
     * @param wd    Width.
     * @param ht    Height.
     * @param fill  Fill colour.
     * @param outline Outline colour (transparent = no outline).
     * @param thick   Outline thickness in pixels.
     */
    void drawRect(sf::RenderWindow& w, float x, float y, float wd, float ht,
                  sf::Color fill,
                  sf::Color outline = sf::Color::Transparent,
                  float thick = 0.f) const;

    /**
     * @brief Draws the scrollable recipe list in the left panel.
     * @param w          Render window.
     * @param recipes    Full recipe list.
     * @param inv        Player inventory for craftability check.
     * @param hasTable   True if CraftingTable is nearby.
     * @param hasCampfire True if Campfire is nearby.
     * @param mouse      Mouse position for hover highlighting.
     */
    void drawRecipeList(sf::RenderWindow& w, const std::vector<Recipe>& recipes,
                        const Inventory& inv, bool hasTable, bool hasCampfire,
                        sf::Vector2i mouse) const;

    /**
     * @brief Draws the selected recipe detail view in the right panel.
     * @param w          Render window.
     * @param sel        The recipe to display.
     * @param inv        Player inventory for ingredient owned/needed counts.
     * @param hasTable   True if CraftingTable is nearby.
     * @param hasCampfire True if Campfire is nearby.
     * @param mouse      Mouse position for CRAFT button hover.
     */
    void drawRecipeDetail(sf::RenderWindow& w, const Recipe& sel,
                          const Inventory& inv, bool hasTable, bool hasCampfire,
                          sf::Vector2i mouse) const;
};
