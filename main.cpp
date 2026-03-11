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
    std::vector<std::string> eventLog = { "Welcome! WASD=move, Space=jump, E=eat, F=attack" };

    /**
     * @brief Adds a message to the event log.
     *
     * Keeps only the most recent six messages.
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
        int row = -1;       ///< Row of the block being mined
        int col = -1;       ///< Column of the block being mined
        float timer = 0.f;  ///< Accumulated mining time
        bool active = false;///< True if mining is currently in progress
    };

    /**
     * @brief Current mining state.
     */
    MineState mine;

    /// Gameplay timers
    float hungerTimer = 0.f; ///< Accumulates time until next hunger tick
    float sleepTimer = 0.f;  ///< Accumulates time until next sleep decay tick
    float attackTimer = 0.f; ///< Tracks remaining player attack cooldown
    float shakeTimer = 0.f;  ///< Tracks duration of screen shake effect

    /// Player movement physics values used in the main loop
    float vy = 0.f;      ///< Vertical velocity
    float vx = 0.f;      ///< Horizontal velocity
    bool onGround = true;///< True if player is currently on the ground

    /// Check if a key is held, for left and right movement.
    bool leftHeld = false;
    bool rightHeld = false;
    bool jumpHeld = false;

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

        /// Process window and input events
        sf::Event evt;
        while (window.pollEvent(evt)) {
            /// Close the window when requested
            if (evt.type == sf::Event::Closed) window.close();

            /// Handle keyboard input
            if (evt.type == sf::Event::KeyPressed) {
                /// Move the character when A or D is held
                if (evt.key.code == sf::Keyboard::A) leftHeld = true;
                if (evt.key.code == sf::Keyboard::D) rightHeld = true;
                if (evt.key.code == sf::Keyboard::Space) jumpHeld = true;

                /// Select hotbar slot using number keys 1-9
                if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
                    player.getInventory().selectSlot(evt.key.code - sf::Keyboard::Num1);

                /// Eat the currently selected food item
                if (evt.key.code == sf::Keyboard::E) {
                    if (player.eat()) addLog("[EAT] Hunger restored.");
                    else              addLog("[EAT] No food in selected slot!");
                }

                /// Attack nearby animals or zombies if cooldown is ready
                if (evt.key.code == sf::Keyboard::F && attackTimer <= 0.f) {
                    attackTimer = ATTACK_CD;
                    bool hit = false;
                    float px = player.getPositionX(), py = player.getPositionY();

                    /// Check animals in attack range
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

                    /// Check zombies in attack range
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

                    /// Log miss if no valid target was hit
                    if (!hit) addLog("[ATTACK] No target in range.");

                    /// Remove any entities killed during this attack
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

                /// Show available crafting recipes in the log
                if (evt.key.code == sf::Keyboard::C) {
                    auto names = game.getCrafting().getRecipeNames();
                    addLog("[CRAFT] Recipes: " + names[0] + ", " + names[1] + "...");
                    addLog("[CRAFT] Use craft(name) in code to craft.");
                }

                /// Exit the game
                if (evt.key.code == sf::Keyboard::Escape) window.close();
            }

            /// Cancel movement of the player when the key is released
            if (evt.type == sf::Event::KeyReleased) {
                if (evt.key.code == sf::Keyboard::A) leftHeld = false;
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

                /// Right click places the selected block item into the world
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

            /// Stop mining when the left mouse button is released
            if (evt.type == sf::Event::MouseButtonReleased &&
                evt.mouseButton.button == sf::Mouse::Left)
                mine.active = false;
        }

        /// Continue mining progress while mining is active
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

        /// Decrease hunger over time and apply starvation damage if necessary
        hungerTimer += dt;
        if (hungerTimer >= HUNGER_TICK) {
            hungerTimer = 0.f;
            player.getHunger().decrease(1);
            if (player.getHunger().isEmpty()) {
                player.setHealth(player.getHealth() - 1);
                addLog("[HUNGER] Starving!");
            }
        }

        /// Decrease sleep over time and warn when exhausted
        sleepTimer += dt;
        if (sleepTimer >= 8.f) {
            sleepTimer = 0.f;
            player.getSleep().decrease(1);
            if (player.getSleep().isEmpty())
                addLog("[SLEEP] Exhausted! Movement slowed.");
        }

        /// Reduce temporary timers
        if (attackTimer > 0.f) attackTimer -= dt;
        if (shakeTimer > 0.f) shakeTimer -= dt;

        /// Read current player position
        float px = player.getPositionX();
        float py = player.getPositionY();

        // Left and right movement
        vx = 0.f;
        if (leftHeld)
            vx = -150.f;

        if (rightHeld)
            vx = 150.f;

        px += vx * dt;
        px = std::max(0.f, std::min((WORLD_COLS - 1) * (float)TILE_SIZE, px));

        // Jump when the jump button is pressed and the player is on the ground
        if (jumpHeld && onGround) {
            vy = -420.f;
            onGround = false;
        }

        /// Apply gravity
        vy += 900.f * dt;
        py += vy * dt;

        /// Resolve collision between player and tile map
        TileMap tm = world.getTileMap();
        resolveTileCollision(px, py, vx, vy, onGround, tm);

        /// Store resolved player position
        player.setPosition(px, py);

        /// Update world entities
        world.update(dt, player);

        /// Trigger screen shake when a zombie is very close
        for (auto& z : world.getZombies())
            if (std::hypot(z->getX() - px, z->getY() - py) < 50.f) {
                shakeTimer = 0.3f;
                break;
            }

        /// Compute camera X offset while clamping to world boundaries
        float camX = std::max(0.f, std::min(px - WIN_W / 2.f,
            (float)(WORLD_COLS * TILE_SIZE - WIN_W)));

        /**
         * @brief Current screen shake offset.
         */
        sf::Vector2f shake;

        /// Randomize view offset while shake effect is active
        if (shakeTimer > 0.f)
            shake = { (float)(rand() % 7 - 3), (float)(rand() % 5 - 2) };

        /// Apply camera view including shake
        window.setView(sf::View(sf::FloatRect(shake.x, shake.y, WIN_W, WIN_H)));

        /// Render the frame
        renderer.drawBackground();
        renderer.drawWorld(world, camX);
        renderer.drawEntities(world, camX);
        renderer.drawPlayer(player, camX);

        /// Draw mining progress overlay if mining is active
        if (mine.active)
            renderer.drawMineOverlay(mine.row, mine.col, mine.timer / MINE_TIME, camX);

        /// Draw HUD and event log
        renderer.drawHUD(player, eventLog, player.getInventory().getSelectedSlot());

        /// Present the rendered frame
        window.display();
    }

    /// Return success exit code
    return 0;
}