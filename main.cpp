/**
 * @file main.cpp
 * @brief Entry point, game loop, title/win/death screens, and button helper.
 *
 * Owns the SFML window and drives the top-level game lifecycle:
 * - Shows the landing/how-to-play page (showLandingPage).
 * - Runs repeated game sessions via runGame().
 * - Shows win (showWinScreen) and death (showDeathScreen) screens between sessions.
 *
 * @author Group 46
 */
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>

#include "Game.h"
#include "Renderer.h"
#include "CollisionHelper.h"
#include "CraftingUI.h"

/// Window width in pixels.
static const int WIN_W = 1024;
/// Window height in pixels.
static const int WIN_H = 600;

/** @brief Tries common system font paths and loads the first one found.
 * @param font SFML Font object to load into.
 * @return True if a font was loaded successfully.
 */
bool tryLoadFont(sf::Font& font) {
    for (auto& path : {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Menlo.ttc"
    }) { if (font.loadFromFile(path)) return true; }
    return false;
}

// ── Button helper ─────────────────────────────────────────────────────────────
/**
 * @brief Simple SFML button widget used on the win, death, and title screens.
 *
 * Renders a coloured rectangle with centred text. Hover state is updated
 * each frame by comparing the mouse position against the button bounds.
 */
struct Button {
    sf::RectangleShape box;     ///< The visual background rectangle.
    sf::Text           label;   ///< The centred text label drawn over the box.
    bool               hovered = false; ///< True while the mouse is over the button.

    /**
     * @brief Constructs a Button at the given screen position with the given style.
     * @param text      Label string.
     * @param x         Left edge in screen pixels.
     * @param y         Top edge in screen pixels.
     * @param w         Width in pixels.
     * @param h         Height in pixels.
     * @param font      SFML Font for the label.
     * @param fill      Background fill colour.
     * @param textColor Label text colour.
     */
    Button(const std::string& text, float x, float y, float w, float h,
           sf::Font& font, sf::Color fill, sf::Color textColor) {
        box.setSize({w, h});
        box.setPosition(x, y);
        box.setFillColor(fill);
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(255,255,255,60));

        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(18);
        label.setFillColor(textColor);
        auto lb = label.getLocalBounds();
        label.setOrigin(lb.left + lb.width/2.f, lb.top + lb.height/2.f);
        label.setPosition(x + w/2.f, y + h/2.f);
    }

    /**
     * @brief Returns true if the given screen-space mouse position is inside this button.
     * @param mouse Screen-space mouse coordinates.
     * @return True if mouse is within the button bounds.
     */
    bool contains(sf::Vector2i mouse) const {
        auto pos = box.getPosition();
        auto sz  = box.getSize();
        return mouse.x >= pos.x && mouse.x <= pos.x + sz.x &&
               mouse.y >= pos.y && mouse.y <= pos.y + sz.y;
    }

    /**
     * @brief Updates hovered state and adjusts box colour for hover feedback.
     * @param mouse Current screen-space mouse position.
     */
    void updateHover(sf::Vector2i mouse) {
        hovered = contains(mouse);
        box.setFillColor(hovered
            ? sf::Color(box.getFillColor().r + 30,
                        box.getFillColor().g + 30,
                        box.getFillColor().b + 30)
            : box.getFillColor());
    }

    /**
     * @brief Draws the button box and label to the given window.
     * @param w SFML render window.
     */
    void draw(sf::RenderWindow& w) {
        w.draw(box); w.draw(label);
    }
};

/**
 * @brief Shows the win screen. Returns true if the player wants to play again.
 */
/**
 * @brief Shows the win screen with Continue, Play Again, and Quit options.
 * @return 0 = quit,  1 = play again (new game),  2 = continue current game
 */
