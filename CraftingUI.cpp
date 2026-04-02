/**
 * @file CraftingUI.cpp
 * @brief The CraftingUI.cpp file implements the graphical crafting panel rendering and input handling.
 *
 * The CraftingUI.cpp file defines how the CraftingUI class opens, closes,
 * processes input, and draws the crafting interface, including the recipe list,
 * recipe details, crafting requirements, and result display.
 *
 * @author Group 46
 */
#include "CraftingUI.h"
#include <algorithm>

/**
 * @brief The constructor initializes the CraftingUI object.
 *
 * The constructor stores the font reference and records whether the font
 * loaded successfully so the CraftingUI object can safely render text.
 *
 * @param f The f parameter represents the font used by the crafting UI.
 * @param loaded The loaded parameter indicates whether the font was loaded successfully.
 */
CraftingUI::CraftingUI(sf::Font& f, bool loaded) : font(f), fontLoaded(loaded) {}

/**
 * @brief The open function sets the crafting UI to the open state.
 */
void CraftingUI::open() { isOpen = true; }

/**
 * @brief The close function sets the crafting UI to the closed state.
 */
void CraftingUI::close() { isOpen = false; }

/**
 * @brief The toggle function switches the crafting UI between the open and closed states.
 */
void CraftingUI::toggle() { isOpen = !isOpen; }

// ── Helpers ───────────────────────────────────────────────────────────────────

/**
 * @brief The itemColor function determines the display color for an item in the crafting menu.
 *
 * The itemColor function selects a color based on the item name and item type
 * so the crafting UI can visually distinguish food, tools, and block materials.
 *
 * @param name The name parameter represents the item name.
 * @param type The type parameter represents the ItemType of the item.
 * @return The function returns the sf::Color used to display the item.
 */
sf::Color CraftingUI::itemColor(const std::string& name, ItemType type) const {
    if (name == "GoldenApple")                          return sf::Color(255,215,  0);
    if (name == "Gold")                                 return sf::Color(255,200,  0);
    if (type == ItemType::FOOD)                         return sf::Color(220, 80, 80);
    if (type == ItemType::TOOL) {
        if (name.find("Sword")   != std::string::npos) return sf::Color(180,180,220);
        if (name.find("Pickaxe") != std::string::npos) return sf::Color(160,120, 60);
        if (name == "Bed")                             return sf::Color(180, 80,120);
        if (name == "Campfire")                        return sf::Color(220, 90, 20);
        return sf::Color(140,180,140);
    }
    if (name == "Wood" || name == "WoodPlank")  return sf::Color(160,110,40);
    if (name == "Stone"|| name == "StoneBrick") return sf::Color(130,130,130);
    return sf::Color(120,90,40);
}

/**
 * @brief The drawText function renders text onto the screen.
 *
 * The drawText function draws text only when the font has loaded successfully.
 *
 * @param w The w parameter represents the render window.
 * @param s The s parameter represents the string to draw.
 * @param x The x parameter represents the horizontal position.
 * @param y The y parameter represents the vertical position.
 * @param sz The sz parameter represents the font size.
 * @param col The col parameter represents the text color.
 */
void CraftingUI::drawText(sf::RenderWindow& w, const std::string& s,
                          float x, float y, unsigned sz, sf::Color col) const {
    if (!fontLoaded) return;
    sf::Text t(s, font, sz);
    t.setPosition(x, y);
    t.setFillColor(col);
    w.draw(t);
}

/**
 * @brief The drawRect function draws a rectangle with optional outline styling.
 *
 * @param w The w parameter represents the render window.
 * @param x The x parameter represents the horizontal position.
 * @param y The y parameter represents the vertical position.
 * @param wd The wd parameter represents the width.
 * @param ht The ht parameter represents the height.
 * @param fill The fill parameter represents the fill color.
 * @param outline The outline parameter represents the outline color.
 * @param thick The thick parameter represents the outline thickness.
 */
void CraftingUI::drawRect(sf::RenderWindow& w, float x, float y,
                          float wd, float ht,
                          sf::Color fill, sf::Color outline, float thick) const {
    sf::RectangleShape r({wd, ht});
    r.setPosition(x, y);
    r.setFillColor(fill);
    if (thick > 0.f) {
        r.setOutlineThickness(thick);
        r.setOutlineColor(outline);
    }
    w.draw(r);
}

// ── Click / scroll ────────────────────────────────────────────────────────────

