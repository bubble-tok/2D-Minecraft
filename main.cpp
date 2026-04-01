#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>

#include "Game.h"
#include "Renderer.h"
#include "CollisionHelper.h"

static const int   WIN_W = 1024;
static const int   WIN_H = 600;
static const float MINE_TIME = 1.5f;
static const float HUNGER_TICK = 4.f;
static const float ATTACK_CD = 0.5f;

// Try to load font from several candidate paths.
// Returns true and fills 'font' on success.
bool tryLoadFont(sf::Font& font) {
    const char* paths[] = {
        "DejaVuSans.ttf",               // next to executable (recommended)
        "ArialTh.ttf",
        "arial.ttf",
        "fonts/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",   // macOS
        "/System/Library/Fonts/Helvetica.ttc",            // macOS fallback
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",// Linux
        "/usr/share/fonts/TTF/DejaVuSans.ttf",            // Arch Linux
        "C:/Windows/Fonts/arial.ttf",                     // Windows
        "C:/Windows/Fonts/calibri.ttf",
    };
    for (auto p : paths)
        if (font.loadFromFile(p)) return true;
    return false;
}

bool showGameOver(sf::RenderWindow& window) {
    sf::Font font;
    bool hasFont = tryLoadFont(font);

    // Reset view to default so UI renders at correct screen coords
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)WIN_W, (float)WIN_H)));

    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f((float)WIN_W, (float)WIN_H));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    // Panel
    sf::RectangleShape panel(sf::Vector2f(420.f, 260.f));
    panel.setFillColor(sf::Color(30, 5, 5, 240));
    panel.setOutlineColor(sf::Color(180, 30, 30));
    panel.setOutlineThickness(3.f);
    panel.setPosition(WIN_W / 2.f - 210.f, WIN_H / 2.f - 130.f);

    // Retry button
    sf::RectangleShape retryBtn(sf::Vector2f(170.f, 55.f));
    retryBtn.setOutlineThickness(2.f);
    retryBtn.setPosition(WIN_W / 2.f - 195.f, WIN_H / 2.f + 40.f);

    // Quit button
    sf::RectangleShape quitBtn(sf::Vector2f(170.f, 55.f));
    quitBtn.setOutlineThickness(2.f);
    quitBtn.setPosition(WIN_W / 2.f + 25.f, WIN_H / 2.f + 40.f);

    // Text objects (used only when font loaded)
    sf::Text titleText, subtitleText, retryText, quitText, hintText;

    if (hasFont) {
        titleText.setFont(font);
        titleText.setString("GAME OVER");
        titleText.setCharacterSize(54);
        titleText.setFillColor(sf::Color(230, 40, 40));
        titleText.setStyle(sf::Text::Bold);
        auto tb = titleText.getLocalBounds();
        titleText.setOrigin(tb.left + tb.width / 2.f, tb.top);
        titleText.setPosition(WIN_W / 2.f, WIN_H / 2.f - 120.f);

        subtitleText.setFont(font);
        subtitleText.setString("You were defeated...");
        subtitleText.setCharacterSize(22);
        subtitleText.setFillColor(sf::Color(200, 150, 150));
        auto sb = subtitleText.getLocalBounds();
        subtitleText.setOrigin(sb.left + sb.width / 2.f, sb.top);
        subtitleText.setPosition(WIN_W / 2.f, WIN_H / 2.f - 45.f);

        retryText.setFont(font);
        retryText.setString("Retry  [R]");
        retryText.setCharacterSize(22);
        retryText.setFillColor(sf::Color::White);
        auto rb = retryText.getLocalBounds();
        retryText.setOrigin(rb.left + rb.width / 2.f, rb.top + rb.height / 2.f);
        retryText.setPosition(retryBtn.getPosition().x + 85.f,
            retryBtn.getPosition().y + 27.f);

        quitText.setFont(font);
        quitText.setString("Quit  [Q]");
        quitText.setCharacterSize(22);
        quitText.setFillColor(sf::Color::White);
        auto qb = quitText.getLocalBounds();
        quitText.setOrigin(qb.left + qb.width / 2.f, qb.top + qb.height / 2.f);
        quitText.setPosition(quitBtn.getPosition().x + 85.f,
            quitBtn.getPosition().y + 27.f);

        hintText.setFont(font);
        hintText.setString("R = Retry    Q / Esc = Quit");
        hintText.setCharacterSize(16);
        hintText.setFillColor(sf::Color(150, 150, 150));
        auto hb = hintText.getLocalBounds();
        hintText.setOrigin(hb.left + hb.width / 2.f, hb.top);
        hintText.setPosition(WIN_W / 2.f, WIN_H / 2.f + 105.f);
    }

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) { window.close(); return false; }

            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f m((float)evt.mouseButton.x, (float)evt.mouseButton.y);
                if (retryBtn.getGlobalBounds().contains(m)) return true;
                if (quitBtn.getGlobalBounds().contains(m)) { window.close(); return false; }
            }

            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::R) return true;
                if (evt.key.code == sf::Keyboard::Q ||
                    evt.key.code == sf::Keyboard::Escape) {
                    window.close(); return false;
                }
            }
        }

        // Hover colours
        sf::Vector2f mouseF(sf::Mouse::getPosition(window));
        retryBtn.setFillColor(retryBtn.getGlobalBounds().contains(mouseF)
            ? sf::Color(90, 180, 90) : sf::Color(55, 120, 55));
        retryBtn.setOutlineColor(retryBtn.getGlobalBounds().contains(mouseF)
            ? sf::Color(150, 255, 150) : sf::Color(90, 180, 90));

        quitBtn.setFillColor(quitBtn.getGlobalBounds().contains(mouseF)
            ? sf::Color(190, 60, 60) : sf::Color(130, 35, 35));
        quitBtn.setOutlineColor(quitBtn.getGlobalBounds().contains(mouseF)
            ? sf::Color(255, 120, 120) : sf::Color(190, 70, 70));

        window.clear(sf::Color(15, 15, 15));
        window.draw(overlay);
        window.draw(panel);
        window.draw(retryBtn);
        window.draw(quitBtn);
        if (hasFont) {
            window.draw(titleText);
            window.draw(subtitleText);
            window.draw(retryText);
            window.draw(quitText);
            window.draw(hintText);
        }
        window.display();
    }
    return false;
}