int showWinScreen(sf::RenderWindow& window) {
    sf::Font font;
    bool hasFont = tryLoadFont(font);
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, WIN_W, WIN_H)));

    sf::RectangleShape overlay({(float)WIN_W, (float)WIN_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    sf::RectangleShape panel({560.f, 280.f});
    panel.setFillColor(sf::Color(15, 35, 15, 245));
    panel.setOutlineColor(sf::Color(255, 215, 0));
    panel.setOutlineThickness(3.f);
    panel.setPosition(WIN_W/2.f - 280.f, WIN_H/2.f - 140.f);

    sf::Text title, subtitle;
    if (hasFont) {
        title.setFont(font); title.setString("YOU WIN!");
        title.setCharacterSize(58); title.setFillColor(sf::Color(255, 215, 0));
        auto tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width/2.f, tb.top);
        title.setPosition(WIN_W/2.f, WIN_H/2.f - 120.f);

        subtitle.setFont(font);
        subtitle.setString("You crafted and ate the Golden Apple!");
        subtitle.setCharacterSize(16); subtitle.setFillColor(sf::Color(200, 200, 200));
        auto sb = subtitle.getLocalBounds();
        subtitle.setOrigin(sb.left + sb.width/2.f, sb.top);
        subtitle.setPosition(WIN_W/2.f, WIN_H/2.f - 50.f);
    }

    // Three buttons: Continue | Play Again | Quit
    float btnW  = 155.f, btnH = 44.f, gap = 14.f;
    float totalW = btnW * 3 + gap * 2;
    float startX = WIN_W/2.f - totalW/2.f;
    float btnY   = WIN_H/2.f + 30.f;

    Button btnCont ("Continue",   startX,               btnY, btnW, btnH, font,
                    sf::Color(20,  80,120), sf::Color::White);
    Button btnPlay ("Play Again", startX + btnW + gap,  btnY, btnW, btnH, font,
                    sf::Color(30, 120, 30), sf::Color::White);
    Button btnQuit ("Quit",       startX + (btnW+gap)*2,btnY, btnW, btnH, font,
                    sf::Color(100, 30, 30), sf::Color::White);

    if (hasFont) {
        // Helper label under Continue button
        sf::Text hint;
        hint.setFont(font);
        hint.setString("Keep exploring");
        hint.setCharacterSize(11);
        hint.setFillColor(sf::Color(120,160,200));
        auto hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width/2.f, hb.top);
        hint.setPosition(startX + btnW/2.f, btnY + btnH + 4.f);
        // stored but drawn below in loop
        (void)hint;
    }

    while (window.isOpen()) {
        sf::Vector2i mouse = sf::Mouse::getPosition(window);

        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)             { window.close(); return 0; }
            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::Escape)  { window.close(); return 0; }
                if (evt.key.code == sf::Keyboard::C)       return 2; // Continue
                if (evt.key.code == sf::Keyboard::R)       return 1; // Replay
            }
            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) {
                if (btnCont.contains(mouse)) return 2;
                if (btnPlay.contains(mouse)) return 1;
                if (btnQuit.contains(mouse)) { window.close(); return 0; }
            }
        }

        btnCont.box.setFillColor(btnCont.contains(mouse)
            ? sf::Color(40,120,180) : sf::Color(20,80,120));
        btnPlay.box.setFillColor(btnPlay.contains(mouse)
            ? sf::Color(50,160,50)  : sf::Color(30,120,30));
        btnQuit.box.setFillColor(btnQuit.contains(mouse)
            ? sf::Color(150,50,50)  : sf::Color(100,30,30));

        window.clear(sf::Color(10, 20, 10));
        window.draw(overlay); window.draw(panel);
        if (hasFont) {
            window.draw(title); window.draw(subtitle);
            // Key hints under buttons
            sf::Text kh;
            kh.setFont(font); kh.setCharacterSize(10);
            kh.setFillColor(sf::Color(140,140,140));
            kh.setString("[C] Continue");
            kh.setPosition(startX + btnW/2.f - 30.f, btnY + btnH + 5.f);
            window.draw(kh);
            kh.setString("[R] New Game");
            kh.setPosition(startX + btnW + gap + btnW/2.f - 32.f, btnY + btnH + 5.f);
            window.draw(kh);
        }
        btnCont.draw(window); btnPlay.draw(window); btnQuit.draw(window);
        window.display();
    }
    return 0;
}

/**
 * @brief Shows the death screen with reason and Play Again / Quit buttons.
 * @return True if the player chose to play again.
 */
