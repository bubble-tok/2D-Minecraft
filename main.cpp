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

int main() {
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "Minecraft 2D - Stage 3", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game     game;
    Renderer renderer(window);

    // Starting inventory
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Apple(5)));
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Bread(3)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Wood(10)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Stone(5)));

    // Spawn player with feet exactly on ground surface
    game.getPlayer().setPosition(160.f, (GROUND_ROW * TILE_SIZE) - TILE_SIZE);

    std::vector<std::string> eventLog = { "Welcome! WASD=move, Space=jump, E=eat, F=attack" };
    auto addLog = [&](const std::string& msg) {
        eventLog.push_back(msg);
        if ((int)eventLog.size() > 6) eventLog.erase(eventLog.begin());
        };

    struct MineState { int row = -1, col = -1; float timer = 0.f; bool active = false; };
    MineState mine;
    float hungerTimer = 0.f;
    float sleepTimer = 0.f;
    float attackTimer = 0.f;
    float shakeTimer = 0.f;
    float vy = 0.f;
    float vx = 0.f;
    bool  onGround = true;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        Player& player = game.getPlayer();
        World& world = game.getWorld();

        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) window.close();

            if (evt.type == sf::Event::KeyPressed) {
                // Hotbar selection
                if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(evt.key.code - sf::Keyboard::Num1);

                // Eat
                if (evt.key.code == sf::Keyboard::E) {
                    if (player.eat()) addLog("[EAT] Hunger restored.");
                    else              addLog("[EAT] No food in selected slot!");
                }

                // Attack
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

                // Z = Sleep
                if (evt.key.code == sf::Keyboard::Z) {
                    player.getSleep().sleep();
                    addLog("[SLEEP] You slept. Sleep restored!");
                }

                // C = Show crafting recipes
                if (evt.key.code == sf::Keyboard::C) {
                    auto names = game.getCrafting().getRecipeNames();
                    addLog("[CRAFT] Recipes: " + names[0] + ", " + names[1] + "...");
                    addLog("[CRAFT] Use craft(name) in code to craft.");
                }

                if (evt.key.code == sf::Keyboard::Escape) window.close();
            }

            // Mouse input
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

        // Hunger decay
        hungerTimer += dt;
        if (hungerTimer >= HUNGER_TICK) {
            hungerTimer = 0.f;
            player.getHunger().decrease(1);
            if (player.getHunger().isEmpty()) {
                player.setHealth(player.getHealth() - 1);
                addLog("[HUNGER] Starving!");
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

        // Horizontal input
        vx = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  vx = -150.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) vx = 150.f;

        px += vx * dt;
        px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

        // Jump
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && onGround) {
            vy = -420.f; onGround = false;
        }

        // Apply gravity
        vy += 900.f * dt;
        py += vy * dt;

        // Resolve tile collision
        TileMap tm = world.getTileMap();
        resolveTileCollision(px, py, vx, vy, onGround, tm);

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