void resetGame(Game& game, float& vy, float& vx, bool& onGround,
    float& hungerTimer, float& sleepTimer,
    float& attackTimer, float& shakeTimer,
    std::vector<std::string>& eventLog)
{
    game = Game();
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Apple(5)));
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Bread(3)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Wood(10)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Stone(5)));
    game.getPlayer().setPosition(160.f, (GROUND_ROW * TILE_SIZE) - TILE_SIZE);

    vy = 0.f; vx = 0.f; onGround = true;
    hungerTimer = 0.f; sleepTimer = 0.f;
    attackTimer = 0.f; shakeTimer = 0.f;
    eventLog = { "Welcome! WASD=move, Space=jump, E=eat, F=attack" };
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "Minecraft 2D - Stage 3", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game     game;
    Renderer renderer(window);

    std::vector<std::string> eventLog = { "Welcome! WASD=move, Space=jump, E=eat, F=attack" };
    auto addLog = [&](const std::string& msg) {
        eventLog.push_back(msg);
        if ((int)eventLog.size() > 6) eventLog.erase(eventLog.begin());
        };

    float hungerTimer = 0.f, sleepTimer = 0.f;
    float attackTimer = 0.f, shakeTimer = 0.f;
    float vy = 0.f, vx = 0.f;
    bool  onGround = true;
    bool  leftHeld = false, rightHeld = false, jumpHeld = false;

    resetGame(game, vy, vx, onGround,
        hungerTimer, sleepTimer, attackTimer, shakeTimer, eventLog);

    struct MineState { int row = -1, col = -1; float timer = 0.f; bool active = false; };
    MineState mine;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        Player& player = game.getPlayer();
        World& world = game.getWorld();

        // Game Over check
        if (player.getHealth() <= 0) {
            bool retry = showGameOver(window);
            if (!retry || !window.isOpen()) break;
            resetGame(game, vy, vx, onGround,
                hungerTimer, sleepTimer, attackTimer, shakeTimer, eventLog);
            mine = {};
            leftHeld = rightHeld = jumpHeld = false;
            clock.restart();
            continue;
        }

        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) window.close();

            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::A)     leftHeld = true;
                if (evt.key.code == sf::Keyboard::D)     rightHeld = true;
                if (evt.key.code == sf::Keyboard::Space) jumpHeld = true;

                if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(evt.key.code - sf::Keyboard::Num1);

                if (evt.key.code == sf::Keyboard::E) {
                    if (player.eat()) addLog("[EAT] Hunger restored.");
                    else              addLog("[EAT] No food in selected slot!");
                }

                if (evt.key.code == sf::Keyboard::F && attackTimer <= 0.f) {
                    attackTimer = ATTACK_CD;
                    bool hit = false;
                    float px = player.getPositionX(), py = player.getPositionY();
                    for (auto& a : world.getAnimals()) {
                        if (!a->isAlive()) continue;
                        if (std::hypot(a->getX() - px, a->getY() - py) < 90.f) {
                            player.attack(*a); hit = true;
                            if (!a->isAlive()) {
                                auto meat = a->dropMeat();
                                player.pickUp(meat);
                                addLog("[KILL] Got " + meat->getName() + "!");
                            }
                            else addLog("[HIT] Animal HP:" + std::to_string(a->getHp()));
                        }
                    }
                    for (auto& z : world.getZombies()) {
                        if (!z->isAlive()) continue;
                        if (std::hypot(z->getX() - px, z->getY() - py) < 90.f) {
                            player.attack(*z); hit = true;
                            addLog(z->isAlive()
                                ? "[HIT] Zombie HP:" + std::to_string(z->getHp())
                                : "[KILL] Zombie dead!");
                        }
                    }
                    if (!hit) addLog("[ATTACK] No target in range.");
                    world.removeDeadEntities();
                }

                if (evt.key.code == sf::Keyboard::F5) { game.saveGame(); addLog("[SAVE] Saved!"); }
                if (evt.key.code == sf::Keyboard::F9) { game.loadGame(); addLog("[LOAD] Loaded!"); }

                if (evt.key.code == sf::Keyboard::Z) {
                    player.getSleep().sleep();
                    addLog("[SLEEP] You slept. Sleep restored!");
                }
                if (evt.key.code == sf::Keyboard::C) {
                    auto names = game.getCrafting().getRecipeNames();
                    addLog("[CRAFT] Recipes: " + names[0] + ", " + names[1] + "...");
                    addLog("[CRAFT] Use craft(name) in code to craft.");
                }
                if (evt.key.code == sf::Keyboard::Escape) window.close();
            }

            if (evt.type == sf::Event::KeyReleased) {
                if (evt.key.code == sf::Keyboard::A)     leftHeld = false;
                if (evt.key.code == sf::Keyboard::D)     rightHeld = false;
                if (evt.key.code == sf::Keyboard::Space) jumpHeld = false;
            }

            if (evt.type == sf::Event::MouseButtonPressed) {
                float camX = std::max(0.f, player.getPositionX() - WIN_W / 2.f);
                int col = (int)((evt.mouseButton.x + camX) / TILE_SIZE);
                int row = evt.mouseButton.y / TILE_SIZE;

                if (evt.mouseButton.button == sf::Mouse::Left) {
                    std::string blk = world.getBlock(row, col);
                    if (!blk.empty()) { mine = { row, col, 0.f, true }; addLog("[MINE] Mining " + blk + "..."); }
                }
                if (evt.mouseButton.button == sf::Mouse::Right) {
                    auto item = player.getInventory().getSelectedItem();
                    if (item && item->getType() == ItemType::BLOCK) {
                        if (world.placeBlock(row, col, item->getName())) {
                            player.getInventory().removeItem(item->getName(), 1);
                            addLog("[PLACE] Placed " + item->getName());
                        }
                    }
                    else addLog("[PLACE] Select a block first!");
                }
            }
            if (evt.type == sf::Event::MouseButtonReleased &&
                evt.mouseButton.button == sf::Mouse::Left)
                mine.active = false;
        }

        // Mining progress
        if (mine.active) {
            mine.timer += dt;
            if (mine.timer >= MINE_TIME) {
                std::string blk = world.getBlock(mine.row, mine.col);
                if (!blk.empty()) {
                    world.removeBlock(mine.row, mine.col);
                    player.getInventory().addItem(std::make_shared<Block>(blk, 1));
                    addLog("[MINE] Mined " + blk + "!");
                }
                mine = {};
            }
        }

    
        // Decrease hunger over time and apply starvation damage
        hungerTimer += dt;
        if (hungerTimer >= HUNGER_TICK) {
            hungerTimer = 0.f;
            player.getHunger().decrease(1);
            if (player.getHunger().isEmpty()) {
                player.setHealth(player.getHealth() - 1);
                addLog("[HUNGER] Starving! HP: " + std::to_string(player.getHealth()));
                if (player.getHealth() <= 0) {
                    addLog("[DEAD] You starved to death! Game over.");
                    window.close();
                }
            }
        }
        // Sleep decay
        sleepTimer += dt;
        if (sleepTimer >= 8.f) {
            sleepTimer = 0.f;
            player.getSleep().decrease(1);
            if (player.getSleep().isEmpty())
                addLog("[SLEEP] Exhausted! Movement slowed.");
        }

        if (attackTimer > 0.f) attackTimer -= dt;
        if (shakeTimer > 0.f) shakeTimer -= dt;

        float px = player.getPositionX();
        float py = player.getPositionY();

        vx = 0.f;
        if (leftHeld)  vx = -150.f;
        if (rightHeld) vx = 150.f;

        px += vx * dt;
        px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

        vy += 900.f * dt;
        py += vy * dt;

        TileMap tm = world.getTileMap();
        resolveTileCollision(px, py, vx, vy, onGround, tm);

        // Jump after collision resolve, only if headroom exists
        if (jumpHeld && onGround && hasHeadroomAbove(px, py, tm)) {
            vy = -420.f;
            onGround = false;
        }

        player.setPosition(px, py);

        world.update(dt, player);
        for (auto& z : world.getZombies())
            if (std::hypot(z->getX() - px, z->getY() - py) < 50.f) { shakeTimer = 0.3f; break; }

        float camX = std::max(0.f, std::min(px - WIN_W / 2.f,
            (float)(WORLD_COLS * TILE_SIZE - WIN_W)));
        sf::Vector2f shake;
        if (shakeTimer > 0.f) shake = { (float)(rand() % 7 - 3), (float)(rand() % 5 - 2) };
        window.setView(sf::View(sf::FloatRect(shake.x, shake.y, WIN_W, WIN_H)));

        renderer.drawBackground();
        renderer.drawWorld(world, camX);
        renderer.drawEntities(world, camX);
        renderer.drawPlayer(player, camX);
        if (mine.active)
            renderer.drawMineOverlay(mine.row, mine.col, mine.timer / MINE_TIME, camX);
        renderer.drawHUD(player, eventLog, player.getInventory().getSelectedSlot());

        window.display();
    }
    return 0;
}