bool showDeathScreen(sf::RenderWindow& window, const std::string& reason) {
    sf::Font font;
    bool hasFont = tryLoadFont(font);
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, WIN_W, WIN_H)));

    sf::RectangleShape overlay({(float)WIN_W, (float)WIN_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    sf::RectangleShape panel({520.f, 260.f});
    panel.setFillColor(sf::Color(35, 5, 5, 245));
    panel.setOutlineColor(sf::Color(200, 50, 50));
    panel.setOutlineThickness(3.f);
    panel.setPosition(WIN_W/2.f - 260.f, WIN_H/2.f - 130.f);

    sf::Text title, subtitle;
    if (hasFont) {
        title.setFont(font); title.setString("YOU DIED");
        title.setCharacterSize(58); title.setFillColor(sf::Color(220, 50, 50));
        auto tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width/2.f, tb.top);
        title.setPosition(WIN_W/2.f, WIN_H/2.f - 110.f);

        subtitle.setFont(font); subtitle.setString(reason);
        subtitle.setCharacterSize(16); subtitle.setFillColor(sf::Color(200, 200, 200));
        auto sb = subtitle.getLocalBounds();
        subtitle.setOrigin(sb.left + sb.width/2.f, sb.top);
        subtitle.setPosition(WIN_W/2.f, WIN_H/2.f - 40.f);
    }

    // Buttons — Play Again and Quit
    float btnY  = WIN_H/2.f + 20.f;
    float btnW  = 180.f, btnH = 44.f;
    float gap   = 24.f;
    float totalW = btnW * 2 + gap;
    float startX = WIN_W/2.f - totalW/2.f;

    Button btnPlay("Play Again", startX,              btnY, btnW, btnH, font,
                   sf::Color(30, 100, 30), sf::Color::White);
    Button btnQuit("Quit",        startX + btnW + gap, btnY, btnW, btnH, font,
                   sf::Color(100, 30, 30), sf::Color::White);

    while (window.isOpen()) {
        sf::Vector2i mouse = sf::Mouse::getPosition(window);

        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) { window.close(); return false; }
            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::Escape)
                    { window.close(); return false; }
                if (evt.key.code == sf::Keyboard::R) return true; // quick replay
            }
            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) {
                if (btnPlay.contains(mouse)) return true;
                if (btnQuit.contains(mouse)) { window.close(); return false; }
            }
        }

        btnPlay.box.setFillColor(btnPlay.contains(mouse)
            ? sf::Color(50, 140, 50) : sf::Color(30, 100, 30));
        btnQuit.box.setFillColor(btnQuit.contains(mouse)
            ? sf::Color(150, 50, 50) : sf::Color(100, 30, 30));

        window.clear(sf::Color(10, 0, 0));
        window.draw(overlay); window.draw(panel);
        if (hasFont) { window.draw(title); window.draw(subtitle); }
        btnPlay.draw(window); btnQuit.draw(window);
        window.display();
    }
    return false;
}

/**
 * @brief Runs one full game session.
 *
 * @return True if the player wants to play again, false to quit.
 */
