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

/**
 * @brief Window width in pixels.
 */
static const int WIN_W = 1024;

/**
 * @brief Window height in pixels.
 */
static const int WIN_H = 600;

/**
 * @brief Time required to mine a block, in seconds.
 */
static const float MINE_TIME = 1.5f;

/**
 * @brief Interval between hunger decreases, in seconds.
 */
static const float HUNGER_TICK = 4.f;

/**
 * @brief Cooldown between player attacks, in seconds.
 */
static const float ATTACK_CD = 0.5f;

/**
 * @brief Displays a win screen and waits for the player to exit.
 *
 * Shows a centered panel with a victory message. The player can
 * press Escape or close the window to exit.
 *
 * @param window Reference to the SFML render window
 */
void showWinScreen(sf::RenderWindow& window) {
    sf::Font font;
    bool hasFont = false;
    for (auto& path : {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Menlo.ttc"
    }) {
        if (font.loadFromFile(path)) { hasFont = true; break; }
    }

    window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)WIN_W, (float)WIN_H)));

    /// Dark overlay
    sf::RectangleShape overlay(sf::Vector2f((float)WIN_W, (float)WIN_H));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    /// Gold-bordered panel
    sf::RectangleShape panel(sf::Vector2f(460.f, 200.f));
    panel.setFillColor(sf::Color(20, 30, 10, 240));
    panel.setOutlineColor(sf::Color(255, 215, 0));
    panel.setOutlineThickness(3.f);
    panel.setPosition(WIN_W / 2.f - 230.f, WIN_H / 2.f - 100.f);

    sf::Text title, subtitle, hint;
    if (hasFont) {
        title.setFont(font);
        title.setString("YOU WIN!");
        title.setCharacterSize(52);
        title.setFillColor(sf::Color(255, 215, 0));
        auto tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top);
        title.setPosition(WIN_W / 2.f, WIN_H / 2.f - 70.f);

        subtitle.setFont(font);
        subtitle.setString("You crafted and ate the Golden Apple!");
        subtitle.setCharacterSize(18);
        subtitle.setFillColor(sf::Color::White);
        auto sb = subtitle.getLocalBounds();
        subtitle.setOrigin(sb.left + sb.width / 2.f, sb.top);
        subtitle.setPosition(WIN_W / 2.f, WIN_H / 2.f);

        hint.setFont(font);
        hint.setString("Press Escape or close the window to exit.");
        hint.setCharacterSize(14);
        hint.setFillColor(sf::Color(180, 180, 180));
        auto hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width / 2.f, hb.top);
        hint.setPosition(WIN_W / 2.f, WIN_H / 2.f + 50.f);
    }

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) window.close();
            if (evt.type == sf::Event::KeyPressed &&
                evt.key.code == sf::Keyboard::Escape) window.close();
        }
        window.clear(sf::Color(15, 15, 15));
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

/**
 * @brief Entry point of the game.
 *
 * Creates the game window, initializes the game systems,
 * processes input, updates gameplay state, and renders
 * each frame until the window is closed.
 *
 * @return Exit status code
 */
