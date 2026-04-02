#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>

#include "Game.h"
#include "Renderer.h"
#include "CollisionHelper.h"

static const int   WIN_W = 1024;
static const int   WIN_H = 600;

/**
 * @brief Attempts to load a font from common system paths.
 */
bool tryLoadFont(sf::Font& font) {
    for (auto& path : {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Menlo.ttc"
    }) {
        if (font.loadFromFile(path)) return true;
    }
    return false;
}

/**
 * @brief Displays a full-screen end panel and blocks until Escape or close.
 */
void showEndScreen(sf::RenderWindow& window,
                   const std::string& titleStr,
                   const std::string& subtitleStr,
                   sf::Color titleColor,
                   sf::Color panelColor,
                   sf::Color outlineColor,
                   sf::Color bgColor)
{
    sf::Font font;
    bool hasFont = tryLoadFont(font);
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)WIN_W, (float)WIN_H)));

    sf::RectangleShape overlay(sf::Vector2f((float)WIN_W, (float)WIN_H));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    sf::RectangleShape panel(sf::Vector2f(500.f, 210.f));
    panel.setFillColor(panelColor);
    panel.setOutlineColor(outlineColor);
    panel.setOutlineThickness(3.f);
    panel.setPosition(WIN_W / 2.f - 250.f, WIN_H / 2.f - 105.f);

    sf::Text title, subtitle, hint;
    if (hasFont) {
        title.setFont(font);
        title.setString(titleStr);
        title.setCharacterSize(52);
        title.setFillColor(titleColor);
        auto tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top);
        title.setPosition(WIN_W / 2.f, WIN_H / 2.f - 75.f);

        subtitle.setFont(font);
        subtitle.setString(subtitleStr);
        subtitle.setCharacterSize(18);
        subtitle.setFillColor(sf::Color::White);
        auto sb = subtitle.getLocalBounds();
        subtitle.setOrigin(sb.left + sb.width / 2.f, sb.top);
        subtitle.setPosition(WIN_W / 2.f, WIN_H / 2.f + 5.f);

        hint.setFont(font);
        hint.setString("Press Escape or close the window to exit.");
        hint.setCharacterSize(13);
        hint.setFillColor(sf::Color(170, 170, 170));
        auto hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width / 2.f, hb.top);
        hint.setPosition(WIN_W / 2.f, WIN_H / 2.f + 55.f);
    }

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) window.close();
            if (evt.type == sf::Event::KeyPressed &&
                evt.key.code == sf::Keyboard::Escape) window.close();
        }
        window.clear(bgColor);
        window.draw(overlay);
        window.draw(panel);
        if (hasFont) {
            window.draw(title);
            window.draw(subtitle);
            window.draw(hint);
        }
        window.display();
    }
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "Minecraft 2D - Stage 4", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game     game;
    Renderer renderer(window);

    // Give the player starter items
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Apple(5)));
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Bread(3)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Wood(10)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Stone(5)));
    game.getPlayer().setPosition(160.f, (GROUND_ROW * TILE_SIZE) - TILE_SIZE);

    std::vector<std::string> eventLog = { "Goal: craft & eat a Golden Apple to WIN!" };

    auto addLog = [&](const std::string& msg) {
        eventLog.push_back(msg);
        if ((int)eventLog.size() > 6) eventLog.erase(eventLog.begin());
    };

    // ── Game-state flags set by event processing ──────────────────────────────
    bool        hasWon      = false;
    bool        isDead      = false;
    std::string deathReason;

    // ── Camera ────────────────────────────────────────────────────────────────
    float camX = 0.f;
    float camY = 0.f;

    sf::Clock clock;

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);

        // End-state checks — show screen then break
        if (hasWon) {
            showEndScreen(window,
                "YOU WIN!",
                "You crafted and ate the Golden Apple!",
                sf::Color(255, 215, 0),
                sf::Color(20, 30, 10, 240),
                sf::Color(255, 215, 0),
                sf::Color(15, 15, 15));
            break;
        }
        if (isDead) {
            showEndScreen(window,
                "YOU DIED",
                deathReason,
                sf::Color(200, 50, 50),
                sf::Color(30, 5, 5, 240),
                sf::Color(200, 50, 50),
                sf::Color(10, 0, 0));
            break;
        }

        Player& player = game.getPlayer();
        World&  world  = game.getWorld();

        // ── SFML event loop ───────────────────────────────────────────────────
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) window.close();

            if (evt.type == sf::Event::KeyPressed) {
                game.getInput().onKeyPressed(evt.key.code);

                switch (evt.key.code) {
                    case sf::Keyboard::E:      game.requestEat();               break;
                    case sf::Keyboard::F:      game.requestAttack();            break;
                    case sf::Keyboard::Z:      game.requestSleep();             break;
                    case sf::Keyboard::C:      game.requestCraftCookedMeat();   break;
                    case sf::Keyboard::G:      game.requestCraftGoldenApple();  break;
                    case sf::Keyboard::F5:     game.saveGame();                 break;
                    case sf::Keyboard::F9:     game.loadGame();                 break;
                    case sf::Keyboard::Escape: window.close();                  break;
                    default: break;
                }

                // Hotbar slot selection
                if (evt.key.code >= sf::Keyboard::Num1 &&
                    evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(
                        evt.key.code - sf::Keyboard::Num1);
            }

            if (evt.type == sf::Event::KeyReleased)
                game.getInput().onKeyReleased(evt.key.code);

            // Mouse — block mining and placement
            if (evt.type == sf::Event::MouseButtonPressed) {
                // Use the world-space view for accurate tile picking
                sf::View worldView(sf::FloatRect(camX, camY, WIN_W, WIN_H));
                sf::Vector2f worldPos = window.mapPixelToCoords(
                    sf::Vector2i(evt.mouseButton.x, evt.mouseButton.y), worldView);

                int col = (int)(worldPos.x / TILE_SIZE);
                int row = (int)(worldPos.y / TILE_SIZE);

                if (evt.mouseButton.button == sf::Mouse::Left) {
                    std::string blk = world.getBlock(row, col);
                    if (!blk.empty())
                        game.startMining(row, col, blk);
                }
                if (evt.mouseButton.button == sf::Mouse::Right)
                    game.placeBlock(row, col);
            }

            if (evt.type == sf::Event::MouseButtonReleased &&
                evt.mouseButton.button == sf::Mouse::Left)
                game.stopMining();
        }

        // ── Update all game systems ───────────────────────────────────────────
        game.update(dt);

        // ── Consume events emitted by game.update() ───────────────────────────
        for (const auto& e : game.getEvents()) {
            switch (e.type) {
                case Game::Event::PlayerAte:
                    addLog("[EAT] Hunger restored.");
                    break;
                case Game::Event::PlayerWon:
                    addLog("[WIN!] You ate the Golden Apple!");
                    hasWon = true;
                    break;
                case Game::Event::PlayerDiedStarvation:
                    isDead      = true;
                    deathReason = "You starved to death.";
                    break;
                case Game::Event::PlayerDiedVoid:
                    isDead      = true;
                    deathReason = "You fell into the void.";
                    break;
                case Game::Event::PlayerDiedZombie:
                    isDead      = true;
                    deathReason = "You were killed by a zombie.";
                    break;
                case Game::Event::HungerWarning:
                    addLog("[HUNGER] Starving! HP: " + e.detail);
                    break;
                case Game::Event::SleepWarning:
                    addLog("[SLEEP] Exhausted! Press Z to sleep.");
                    break;
                case Game::Event::AnimalKilled:
                    addLog("[KILL] Got " + e.detail + "!");
                    break;
                case Game::Event::ZombieHit:
                    addLog("[HIT] HP remaining: " + e.detail);
                    break;
                case Game::Event::ZombieKilled:
                    addLog("[KILL] Zombie dead!");
                    break;
                case Game::Event::NoAttackTarget:
                    addLog("[ATTACK] No target in range.");
                    break;
                case Game::Event::BlockMineHit:
                    addLog("[MINE] Mining " + e.detail + "...");
                    break;
                case Game::Event::BlockMined:
                    addLog("[MINE] Got " + e.detail + "!");
                    break;
                case Game::Event::BlockPlaced:
                    addLog("[PLACE] Placed " + e.detail);
                    break;
                case Game::Event::BlockPlaceFailed:
                    addLog("[PLACE] Select a block first!");
                    break;
                case Game::Event::Saved:
                    addLog("[SAVE] Saved!");
                    break;
                case Game::Event::Loaded:
                    addLog("[LOAD] Loaded!");
                    break;
                case Game::Event::SleptWell:
                    addLog("[SLEEP] Sleep restored!");
                    break;
                case Game::Event::CraftedCookedMeat:
                    addLog("[CRAFT] Crafted CookedMeat!");
                    break;
                case Game::Event::CraftFailCookedMeat:
                    addLog("[CRAFT] Need 1x RawMeat.");
                    break;
                case Game::Event::CraftedGoldenApple:
                    addLog("[CRAFT] Crafted GoldenApple! Eat it to WIN!");
                    break;
                case Game::Event::CraftFailGoldenApple:
                    addLog("[CRAFT] Need 1x Apple + 1x Gold.");
                    break;
                default: break;
            }
        }

        // ── Camera ────────────────────────────────────────────────────────────
        float px = player.getPositionX();
        float py = player.getPositionY();

        camX = std::max(0.f, std::min(px - WIN_W / 2.f,
               (float)(WORLD_COLS * TILE_SIZE - WIN_W)));
        camY = std::max(0.f, std::min(py - WIN_H / 2.f,
               (float)(WORLD_ROWS * TILE_SIZE - WIN_H)));

        sf::Vector2f shake;
        if (game.getShakeTimer() > 0.f)
            shake = { (float)(rand() % 7 - 3), (float)(rand() % 5 - 2) };

        // ── World rendering (world-space view) ────────────────────────────────
        window.setView(sf::View(sf::FloatRect(
            camX + shake.x, camY + shake.y, WIN_W, WIN_H)));

        renderer.drawBackground();
        renderer.drawWorld(world);
        renderer.drawEntities(world);
        renderer.drawPlayer(player);

        // Mine overlay
        const MineState& ms = game.getMineState();
        if (ms.active && !world.getBlock(ms.row, ms.col).empty()) {
            std::string blkType = world.getBlock(ms.row, ms.col);
            float hitProgress   = ms.timer / 1.5f; // MINE_TIME
            int   totalHits     = world.blockMaxDurability(blkType);
            int   hitsLeft      = world.getBlockDurability(ms.row, ms.col);
            float overallProg   = totalHits > 0
                ? ((float)(totalHits - hitsLeft) + hitProgress) / (float)totalHits
                : hitProgress;
            renderer.drawMineOverlay(ms.row, ms.col, overallProg);
        }

        // ── HUD rendering (screen-space view) ────────────────────────────────
        window.setView(sf::View(sf::FloatRect(0.f, 0.f, WIN_W, WIN_H)));
        renderer.drawHUD(player, eventLog, player.getInventory().getSelectedSlot());

        window.display();
    }

    return 0;
}