bool runGame(sf::RenderWindow& window) {
    Game     game;
    Renderer renderer(window);

    // Load font for crafting UI (same paths as renderer)
    sf::Font craftFont;
    bool craftFontLoaded = tryLoadFont(craftFont);
    CraftingUI craftUI(craftFont, craftFontLoaded);

    // Starter items
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Apple(3)));
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Bread(2)));

    // Spawn on terrain surface
    int spawnCol = 4;
    int spawnRow = game.getWorld().surfaceRowAt(spawnCol);
    game.getPlayer().setPosition(
        (float)(spawnCol * TILE_SIZE),
        (float)((spawnRow - 1) * TILE_SIZE));

    std::vector<std::string> eventLog = {
        "Chop trees for Wood. Mine stone underground.",
        "[R] Wood->Plank  [H] Plank->Stick",
        "[T] Plank->CraftingTable (place it!)",
        "[K/P] Table->Sword/Pickaxe",
        "[G] Apple+Gold->GoldenApple -> EAT to WIN!"
    };

    auto addLog = [&](const std::string& msg) {
        eventLog.push_back(msg);
        if ((int)eventLog.size() > 7) eventLog.erase(eventLog.begin());
    };

    bool        hasWon      = false;
    bool        isDead      = false;
    std::string deathReason;
    float       camX = 0.f, camY = 0.f;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);

        // ── End state ─────────────────────────────────────────────────────────
        if (hasWon) {
            int choice = showWinScreen(window);
            if (choice == 2) {
                // Continue playing — clear the win flag and keep going
                hasWon = false;
                addLog("[WIN] You won! Keep exploring...");
                continue;
            }
            // choice 1 = play again (new game), 0 = quit
            return (choice == 1);
        }
        if (isDead)  return showDeathScreen(window, deathReason);

        Player& player = game.getPlayer();
        World&  world  = game.getWorld();

        game.getInput().beginFrame();

        // ── SFML events ───────────────────────────────────────────────────────
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) return false;

            if (evt.type == sf::Event::KeyPressed) {
                game.getInput().onKeyPressed(evt.key.code);

                switch (evt.key.code) {
                    case sf::Keyboard::E:      game.requestEat();    break;
                    case sf::Keyboard::F:      game.requestAttack(); break;
                    case sf::Keyboard::Z:      game.requestSleep();  break;
                    case sf::Keyboard::F5:     game.saveGame();      break;
                    case sf::Keyboard::F9:     game.loadGame();      break;
                    case sf::Keyboard::Tab:    craftUI.toggle();     break;
                    case sf::Keyboard::Escape:
                        if (craftUI.isOpen) craftUI.close();
                        else return false;
                        break;
                    default: break;
                }

                // Hotbar 1-9
                if (evt.key.code >= sf::Keyboard::Num1 &&
                    evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(
                        evt.key.code - sf::Keyboard::Num1);
            }

            if (evt.type == sf::Event::KeyReleased)
                game.getInput().onKeyReleased(evt.key.code);

            if (evt.type == sf::Event::MouseWheelScrolled && craftUI.isOpen)
                craftUI.handleScroll(evt.mouseWheelScroll.delta,
                                     game.getCrafting().getRecipes());

            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) {

                // If crafting UI is open, route click to it first
                if (craftUI.isOpen) {
                    bool craftClicked = craftUI.handleClick(
                        sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y),
                        game.getCrafting().getRecipes());
                    if (craftClicked) {
                        std::string name = craftUI.getSelectedRecipeName(
                            game.getCrafting().getRecipes());
                        if (!name.empty()) game.requestCraft(name);
                    }
                    // Don't pass click through to world when UI is open
                } else {
                    sf::View worldView(sf::FloatRect(camX, camY, WIN_W, WIN_H));
                    sf::Vector2f worldPos = window.mapPixelToCoords(
                        sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y), worldView);
                    int col = (int)(worldPos.x / TILE_SIZE);
                    int row = (int)(worldPos.y / TILE_SIZE);

                    std::string blk = world.getBlock(row, col);
                    if (!blk.empty()) game.startMining(row, col, blk);
                }
            }

            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Right && !craftUI.isOpen) {
                sf::View worldView(sf::FloatRect(camX, camY, WIN_W, WIN_H));
                sf::Vector2f worldPos = window.mapPixelToCoords(
                    sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y), worldView);
                int col = (int)(worldPos.x / TILE_SIZE);
                int row = (int)(worldPos.y / TILE_SIZE);
                game.placeBlock(row, col);
            }

            if (evt.type == sf::Event::MouseButtonReleased &&
                evt.mouseButton.button == sf::Mouse::Left)
                game.stopMining();
        }

        // ── Update ────────────────────────────────────────────────────────────
        game.update(dt);

        // ── Consume events ────────────────────────────────────────────────────
        for (const auto& e : game.getEvents()) {
            switch (e.type) {
                case Game::Event::PlayerAte:
                    addLog("[EAT] Hunger restored."); break;
                case Game::Event::PlayerAteRaw:
                    addLog("[EAT] Cook RawMeat first! (C near Campfire)"); break;
                case Game::Event::PlayerWon:
                    addLog("[WIN!] You ate the Golden Apple!");
                    hasWon = true; break;
                case Game::Event::PlayerDiedStarvation:
                    isDead = true; deathReason = "You starved to death."; break;
                case Game::Event::PlayerDiedVoid:
                    isDead = true; deathReason = "You fell into the void."; break;
                case Game::Event::PlayerDiedZombie:
                    isDead = true; deathReason = "You were killed by a zombie."; break;
                case Game::Event::HungerWarning:
                    addLog("[HUNGER] Starving! HP:" + e.detail); break;
                case Game::Event::SleepWarning:
                    addLog("[SLEEP] Exhausted! Sleep near your Bed (Z)."); break;
                case Game::Event::AnimalKilled:
                    addLog("[KILL] Got " + e.detail + "!"); break;
                case Game::Event::AnimalHit:
                    addLog("[HIT] Animal HP:" + e.detail); break;
                case Game::Event::ZombieHit:
                    addLog("[HIT] Zombie HP:" + e.detail); break;
                case Game::Event::ZombieKilled:
                    addLog("[KILL] Zombie dead!"); break;
                case Game::Event::NoAttackTarget:
                    addLog("[ATTACK] No target in range."); break;
                case Game::Event::BlockMineHit:
                    addLog("[MINE] Mining " + e.detail + "..."); break;
                case Game::Event::BlockMined:
                    if (e.detail == "Wood+Apple")   addLog("[MINE] Got Wood + Apple!");
                    else if (e.detail == "Leaves+Apple") addLog("[MINE] Leaves dropped an Apple!");
                    else if (e.detail == "Leaves")  {} // silent
                    else                            addLog("[MINE] Got " + e.detail + "!"); break;
                case Game::Event::BlockPlaced:
                    addLog("[PLACE] Placed " + e.detail); break;
                case Game::Event::BlockPlaceFailed:
                    addLog("[PLACE] Can't place here."); break;
                case Game::Event::BlockOutOfReach:
                    addLog("[REACH] Too far away!"); break;
                case Game::Event::InventoryFull:
                    addLog("[INV] Inventory full! Drop something first."); break;
                case Game::Event::Saved:
                    addLog("[SAVE] Saved!"); break;
                case Game::Event::Loaded:
                    addLog("[LOAD] Loaded!"); break;
                case Game::Event::SleptWell:
                    addLog("[SLEEP] Good night! Day " + e.detail + " begins."); break;
                case Game::Event::SleepNeedsBed:
                    addLog("[SLEEP] Need a Bed nearby! (B to craft)"); break;
                case Game::Event::SleepWrongTime:
                    addLog("[SLEEP] Can only sleep at Night."); break;
                case Game::Event::NewDay:
                    addLog("[DAY] Day " + e.detail + " has begun!"); break;
                case Game::Event::CraftOK:
                    addLog("[CRAFT] Made " + e.detail + "!"); break;
                case Game::Event::CraftFail:
                    addLog("[CRAFT] Missing ingredients for " + e.detail + "."); break;
                case Game::Event::CraftNeedTable:
                    addLog("[CRAFT] Need CraftingTable nearby!"); break;
                case Game::Event::CraftNeedCampfire:
                    addLog("[CRAFT] Need Campfire nearby to cook!"); break;
                default: break;
            }
        }

        // ── Camera ────────────────────────────────────────────────────────────
        float px = player.getPositionX();
        float py = player.getPositionY();
        camX = std::max(0.f, std::min(px - WIN_W/2.f,
               (float)(WORLD_COLS * TILE_SIZE - WIN_W)));
        camY = std::max(0.f, std::min(py - WIN_H/2.f,
               (float)(WORLD_ROWS * TILE_SIZE - WIN_H)));

        sf::Vector2f shake;
        if (game.getShakeTimer() > 0.f)
            shake = {(float)(rand() % 7 - 3), (float)(rand() % 5 - 2)};

        // ── Render world ──────────────────────────────────────────────────────
        window.setView(sf::View(sf::FloatRect(
            camX + shake.x, camY + shake.y, WIN_W, WIN_H)));

        renderer.drawBackground(game.getDayCycle(), camX, camY);
        renderer.drawWorld(world);
        renderer.drawEntities(world);
        renderer.drawPlayer(player);

        // ── Tile highlight under mouse (shows reach feedback) ─────────────────
        if (!craftUI.isOpen) {
            sf::View worldView(sf::FloatRect(camX, camY, WIN_W, WIN_H));
            sf::Vector2f mouseWorld = window.mapPixelToCoords(
                sf::Mouse::getPosition(window), worldView);
            int hRow = (int)(mouseWorld.y / TILE_SIZE);
            int hCol = (int)(mouseWorld.x / TILE_SIZE);
            if (hRow >= 0 && hRow < WORLD_ROWS && hCol >= 0 && hCol < WORLD_COLS) {
                float bx = hCol * TILE_SIZE + TILE_SIZE / 2.f;
                float by = hRow * TILE_SIZE + TILE_SIZE / 2.f;
                float ppx = player.getPositionX() + TILE_SIZE / 2.f;
                float ppy = player.getPositionY() + TILE_SIZE / 2.f;
                bool inReach = std::hypot(bx - ppx, by - ppy) <= Game::REACH_DISTANCE;
                renderer.drawTileHighlight(hRow, hCol, inReach);
            }
        }

        const MineState& ms = game.getMineState();
        if (ms.active && !world.getBlock(ms.row, ms.col).empty()) {
            std::string blkType = world.getBlock(ms.row, ms.col);
            float hitProgress   = ms.timer / game.getMineTime();
            int   totalHits     = world.blockMaxDurability(blkType);
            int   hitsLeft      = world.getBlockDurability(ms.row, ms.col);
            float overallProg   = totalHits > 0
                ? ((float)(totalHits - hitsLeft) + hitProgress) / (float)totalHits
                : hitProgress;
            renderer.drawMineOverlay(ms.row, ms.col, overallProg);
        }

        // ── Render HUD ────────────────────────────────────────────────────────
        window.setView(sf::View(sf::FloatRect(0.f, 0.f, WIN_W, WIN_H)));
        renderer.drawHUD(player, eventLog,
                         player.getInventory().getSelectedSlot(),
                         game.getDayCycle());

        // Crafting UI drawn on top of everything (screen space)
        craftUI.draw(window,
                     game.getCrafting().getRecipes(),
                     player.getInventory(),
                     game.isNearCraftingTable(),
                     game.isNearCampfire(),
                     sf::Mouse::getPosition(window));

        window.display();
    }

    return false;
}