/**
 * @brief The handleClick function processes mouse clicks inside the crafting UI.
 *
 * The handleClick function updates the selected recipe, the scroll position,
 * the close state, or the craft action depending on where the player clicks.
 *
 * @param mouse The mouse parameter represents the mouse position.
 * @param recipes The recipes parameter represents the list of available recipes.
 * @return The function returns true when the player clicks the craft button.
 */
bool CraftingUI::handleClick(sf::Vector2i mouse, const std::vector<Recipe>& recipes) {
    if (!isOpen) return false;

    // The function checks whether the player clicked a recipe row.
    for (int i = 0; i < VISIBLE; ++i) {
        int idx = scrollOffset + i;
        if (idx >= (int)recipes.size()) break;
        float ry = LIST_Y + i * ROW_H;
        if (mouse.x >= LIST_X && mouse.x <= LIST_X + LIST_W &&
            mouse.y >= ry     && mouse.y <= ry + ROW_H - 2) {
            selectedRecipe = idx;
            return false;
        }
    }

    // The function checks whether the player clicked the scroll arrows.
    float arrowY = LIST_Y + LIST_H + 4.f;
    if (mouse.x >= LIST_X && mouse.x <= LIST_X + 24 &&
        mouse.y >= arrowY && mouse.y <= arrowY + 20)
        scrollOffset = std::max(0, scrollOffset - 1);

    if (mouse.x >= LIST_X + 28 && mouse.x <= LIST_X + 52 &&
        mouse.y >= arrowY      && mouse.y <= arrowY + 20)
        scrollOffset = std::min((int)recipes.size() - VISIBLE, scrollOffset + 1);

    // The function checks whether the player clicked the close button.
    if (mouse.x >= PX + PW - 32 && mouse.x <= PX + PW - 8 &&
        mouse.y >= PY + 8       && mouse.y <= PY + 32) {
        close();
        return false;
    }

    // The function checks whether the player clicked the craft button.
    float btnW2 = 160.f, btnH2 = 38.f;
    float btnX  = DETAIL_X + DETAIL_W / 2.f - btnW2 / 2.f;
    float btnY  = DETAIL_Y + DETAIL_H - btnH2 - 6.f;
    if (mouse.x >= btnX && mouse.x <= btnX + btnW2 &&
        mouse.y >= btnY && mouse.y <= btnY + btnH2)
        return true;

    return false;
}

/**
 * @brief The handleScroll function updates the recipe list scroll offset.
 *
 * The handleScroll function clamps the scroll offset so the visible recipe list
 * always stays within valid bounds.
 *
 * @param delta The delta parameter represents the scroll input value.
 * @param recipes The recipes parameter represents the list of recipes.
 */
void CraftingUI::handleScroll(float delta, const std::vector<Recipe>& recipes) {
    if (!isOpen) return;
    int maxOff = std::max(0, (int)recipes.size() - VISIBLE);
    scrollOffset = std::clamp((int)(scrollOffset - delta), 0, maxOff);
}

/**
 * @brief The getSelectedRecipeName function returns the name of the selected recipe.
 *
 * @param recipes The recipes parameter represents the recipe list.
 * @return The function returns the selected recipe name, or an empty string if no valid selection exists.
 */
std::string CraftingUI::getSelectedRecipeName(const std::vector<Recipe>& recipes) const {
    if (recipes.empty() || selectedRecipe >= (int)recipes.size()) return "";
    return recipes[selectedRecipe].outputName;
}

// ── Drawing ───────────────────────────────────────────────────────────────────

/**
 * @brief The drawRecipeList function renders the recipe list panel.
 *
 * The drawRecipeList function displays the visible recipes, selection state,
 * hover state, crafting availability, and scroll controls.
 *
 * @param w The w parameter represents the render window.
 * @param recipes The recipes parameter represents the recipe list.
 * @param inv The inv parameter represents the player inventory.
 * @param hasTable The hasTable parameter indicates whether a CraftingTable is nearby.
 * @param hasCampfire The hasCampfire parameter indicates whether a Campfire is nearby.
 * @param mouse The mouse parameter represents the mouse position.
 */