int main() {
    /**
     * @brief Main SFML render window.
     */
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "Minecraft 2D - Stage 3", sf::Style::Titlebar | sf::Style::Close);

    /// Limit rendering to 60 frames per second
    window.setFramerateLimit(60);

    /**
     * @brief Main game controller containing world and player logic.
     */
    Game game;

    /**
     * @brief Renderer responsible for drawing the scene.
     */
    Renderer renderer(window);

    /// Add starter food and block items to the player's inventory
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Apple(5)));
    game.getPlayer().pickUp(std::make_shared<Food>(FoodItems::Bread(3)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Wood(10)));
    game.getPlayer().pickUp(std::make_shared<Block>(BlockItems::Stone(5)));

    /// Spawn player so their feet rest exactly on the ground surface
    game.getPlayer().setPosition(160.f, (GROUND_ROW * TILE_SIZE) - TILE_SIZE);

    /**
     * @brief Event log storing recent gameplay messages.
     */
    std::vector<std::string> eventLog = { "Goal: craft & eat a Golden Apple to WIN!" };

    /**
     * @brief Adds a message to the event log, keeping only the most recent six.
     *
     * @param msg Message to append
     */
    auto addLog = [&](const std::string& msg) {
        eventLog.push_back(msg);
        if ((int)eventLog.size() > 6) eventLog.erase(eventLog.begin());
    };

    /**
     * @struct MineState
     * @brief Stores the current mining target and progress.
     */
    struct MineState {
        int row = -1;        ///< Row of the block being mined
        int col = -1;        ///< Column of the block being mined
        float timer = 0.f;   ///< Accumulated mining time
        bool active = false; ///< True if mining is currently in progress
    };

    /**
     * @brief Current mining state.
     */
    MineState mine;

    /// Gameplay timers
    float hungerTimer = 0.f; ///< Accumulates time until next hunger tick
    float sleepTimer  = 0.f; ///< Accumulates time until next sleep decay tick
    float attackTimer = 0.f; ///< Tracks remaining player attack cooldown
    float shakeTimer  = 0.f; ///< Tracks duration of screen shake effect

    /// Player movement physics values
    float vy = 0.f;
    float vx = 0.f;
    bool onGround = true;

    /// Held key state
    bool leftHeld  = false;
    bool rightHeld = false;
    bool jumpHeld  = false;

    /// Win state flag
    bool hasWon = false;

    /**
     * @brief Clock used to compute frame delta time.
     */
    sf::Clock clock;

    /// Main game loop
    while (window.isOpen()) {
        /// Clamp delta time to avoid large physics steps
        float dt = std::min(clock.restart().asSeconds(), 0.05f);

        /**
         * @brief Reference to the player for convenient access.
         */
        Player& player = game.getPlayer();

        /**
         * @brief Reference to the world for convenient access.
         */
        World& world = game.getWorld();

        /// Show win screen if player has won
        if (hasWon) {
            showWinScreen(window);
            break;
        }

        /// Process window and input events
        sf::Event evt;
        while (window.pollEvent(evt)) {
            /// Close the window when requested
            if (evt.type == sf::Event::Closed) window.close();

            /// Handle keyboard input
            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::A) leftHeld  = true;
                if (evt.key.code == sf::Keyboard::D) rightHeld = true;
                if (evt.key.code == sf::Keyboard::Space) jumpHeld = true;

                /// Select hotbar slot using number keys 1-9
                if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(evt.key.code - sf::Keyboard::Num1);

                /// Eat the currently selected food item
                if (evt.key.code == sf::Keyboard::E) {
                    auto selected = player.getInventory().getSelectedItem();
                    bool wasGoldenApple = selected && selected->getName() == "GoldenApple";
                    if (player.eat()) {
                        if (wasGoldenApple) {
                            hasWon = true;
                            addLog("[WIN!] You ate the Golden Apple! You won!");
                        } else {
                            addLog("[EAT] Hunger restored.");
                        }
                    } else {
                        addLog("[EAT] No food in selected slot!");
                    }
                }

                /// Attack nearby animals or zombies if cooldown is ready
                if (evt.key.code == sf::Keyboard::F && attackTimer <= 0.f) {
                    attackTimer = ATTACK_CD;
                    bool hit = false;
                    float px = player.getPositionX(), py = player.getPositionY();

                    for (auto& a : world.getAnimals()) {
                        if (!a->isAlive()) continue;
                        if (std::hypot(a->getX() - px, a->getY() - py) < 90.f) {
                            player.attack(*a);
                            hit = true;
                            if (!a->isAlive()) {
                                auto meat = a->dropMeat();
                                player.pickUp(meat);
                                addLog("[KILL] Got " + meat->getName() + "!");
                            } else {
                                addLog("[HIT] Animal HP:" + std::to_string(a->getHp()));
                            }
                        }
                    }

                    for (auto& z : world.getZombies()) {
                        if (!z->isAlive()) continue;
                        if (std::hypot(z->getX() - px, z->getY() - py) < 90.f) {
                            player.attack(*z);
                            hit = true;
                            addLog(z->isAlive()
                                ? "[HIT] Zombie HP:" + std::to_string(z->getHp())
                                : "[KILL] Zombie dead!");
                        }
                    }

                    if (!hit) addLog("[ATTACK] No target in range.");
                    world.removeDeadEntities();
                }

                /// Save the game state
                if (evt.key.code == sf::Keyboard::F5) {
                    game.saveGame();
                    addLog("[SAVE] Saved!");
                }

                /// Load the game state
                if (evt.key.code == sf::Keyboard::F9) {
                    game.loadGame();
                    addLog("[LOAD] Loaded!");
                }

                /// Restore player sleep meter
                if (evt.key.code == sf::Keyboard::Z) {
                    player.getSleep().sleep();
                    addLog("[SLEEP] You slept. Sleep restored!");
                }

                /// Craft CookedMeat from RawMeat
                if (evt.key.code == sf::Keyboard::C) {
                    if (game.getCrafting().craft("CookedMeat", player.getInventory()))
                        addLog("[CRAFT] Crafted CookedMeat!");
                    else
                        addLog("[CRAFT] Need 1x RawMeat to craft CookedMeat.");
                }

                /// Craft GoldenApple from Apple + Gold
                if (evt.key.code == sf::Keyboard::G) {
                    if (game.getCrafting().craft("GoldenApple", player.getInventory()))
                        addLog("[CRAFT] Crafted GoldenApple! Now eat it to WIN!");
                    else
                        addLog("[CRAFT] Need 1x Apple + 1x Gold to craft GoldenApple.");
                }

                /// Exit the game
                if (evt.key.code == sf::Keyboard::Escape) window.close();
            }

            /// Cancel movement when key is released
            if (evt.type == sf::Event::KeyReleased) {
                if (evt.key.code == sf::Keyboard::A) leftHeld  = false;
                if (evt.key.code == sf::Keyboard::D) rightHeld = false;
                if (evt.key.code == sf::Keyboard::Space) jumpHeld = false;
            }

            /// Handle mouse input
            if (evt.type == sf::Event::MouseButtonPressed) {
                float camX = std::max(0.f, player.getPositionX() - WIN_W / 2.f);
                int col = (int)((evt.mouseButton.x + camX) / TILE_SIZE);
                int row = evt.mouseButton.y / TILE_SIZE;

                /// Left click starts mining a targeted block
                if (evt.mouseButton.button == sf::Mouse::Left) {
                    std::string blk = world.getBlock(row, col);
                    if (!blk.empty()) {
                        mine = { row, col, 0.f, true };
                        addLog("[MINE] Mining " + blk + "...");
                    }
                }

                /// Right click places the selected block into the world
                if (evt.mouseButton.button == sf::Mouse::Right) {
                    auto item = player.getInventory().getSelectedItem();
                    if (item && item->getType() == ItemType::BLOCK) {
                        if (world.placeBlock(row, col, item->getName())) {
                            player.getInventory().removeItem(item->getName(), 1);
                            addLog("[PLACE] Placed " + item->getName());
                        }
                    } else {
                        addLog("[PLACE] Select a block first!");
                    }
                }
            }

            /// Stop mining when left mouse button is released
            if (evt.type == sf::Event::MouseButtonReleased &&
                evt.mouseButton.button == sf::Mouse::Left)
                mine.active = false;
        }

        /// Continue mining progress while active
        if (mine.active) {
            mine.timer += dt;
            if (mine.timer >= MINE_TIME) {
                std::string blk = world.getBlock(mine.row, mine.col);
                if (!blk.empty()) {
                    world.removeBlock(mine.row, mine.col);
                    /// Gold ore drops a Gold item, other blocks drop themselves
                    if (blk == "Gold") {
                        player.getInventory().addItem(
                            std::make_shared<Item>("Gold", 1, ItemType::MISC));
                        addLog("[MINE] Mined Gold ore! Got Gold!");
                    } else {
                        player.getInventory().addItem(
                            std::make_shared<Block>(blk, 1));
                        addLog("[MINE] Mined " + blk + "!");
                    }
                }
                mine = {};
            }
        }

        /// Decrease hunger over time and apply starvation damage if necessary
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

        /// Decrease sleep over time and warn when exhausted
        sleepTimer += dt;
        if (sleepTimer >= 8.f) {
            sleepTimer = 0.f;
            player.getSleep().decrease(1);
            if (player.getSleep().isEmpty())
                addLog("[SLEEP] Exhausted! Press Z to sleep.");
        }

        /// Reduce temporary timers
        if (attackTimer > 0.f) attackTimer -= dt;
        if (shakeTimer  > 0.f) shakeTimer  -= dt;

        /// Read current player position
        float px = player.getPositionX();
        float py = player.getPositionY();

        /// Horizontal movement
        vx = 0.f;
        if (leftHeld)  vx = -150.f;
        if (rightHeld) vx =  150.f;

        px += vx * dt;
        px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

        /// Jump if on ground and space is held
        if (jumpHeld && onGround) {
            vy = -420.f;
            onGround = false;
        }

        /// Apply gravity
        vy += 900.f * dt;
        py += vy * dt;

        /// Resolve tile collisions
        TileMap tm = world.getTileMap();
        resolveTileCollision(px, py, vx, vy, onGround, tm);

        /// Store resolved position
        player.setPosition(px, py);

        /// Update world entities
        world.update(dt, player);

        /// Trigger screen shake when a zombie is very close
        for (auto& z : world.getZombies())
            if (std::hypot(z->getX() - px, z->getY() - py) < 50.f) {
                shakeTimer = 0.3f;
                break;
            }

        /// Compute camera offset clamped to world boundaries
        float camX = std::max(0.f, std::min(px - WIN_W / 2.f,
            (float)(WORLD_COLS * TILE_SIZE - WIN_W)));

        /**
         * @brief Current screen shake offset.
         */
        sf::Vector2f shake;
        if (shakeTimer > 0.f)
            shake = { (float)(rand() % 7 - 3), (float)(rand() % 5 - 2) };

        /// Apply camera view including shake
        window.setView(sf::View(sf::FloatRect(shake.x, shake.y, WIN_W, WIN_H)));

        /// Render the frame
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