// ── Landing page ──────────────────────────────────────────────────────────────

/**
 * @brief Shows the landing/title screen with controls and feature overview.
 * @return True if the player clicked Start, false if they closed the window.
 */
bool showLandingPage(sf::RenderWindow& window) {
    sf::Font font;
    bool hasFont = tryLoadFont(font);
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, WIN_W, WIN_H)));

    // ── Content sections ──────────────────────────────────────────────────────
    // Each section has a header color and a list of lines.
    struct Section {
        std::string             header;
        sf::Color               headerColor;
        std::vector<std::string> lines;
    };

    std::vector<Section> sections = {
        {
            "GOAL",
            sf::Color(255, 215, 0),
            {
                "Survive long enough to craft and eat a Golden Apple.",
                "Golden Apple = 1 Apple + 1 Gold  ->  [G] to craft  ->  [E] to eat",
                "Gold is buried deep underground. Apples drop from trees.",
            }
        },
        {
            "MOVEMENT & COMBAT",
            sf::Color(100, 200, 255),
            {
                "[A] / [D]  Move left / right",
                "[Space]    Jump",
                "[F]        Attack nearby animals or zombies",
                "[LClick]   Mine a block (hold to break it)",
                "[RClick]   Place selected item from hotbar",
                "[1-9]      Select tool bar slot",
            }
        },
        {
            "SURVIVAL",
            sf::Color(230, 130, 30),
            {
                "Hunger drains over time, faster while moving. Eat food to restore it.",
                "Sleep drains too, twice as fast at Night. Sleep in a Bed to restore it.",
                "If Hunger hits 0, you start losing HP. Reach 0 HP and you die.",
                "[E]  Eat selected food item   &   [Z]  Sleep (Night + near a Bed only)",
            }
        },
        {
            "DAY CYCLE  (10 min per day)",
            sf::Color(150, 200, 120),
            {
                "Morning -> Noon -> Evening -> Night  (sky & lighting change in real time)",
                "Zombies spawn faster the further right you explore.",
                "Sleep at Night near a Bed to skip to Morning and restore your Sleep bar.",
                "You CANNOT sleep during the day, wait for Night.",
            }
        },
        {
            "CRAFTING",
            sf::Color(200, 160, 80),
            {
                "Chop trees with [LClick] for Wood.  Mine stone underground for Stone.",
                "[R]  Wood -> WoodPlank (4x)     [H]  WoodPlank -> Stick (4x)",
                "[T]  WoodPlank -> CraftingTable  then [RClick] to place it",
                "[B]  WoodPlank -> Bed            [N]  Wood + Stone -> Campfire",
                "--- Near CraftingTable ---",
                "[P]  WoodenPickaxe (mines faster) [O]  StonePickaxe (mines fastest)",
                "[K]  WoodenSword (+5 dmg)         [L]  StoneSword (+12 dmg)",
                "--- Near Campfire ---",
                "[C]  RawMeat -> CookedMeat  (raw meat cannot be eaten directly!)",
            }
        },
        {
            "TIPS",
            sf::Color(180, 130, 220),
            {
                "Trees drop Apples when chopped (30% chance per trunk tile).",
                "Leaves also have a 20% apple drop chance.",
                "Gold veins are deeper and more common toward the right side of the map.",
                "A pickaxe makes mining significantly faster, craft one early.",
                "Zombies at night are more dangerous, try to have a sword ready.",
                "[F5] Save game   [F9] Load game",
            }
        },
    };

    // ── Layout constants ──────────────────────────────────────────────────────
    const float PANEL_X      = 40.f;
    const float PANEL_W      = WIN_W - 80.f;
    const float TITLE_H      = 110.f;   // reserved for title at top
    const float FOOTER_H     = 70.f;    // reserved for Start button at bottom
    const float CONTENT_H    = WIN_H - TITLE_H - FOOTER_H;
    const float CONTENT_Y    = TITLE_H;

    // Build flat list of renderable text entries with their Y positions
    struct Entry { std::string text; sf::Color color; bool isHeader; float y; };
    std::vector<Entry> entries;
    float lineH  = 17.f;
    float headH  = 22.f;
    float sectionGap = 10.f;
    float y = 0.f;
    for (auto& sec : sections) {
        entries.push_back({sec.header, sec.headerColor, true, y});
        y += headH + 2.f;
        for (auto& line : sec.lines) {
            entries.push_back({"  " + line, sf::Color(210, 210, 210), false, y});
            y += lineH;
        }
        y += sectionGap;
    }
    float totalContentH = y;

    // Scroll offset (pixels into the content)
    float scrollY    = 0.f;
    float maxScrollY = std::max(0.f, totalContentH - CONTENT_H);

    // ── Background gradient rects ─────────────────────────────────────────────
    sf::RectangleShape bgTop({WIN_W, WIN_H / 2.f});
    bgTop.setFillColor(sf::Color(8, 12, 28));
    sf::RectangleShape bgBot({WIN_W, WIN_H / 2.f});
    bgBot.setPosition(0, WIN_H / 2.f);
    bgBot.setFillColor(sf::Color(5, 8, 18));

    // Title panel
    sf::RectangleShape titlePanel({(float)WIN_W, TITLE_H});
    titlePanel.setFillColor(sf::Color(0, 0, 0, 180));

    // Content panel (clipping region visual)
    sf::RectangleShape contentBg({PANEL_W, CONTENT_H});
    contentBg.setPosition(PANEL_X, CONTENT_Y);
    contentBg.setFillColor(sf::Color(0, 0, 0, 120));
    contentBg.setOutlineThickness(1.f);
    contentBg.setOutlineColor(sf::Color(255,255,255,30));

    // Footer panel
    sf::RectangleShape footerPanel({(float)WIN_W, FOOTER_H});
    footerPanel.setPosition(0, WIN_H - FOOTER_H);
    footerPanel.setFillColor(sf::Color(0, 0, 0, 180));

    // Scroll hint bar (right edge)
    sf::RectangleShape scrollBar({4.f, 0.f});
    scrollBar.setFillColor(sf::Color(255,255,255,60));

    // Start button
    float btnW = 200.f, btnH = 40.f;
    sf::RectangleShape btnBox({btnW, btnH});
    btnBox.setPosition(WIN_W/2.f - btnW/2.f, WIN_H - FOOTER_H/2.f - btnH/2.f);
    btnBox.setOutlineThickness(2.f);

    sf::Text btnText, titleText, subtitleText, scrollHint;
    if (hasFont) {
        titleText.setFont(font);
        titleText.setString("Minecraft 2D");
        titleText.setCharacterSize(46);
        titleText.setFillColor(sf::Color(255, 215, 0));
        auto tb = titleText.getLocalBounds();
        titleText.setOrigin(tb.left + tb.width/2.f, tb.top);
        titleText.setPosition(WIN_W/2.f, 12.f);

        subtitleText.setFont(font);
        subtitleText.setString("A 2D survival crafting game & How to Play");
        subtitleText.setCharacterSize(14);
        subtitleText.setFillColor(sf::Color(160, 160, 160));
        auto sb = subtitleText.getLocalBounds();
        subtitleText.setOrigin(sb.left + sb.width/2.f, sb.top);
        subtitleText.setPosition(WIN_W/2.f, 68.f);

        btnText.setFont(font);
        btnText.setString("Start Game");
        btnText.setCharacterSize(20);
        btnText.setFillColor(sf::Color::White);
        auto bb = btnText.getLocalBounds();
        btnText.setOrigin(bb.left + bb.width/2.f, bb.top + bb.height/2.f);
        btnText.setPosition(WIN_W/2.f, WIN_H - FOOTER_H/2.f);

        scrollHint.setFont(font);
        scrollHint.setString("Scroll or use Arrow Keys to read");
        scrollHint.setCharacterSize(11);
        scrollHint.setFillColor(sf::Color(120,120,120));
        auto hb = scrollHint.getLocalBounds();
        scrollHint.setOrigin(hb.left + hb.width/2.f, hb.top);
        scrollHint.setPosition(WIN_W/2.f, WIN_H - FOOTER_H + 4.f);
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Vector2i mouse = sf::Mouse::getPosition(window);

        // ── Events ────────────────────────────────────────────────────────────
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) return false;
            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::Escape)   return false;
                if (evt.key.code == sf::Keyboard::Enter ||
                    evt.key.code == sf::Keyboard::Space)     return true;
                if (evt.key.code == sf::Keyboard::Down)
                    scrollY = std::min(maxScrollY, scrollY + 40.f);
                if (evt.key.code == sf::Keyboard::Up)
                    scrollY = std::max(0.f, scrollY - 40.f);
            }
            if (evt.type == sf::Event::MouseWheelScrolled)
                scrollY = std::clamp(scrollY - evt.mouseWheelScroll.delta * 30.f,
                                     0.f, maxScrollY);
            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) {
                auto bp = btnBox.getPosition();
                auto bs = btnBox.getSize();
                if (mouse.x >= bp.x && mouse.x <= bp.x + bs.x &&
                    mouse.y >= bp.y && mouse.y <= bp.y + bs.y)
                    return true;
            }
        }

        // ── Button hover ──────────────────────────────────────────────────────
        auto bp = btnBox.getPosition();
        auto bs = btnBox.getSize();
        bool btnHover = mouse.x >= bp.x && mouse.x <= bp.x + bs.x &&
                        mouse.y >= bp.y && mouse.y <= bp.y + bs.y;
        btnBox.setFillColor(btnHover ? sf::Color(40,140,40) : sf::Color(20,100,20));
        btnBox.setOutlineColor(btnHover ? sf::Color(120,255,120) : sf::Color(60,180,60));

        // ── Draw background ───────────────────────────────────────────────────
        window.clear(sf::Color(8, 12, 28));
        window.draw(bgTop);
        window.draw(bgBot);

        // Subtle star dots in background
        sf::CircleShape star(1.2f);
        star.setFillColor(sf::Color(255,255,220,80));
        static const float sx[] = {80,200,400,600,750,900,950,150,350,550,700,850,100,300,500,650,800,50,450,700};
        static const float sy[] = {20,50,30,60,15,40,70,80,25,55,35,65,45,10,75,20,50,35,60,90};
        for (int i = 0; i < 20; ++i) { star.setPosition(sx[i], sy[i]); window.draw(star); }

        // Title area
        window.draw(titlePanel);
        if (hasFont) { window.draw(titleText); window.draw(subtitleText); }

        // Content area background
        window.draw(contentBg);

        // ── Render content lines with scroll clipping ─────────────────────────
        // We use a scissor-style approach: only draw lines whose Y (after
        // scroll offset) falls within the visible content area.
        if (hasFont) {
            sf::Text line;
            line.setFont(font);
            for (auto& entry : entries) {
                float drawY = CONTENT_Y + entry.y - scrollY + 8.f;
                // Skip lines outside the visible content window
                if (drawY + 20.f < CONTENT_Y) continue;
                if (drawY > CONTENT_Y + CONTENT_H) continue;

                line.setString(entry.text);
                line.setCharacterSize(entry.isHeader ? 14 : 12);
                line.setFillColor(entry.color);
                if (entry.isHeader) {
                    // Underline via a thin rectangle
                    sf::RectangleShape ul({PANEL_W - 16.f, 1.f});
                    ul.setPosition(PANEL_X + 8.f, drawY + 17.f);
                    ul.setFillColor(sf::Color(entry.color.r, entry.color.g,
                                              entry.color.b, 80));
                    window.draw(ul);
                }
                line.setPosition(PANEL_X + 8.f, drawY);
                window.draw(line);
            }
        }

        // Scroll indicator bar on right edge
        if (maxScrollY > 0.f) {
            float barH   = (CONTENT_H / totalContentH) * CONTENT_H;
            float barTop = CONTENT_Y + (scrollY / maxScrollY) * (CONTENT_H - barH);
            scrollBar.setSize({4.f, barH});
            scrollBar.setPosition(WIN_W - PANEL_X + 4.f, barTop);
            window.draw(scrollBar);
        }

        // Footer + button
        window.draw(footerPanel);
        if (hasFont) window.draw(scrollHint);
        window.draw(btnBox);
        if (hasFont) window.draw(btnText);

        window.display();
    }
    return false;
}

// ── Entry point ───────────────────────────────────────────────────────────────
/** @brief Program entry point. Seeds rand, creates the window, shows the landing page, then loops runGame(). @return 0 on clean exit. */
int main() {
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "Minecraft 2D - Stage 4", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Show landing/how-to-play page first
    if (!showLandingPage(window)) return 0;

    // Keep running as long as the player chooses Play Again
    while (window.isOpen() && runGame(window))
        ;

    return 0;
}


/** @brief Window width in pixels. */
// WIN_W = 1024 (defined at top of file)

/** @brief Window height in pixels. */
// WIN_H = 600 (defined at top of file)