void CraftingUI::drawRecipeList(sf::RenderWindow& w,
                                const std::vector<Recipe>& recipes,
                                const Inventory& inv,
                                bool hasTable, bool hasCampfire,
                                sf::Vector2i mouse) const {
    drawRect(w, LIST_X - 4, LIST_Y - 4, LIST_W + 8, LIST_H + 8,
             sf::Color(18,18,28), sf::Color(80,80,120), 1.f);

    for (int i = 0; i < VISIBLE; ++i) {
        int idx = scrollOffset + i;
        if (idx >= (int)recipes.size()) break;
        const Recipe& r = recipes[idx];
        float ry  = LIST_Y + i * ROW_H;
        bool  sel = (idx == selectedRecipe);
        bool  hov = mouse.x >= LIST_X && mouse.x <= LIST_X + LIST_W &&
                    mouse.y >= ry     && mouse.y <= ry + ROW_H - 2;

        drawRect(w, LIST_X, ry, LIST_W, ROW_H - 2,
                 sel ? sf::Color(60,60,100) : hov ? sf::Color(45,45,70)
                                                   : sf::Color(28,28,38));
        drawRect(w, LIST_X + 4, ry + 5, 28, 28,
                 itemColor(r.outputName, r.outputType),
                 sf::Color(255,255,255,40), 1.f);

        bool needsTable    = r.requiresCraftingTable && !hasTable;
        bool needsCampfire = (r.outputName == "CookedMeat") && !hasCampfire;
        sf::Color nameCol  = (needsTable || needsCampfire)
                           ? sf::Color(140,140,140) : sf::Color::White;
        drawText(w, r.outputName, LIST_X + 38, ry + 6, 12, nameCol);

        bool canCraft = true;
        for (auto& [ingr, qty] : r.ingredients)
            if (!inv.hasItem(ingr, qty)) {
                canCraft = false;
                break;
            }

        sf::Color dot = (canCraft && !needsTable && !needsCampfire)
                      ? sf::Color(80,220,80) : sf::Color(180,60,60);
        drawRect(w, LIST_X + LIST_W - 12, ry + 14, 8, 8, dot);
    }

    float arrowY = LIST_Y + LIST_H + 6.f;
    drawRect(w, LIST_X, arrowY, 24, 20, sf::Color(50,50,80), sf::Color(100,100,140), 1.f);
    drawText(w, "^", LIST_X + 7, arrowY + 2, 12, sf::Color::White);
    drawRect(w, LIST_X + 28, arrowY, 24, 20, sf::Color(50,50,80), sf::Color(100,100,140), 1.f);
    drawText(w, "v", LIST_X + 35, arrowY + 2, 12, sf::Color::White);
    drawText(w, "[Tab] to close", LIST_X + 60, arrowY + 4, 10, sf::Color(130,130,130));
}

/**
 * @brief The drawRecipeDetail function renders detailed information for the selected recipe.
 *
 * The drawRecipeDetail function displays crafting requirements, ingredient
 * counts, output details, bonus stats, and the craft button state.
 *
 * @param w The w parameter represents the render window.
 * @param sel The sel parameter represents the selected recipe.
 * @param inv The inv parameter represents the player inventory.
 * @param hasTable The hasTable parameter indicates whether a CraftingTable is nearby.
 * @param hasCampfire The hasCampfire parameter indicates whether a Campfire is nearby.
 * @param mouse The mouse parameter represents the mouse position.
 */
void CraftingUI::drawRecipeDetail(sf::RenderWindow& w, const Recipe& sel,
                                  const Inventory& inv,
                                  bool hasTable, bool hasCampfire,
                                  sf::Vector2i mouse) const {
    drawRect(w, DETAIL_X - 4, DETAIL_Y - 4, DETAIL_W + 8, DETAIL_H + 8,
             sf::Color(18,18,28), sf::Color(80,80,120), 1.f);

    bool needsTable    = sel.requiresCraftingTable && !hasTable;
    bool needsCampfire = (sel.outputName == "CookedMeat") && !hasCampfire;
    bool canCraft = true;
    for (auto& [ingr, qty] : sel.ingredients)
        if (!inv.hasItem(ingr, qty)) {
            canCraft = false;
            break;
        }
    bool craftable = canCraft && !needsTable && !needsCampfire;

    float cursorY = DETAIL_Y + 8.f;
    const float PAD   = DETAIL_X + 8.f;
    const float ICOSZ = 48.f;
    const float ICOSP = 14.f;

    // The function draws the recipe name.
    drawText(w, sel.outputName, PAD, cursorY, 17, sf::Color(255,220,100));
    cursorY += 26.f;

    // The function draws station requirement badges.
    if (sel.requiresCraftingTable) {
        sf::Color bc = hasTable ? sf::Color(50,140,50) : sf::Color(150,50,50);
        drawRect(w, PAD, cursorY, 180, 22, bc, sf::Color(255,255,255,40), 1.f);
        drawText(w, hasTable ? "✓  CraftingTable" : "✗  CraftingTable needed",
                 PAD + 6, cursorY + 5, 11, sf::Color::White);
        cursorY += 28.f;
    }
    if (sel.outputName == "CookedMeat") {
        sf::Color bc = hasCampfire ? sf::Color(50,140,50) : sf::Color(150,50,50);
        drawRect(w, PAD, cursorY, 150, 22, bc, sf::Color(255,255,255,40), 1.f);
        drawText(w, hasCampfire ? "✓  Campfire" : "✗  Campfire needed",
                 PAD + 6, cursorY + 5, 11, sf::Color::White);
        cursorY += 28.f;
    }

    cursorY += 4.f;
    drawText(w, "Ingredients:", PAD, cursorY, 12, sf::Color(160,160,160));
    cursorY += 18.f;

    float iconX   = PAD;
    float rowTopY = cursorY;

    for (auto& [ingr, qty] : sel.ingredients) {
        int owned = 0;
        for (int s = 0; s < inv.getMaxSlots(); ++s) {
            auto it = inv.getSlot(s);
            if (it && it->getName() == ingr) owned += it->getQuantity();
        }
        bool have = owned >= qty;

        drawRect(w, iconX, rowTopY, ICOSZ, ICOSZ,
                 sf::Color(45,45,55), sf::Color(150,150,150,80), 1.f);
        drawRect(w, iconX + 5, rowTopY + 5, ICOSZ - 10, ICOSZ - 10,
                 itemColor(ingr, ItemType::MISC));
        drawText(w, std::to_string(qty) + "x",
                 iconX + ICOSZ - 18, rowTopY + ICOSZ - 16, 10, sf::Color::Yellow);
        drawText(w, ingr, iconX, rowTopY + ICOSZ + 3, 10, sf::Color(200,200,200));
        drawText(w, std::to_string(owned) + " / " + std::to_string(qty),
                 iconX, rowTopY + ICOSZ + 16, 10,
                 have ? sf::Color(80,210,80) : sf::Color(210,80,80));

        iconX += ICOSZ + ICOSP;
        if (iconX + ICOSZ > DETAIL_X + DETAIL_W - 8) {
            iconX = PAD;
            rowTopY += ICOSZ + 34.f;
        }
    }
    cursorY = rowTopY + ICOSZ + 38.f;

    // The function draws the separator line.
    drawRect(w, PAD, cursorY, DETAIL_W - 16, 1, sf::Color(80,80,110));
    cursorY += 10.f;

    // The function draws the result section.
    drawText(w, "Result:", PAD, cursorY, 11, sf::Color(160,160,160));
    cursorY += 16.f;

    float outIconX = PAD + 32.f;
    drawText(w, "->", PAD, cursorY + 14, 14, sf::Color(180,180,180));
    drawRect(w, outIconX, cursorY, ICOSZ, ICOSZ,
             sf::Color(45,45,55), sf::Color(200,200,100,120), 1.5f);
    drawRect(w, outIconX + 5, cursorY + 5, ICOSZ - 10, ICOSZ - 10,
             itemColor(sel.outputName, sel.outputType));
    drawText(w, std::to_string(sel.outputQty) + "x",
             outIconX + ICOSZ - 18, cursorY + ICOSZ - 16, 10, sf::Color::Yellow);

    float outTX = outIconX + ICOSZ + 10.f;
    drawText(w, sel.outputName, outTX, cursorY + 4, 13, sf::Color(255,220,100));
    drawText(w, "Qty: " + std::to_string(sel.outputQty), outTX, cursorY + 22, 11, sf::Color(180,180,180));
    cursorY += ICOSZ + 10.f;

    // The function draws bonus stats for tools and food output.
    if (sel.toolType == ToolType::Sword && sel.bonusDamage > 0) {
        drawText(w, "Attack damage: +" + std::to_string(sel.bonusDamage),
                 PAD, cursorY, 11, sf::Color(220,120,120));
        cursorY += 16.f;
    }
    if (sel.toolType == ToolType::Pickaxe) {
        int pct = (int)((1.f - sel.mineSpeedMult) * 100);
        drawText(w, "Mining speed: " + std::to_string(pct) + "% faster",
                 PAD, cursorY, 11, sf::Color(120,180,220));
        cursorY += 16.f;
    }
    if (sel.hungerRestore > 0) {
        std::string txt = "Restores " + std::to_string(sel.hungerRestore) + " Hunger";
        if (sel.healthRestore > 0)
            txt += " + " + std::to_string(sel.healthRestore) + " HP";
        drawText(w, txt, PAD, cursorY, 11, sf::Color(220,160,80));
    }

    // The function draws the craft button.
    float btnW2 = 160.f, btnH2 = 38.f;
    float btnX  = DETAIL_X + DETAIL_W / 2.f - btnW2 / 2.f;
    float btnY  = DETAIL_Y + DETAIL_H - btnH2 - 6.f;
    bool  btnHov = mouse.x >= btnX && mouse.x <= btnX + btnW2 &&
                   mouse.y >= btnY && mouse.y <= btnY + btnH2;
    sf::Color btnCol = !craftable ? sf::Color(55,55,55)
                     : btnHov    ? sf::Color(60,160,60)
                                 : sf::Color(35,115,35);
    drawRect(w, btnX, btnY, btnW2, btnH2, btnCol,
             sf::Color(craftable ? 80 : 40, craftable ? 200 : 80, craftable ? 80 : 40), 2.f);
    drawText(w, "CRAFT", btnX + btnW2 / 2.f - 22.f, btnY + 10.f, 16,
             craftable ? sf::Color::White : sf::Color(110,110,110));
}

/**
 * @brief The draw function renders the full crafting UI.
 *
 * The draw function displays the background panel, the recipe list, the
 * selected recipe details, the close button, and the status bar when the
 * crafting UI is open.
 *
 * @param w The w parameter represents the render window.
 * @param recipes The recipes parameter represents the recipe list.
 * @param inv The inv parameter represents the player inventory.
 * @param hasTable The hasTable parameter indicates whether a CraftingTable is nearby.
 * @param hasCampfire The hasCampfire parameter indicates whether a Campfire is nearby.
 * @param mouse The mouse parameter represents the mouse position.
 */
void CraftingUI::draw(sf::RenderWindow& w, const std::vector<Recipe>& recipes,
                      const Inventory& inv, bool hasTable, bool hasCampfire,
                      sf::Vector2i mouse) {
    if (!isOpen) return;

    sf::RectangleShape dim({1024.f, 600.f});
    dim.setFillColor(sf::Color(0,0,0,160));
    w.draw(dim);

    drawRect(w, PX, PY, PW, PH, sf::Color(28,28,38), sf::Color(100,100,160), 2.f);
    drawRect(w, PX, PY, PW, 44.f, sf::Color(40,40,60));
    drawText(w, "CRAFTING", PX + 16, PY + 10, 18, sf::Color(255,220,100));

    bool closeHov = mouse.x >= PX + PW - 32 && mouse.x <= PX + PW - 8 &&
                    mouse.y >= PY + 8       && mouse.y <= PY + 32;
    drawRect(w, PX + PW - 32, PY + 8, 24, 24,
             closeHov ? sf::Color(180,40,40) : sf::Color(100,30,30),
             sf::Color(200,100,100), 1.f);
    drawText(w, "X", PX + PW - 25, PY + 10, 14, sf::Color::White);

    drawRecipeList(w, recipes, inv, hasTable, hasCampfire, mouse);

    if (selectedRecipe < (int)recipes.size())
        drawRecipeDetail(w, recipes[selectedRecipe], inv, hasTable, hasCampfire, mouse);

    // The function draws the status bar for nearby crafting stations.
    float sbarY = PY + PH - 28.f;
    drawText(w, hasTable ? "✓ CraftingTable nearby" : "  No CraftingTable",
             PX + 16, sbarY, 11, hasTable ? sf::Color(80,220,80) : sf::Color(140,140,140));
    drawText(w, hasCampfire ? "✓ Campfire nearby" : "  No Campfire",
             PX + 230, sbarY, 11, hasCampfire ? sf::Color(220,140,40) : sf::Color(140,140,140));
    drawText(w, "Green dot = can craft now", PX + PW - 220, sbarY, 11, sf::Color(120,120,120));
}