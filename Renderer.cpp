/**
 * @file Renderer.cpp
 * @brief Implements all game rendering: sky, world tiles, entities, player, HUD.
 *
 * Each draw method is called once per frame from main.cpp. World-space methods
 * assume the caller has set an SFML world-space View; drawBackground() and
 * drawHUD() operate in screen space.
 *
 * @author Group 46
 */
#include "Renderer.h"
#include <cmath>
#include <algorithm>

Renderer::Renderer(sf::RenderWindow& win) : window(win) {
    // Try common system font paths in order; use the first one that loads
    for (auto& path : {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Menlo.ttc"
    }) { if (font.loadFromFile(path)) { fontLoaded = true; break; } }
}

sf::Color Renderer::blockColor(const std::string& type) const {
    if (type == "Grass")         return sf::Color(106,127, 60);
    if (type == "Dirt")          return sf::Color(107, 76, 42);
    if (type == "Stone")         return sf::Color(120,120,120);
    if (type == "Wood")          return sf::Color(139,105, 20);
    if (type == "Sand")          return sf::Color(212,180,131);
    if (type == "Gold")          return sf::Color(255,200,  0);
    if (type == "StoneBrick")    return sf::Color(140,130,120);
    if (type == "WoodPlank")     return sf::Color(180,140, 60);
    if (type == "Leaves")        return sf::Color( 34,100, 34);
    if (type == "CraftingTable") return sf::Color(160, 82, 45);
    if (type == "Campfire")      return sf::Color(220, 90, 20);
    if (type == "Bed")           return sf::Color(180, 80,120);
    return sf::Color(100,100,100);
}

sf::Color Renderer::blockTopColor(const std::string& type) const {
    if (type == "Grass") return sf::Color(74,124,63); // brighter green top strip
    return blockColor(type);
}

void Renderer::drawText(const std::string& str, float x, float y,
                        unsigned size, sf::Color color) {
    if (!fontLoaded) return;
    sf::Text text(str, font, size);
    text.setPosition(x, y);
    text.setFillColor(color);
    window.draw(text);
}

void Renderer::drawBar(float x, float y, float w, float h,
                       float value, float maxVal, sf::Color fill, sf::Color bg) {
    // Background track
    sf::RectangleShape back({w, h});
    back.setPosition(x, y);
    back.setFillColor(bg);
    window.draw(back);

    // Filled portion proportional to value/maxVal
    float pct = (maxVal > 0) ? value / maxVal : 0.f;
    sf::RectangleShape bar({w * pct, h});
    bar.setPosition(x, y);
    bar.setFillColor(fill);
    window.draw(bar);
}

void Renderer::drawWoodTile(float wx, float wy) {
    // Base brown rectangle
    sf::RectangleShape base({(float)TILE-1, (float)TILE-1});
    base.setPosition(wx, wy);
    base.setFillColor(sf::Color(101,67,33));
    base.setOutlineThickness(1.f);
    base.setOutlineColor(sf::Color(60,35,10,120));
    window.draw(base);

    // Vertical wood grain lines
    sf::Color grain(72,45,15,180);
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape line({2.f, (float)TILE-3});
        line.setPosition(wx+8.f+i*10.f, wy+1);
        line.setFillColor(grain);
        window.draw(line);
    }

    // Left-edge highlight for a slight 3D look
    sf::RectangleShape hl({3.f, (float)TILE-3});
    hl.setPosition(wx+1, wy+1);
    hl.setFillColor(sf::Color(160,110,60,80));
    window.draw(hl);
}

void Renderer::drawLeavesTile(float wx, float wy) {
    // Six overlapping circle blobs in varied greens simulate foliage
    struct Blob { float ox,oy,r; sf::Color col; };
    static const Blob blobs[] = {
        {10.f, 8.f,12.f,sf::Color(34,110,34)},
        {28.f, 6.f,11.f,sf::Color(40,130,40)},
        {20.f,14.f,14.f,sf::Color(50,150,50)},
        { 8.f,22.f,10.f,sf::Color(30, 95,30)},
        {32.f,20.f,10.f,sf::Color(45,140,45)},
        {20.f,28.f, 9.f,sf::Color(38,120,38)},
    };
    for (auto& b : blobs) {
        sf::CircleShape c(b.r);
        c.setPosition(wx+b.ox-b.r, wy+b.oy-b.r);
        c.setFillColor(b.col);
        window.draw(c);
    }
    // Small highlight dots for depth
    sf::CircleShape dot(2.5f);
    dot.setFillColor(sf::Color(120,200,80,160));
    dot.setPosition(wx+16, wy+10); window.draw(dot);
    dot.setPosition(wx+26, wy+18); window.draw(dot);
    dot.setPosition(wx+10, wy+24); window.draw(dot);
}

void Renderer::drawBackground(const DayCycle& cycle, float camX, float camY) {
    (void)camY;
    float winW = (float)window.getSize().x;
    float winH = (float)window.getSize().y;

    window.clear(cycle.getSkyColor()); // fill sky with interpolated colour

    // Stars — only visible during evening/night
    if (cycle.isNight() || cycle.getDayTime() > 0.65f) {
        float alpha = cycle.getNightDarkness() * 255.f;
        static const float sx[60] = {120,340,780,1200,1560,200,900,1400,600,1800,
            80,450,1100,1650,300,700,1300,500,1000,1700,160,580,1050,1450,250,
            850,1550,400,950,1250,50,630,1150,1380,700,180,1020,820,1480,350,
            970,1600,130,480,1230,750,1350,290,660,1520,410,890,1070,540,1420,
            320,760,1180,90,1680};
        static const float sy[60] = {30,60,20,50,40,15,70,35,55,25,45,10,65,30,
            75,50,20,80,40,60,25,55,35,15,70,45,28,62,18,52,38,72,22,48,58,12,
            42,68,32,78,50,20,60,34,54,44,24,16,70,36,26,56,46,66,38,14,74,30,
            64,10};
        sf::CircleShape star(1.5f);
        star.setFillColor(sf::Color(255,255,220,(sf::Uint8)alpha));
        for (int i = 0; i < 60; ++i) {
            // Parallax: stars scroll at 1/8 camera speed
            float stx = std::fmod(sx[i] - camX*0.125f + winW*4.f, winW);
            star.setPosition(stx, sy[i]);
            window.draw(star);
        }
    }

    // Sun
    sf::Vector2f sunPos = cycle.getSunPosition(winW, winH);
    if (sunPos.x >= 0.f) {
        sf::CircleShape glow(22.f); glow.setOrigin(22.f,22.f);
        glow.setPosition(sunPos); glow.setFillColor(sf::Color(255,240,100,60));
        window.draw(glow);
        sf::CircleShape sun(14.f); sun.setOrigin(14.f,14.f);
        sun.setPosition(sunPos); sun.setFillColor(sf::Color(255,220,50));
        window.draw(sun);
    }

    // Moon
    sf::Vector2f moonPos = cycle.getMoonPosition(winW, winH);
    if (moonPos.x >= 0.f) {
        sf::CircleShape moon(11.f); moon.setOrigin(11.f,11.f);
        moon.setPosition(moonPos); moon.setFillColor(sf::Color(220,220,240));
        window.draw(moon);
        sf::CircleShape crater(3.f);
        crater.setFillColor(sf::Color(180,180,200,160));
        crater.setPosition(moonPos.x+2,moonPos.y-4); window.draw(crater);
        crater.setRadius(2.f);
        crater.setPosition(moonPos.x-4,moonPos.y+3); window.draw(crater);
    }

    // Night darkness overlay — ramps up at dusk, fades at pre-dawn
    float darkness = cycle.getNightDarkness();
    if (darkness > 0.f) {
        sf::View saved = window.getView();
        window.setView(sf::View(sf::FloatRect(0.f,0.f,winW,winH)));
        sf::RectangleShape ov({winW,winH});
        ov.setFillColor(sf::Color(0,0,20,(sf::Uint8)(darkness*160.f)));
        window.draw(ov);
        window.setView(saved);
    }
}

void Renderer::drawWorld(const World& world) {
    // Cull to visible columns for performance
    sf::View view   = window.getView();
    float viewLeft  = view.getCenter().x - view.getSize().x/2.f;
    float viewRight = view.getCenter().x + view.getSize().x/2.f;
    int colMin = std::max(0,              (int)(viewLeft /TILE)-1);
    int colMax = std::min((int)world.getCols(),(int)(viewRight/TILE)+2);

    const auto& blocks = world.getBlockMap();
    for (int row = 0; row < (int)blocks.size(); ++row) {
        for (int col = colMin; col < colMax; ++col) {
            const std::string& type = blocks[row][col];
            if (type.empty()) continue; // air — skip

            float wx = (float)(col * TILE);
            float wy = (float)(row * TILE);

            // Special tile renderers for organic-looking tree materials
            if (type == "Wood")   { drawWoodTile(wx, wy);   continue; }
            if (type == "Leaves") { drawLeavesTile(wx, wy); continue; }

            // Standard rectangular tile
            sf::RectangleShape tile({(float)TILE-1,(float)TILE-1});
            tile.setPosition(wx, wy);
            tile.setFillColor(blockColor(type));
            tile.setOutlineThickness(1.f);
            tile.setOutlineColor(sf::Color(0,0,0,60));
            window.draw(tile);

            // Grass top-strip accent
            if (type == "Grass") {
                sf::RectangleShape top({(float)TILE-1,5.f});
                top.setPosition(wx, wy);
                top.setFillColor(blockTopColor(type));
                window.draw(top);
            }
            // Gold shimmer line
            if (type == "Gold") {
                sf::RectangleShape shine({(float)TILE-6,3.f});
                shine.setPosition(wx+3, wy+6);
                shine.setFillColor(sf::Color(255,240,100,160));
                window.draw(shine);
            }
            // 2-character label for debugging/clarity (small, semi-transparent)
            drawText(type.substr(0,2), wx+4, wy+TILE-16, 10, sf::Color(0,0,0,100));
        }
    }
}

void Renderer::drawEntities(const World& world) {
    // Animals — pink pig sprites with HP bar
    for (const auto& animal : world.getAnimals()) {
        if (!animal->isAlive()) continue;
        float wx = animal->getX(), wy = animal->getY();
        sf::RectangleShape body({32.f,24.f}); body.setPosition(wx+4,wy+8);
        body.setFillColor(sf::Color(244,160,160));
        body.setOutlineThickness(1.f); body.setOutlineColor(sf::Color(180,80,80));
        window.draw(body);
        drawBar(wx, wy-8, (float)TILE, 4.f,
                (float)animal->getHp(), (float)animal->getHealth().getMaxHp(),
                sf::Color(220,50,50));
        drawText("PIG", wx+4, wy+10, 10, sf::Color::White);
    }

    // Zombies — green sprites with HP bar
    for (const auto& zombie : world.getZombies()) {
        if (!zombie->isAlive()) continue;
        float wx = zombie->getX(), wy = zombie->getY();
        sf::RectangleShape body({28.f,36.f}); body.setPosition(wx+6,wy+4);
        body.setFillColor(sf::Color(90,138,74));
        body.setOutlineThickness(1.f); body.setOutlineColor(sf::Color(40,80,30));
        window.draw(body);
        drawBar(wx, wy-8, (float)TILE, 4.f,
                (float)zombie->getHp(), (float)zombie->getHealth().getMaxHp(),
                sf::Color(50,220,50));
        drawText("ZMB", wx+4, wy+12, 10, sf::Color::White);
    }
}

void Renderer::drawPlayer(const Player& player) {
    float wx = player.getPositionX(), wy = player.getPositionY();

    // Elliptical shadow on the ground for depth
    sf::CircleShape shadow(14.f);
    shadow.setPosition(wx+6, wy+34);
    shadow.setFillColor(sf::Color(0,0,0,50));
    shadow.setScale(1.f, 0.4f);
    window.draw(shadow);

    // Body — blue shirt
    sf::RectangleShape body({28.f,20.f}); body.setPosition(wx+6,wy+18);
    body.setFillColor(sf::Color(74,144,217));
    body.setOutlineThickness(1.f); body.setOutlineColor(sf::Color(44,94,140));
    window.draw(body);

    // Head — skin tone
    sf::RectangleShape head({26.f,24.f}); head.setPosition(wx+7,wy);
    head.setFillColor(sf::Color(244,200,122));
    head.setOutlineThickness(1.f); head.setOutlineColor(sf::Color(200,150,80));
    window.draw(head);

    // Eyes
    sf::RectangleShape eyeL({5.f,5.f}), eyeR({5.f,5.f});
    eyeL.setPosition(wx+11, wy+8); eyeR.setPosition(wx+22, wy+8);
    eyeL.setFillColor(sf::Color(40,40,80)); eyeR.setFillColor(sf::Color(40,40,80));
    window.draw(eyeL); window.draw(eyeR);
}

void Renderer::drawMineOverlay(int row, int col, float progress) {
    float wx = (float)(col * TILE), wy = (float)(row * TILE);

    // Darkening overlay proportional to progress
    sf::RectangleShape ov({(float)TILE,(float)TILE}); ov.setPosition(wx, wy);
    ov.setFillColor(sf::Color(0,0,0,(sf::Uint8)(180 * progress)));
    window.draw(ov);

    // Yellow progress bar at the bottom of the tile
    drawBar(wx+4, wy+TILE-10, TILE-8, 6, progress, 1.f,
            sf::Color(255,200,0), sf::Color(40,40,40));
}

void Renderer::drawTileHighlight(int row, int col, bool inReach) {
    float wx = (float)(col * TILE), wy = (float)(row * TILE);
    sf::RectangleShape outline({(float)TILE,(float)TILE});
    outline.setPosition(wx, wy);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(2.f);
    // White = reachable, red = out of reach
    outline.setOutlineColor(inReach ? sf::Color(255,255,255,180)
                                    : sf::Color(220,60,60,180));
    window.draw(outline);
}

void Renderer::drawHUD(Player& player, const std::vector<std::string>& log,
                       int selectedSlot, const DayCycle& cycle) {
    float winW = (float)window.getSize().x;
    float winH = (float)window.getSize().y;

    // ── Stats panel (top-left) ─────────────────────────────────────────────
    sf::RectangleShape panel({160.f,115.f}); panel.setPosition(8,8);
    panel.setFillColor(sf::Color(0,0,0,160)); window.draw(panel);
    drawText("HP:     "+std::to_string(player.getHealth()), 14, 10, 13);
    drawBar(14,26,140,8,(float)player.getHealth(),100.f,
            sf::Color(220,50,50), sf::Color(40,40,40));
    drawText("HUNGER: "+std::to_string(player.getHungerLevel()), 14, 44, 13);
    drawBar(14,60,140,8,(float)player.getHungerLevel(),100.f,
            sf::Color(230,130,30), sf::Color(40,40,40));
    drawText("SLEEP:  "+std::to_string(player.getSleepLevel()), 14, 78, 13);
    drawBar(14,94,140,8,(float)player.getSleepLevel(),100.f,
            sf::Color(100,160,230), sf::Color(40,40,40));

    // ── Clock panel ────────────────────────────────────────────────────────
    sf::RectangleShape clockPanel({160.f,52.f}); clockPanel.setPosition(8,130);
    clockPanel.setFillColor(sf::Color(0,0,0,160)); window.draw(clockPanel);
    std::string phase = cycle.getPhaseName();
    sf::Color phaseCol = sf::Color::White;
    if (phase=="Morning") phaseCol=sf::Color(255,200,120);
    if (phase=="Noon")    phaseCol=sf::Color(255,255,180);
    if (phase=="Evening") phaseCol=sf::Color(255,140,60);
    if (phase=="Night")   phaseCol=sf::Color(150,170,255);
    drawText("Day "+std::to_string(cycle.getDayCount())+"  "+phase, 14, 133, 13, phaseCol);
    drawBar(14,152,140,6,cycle.getDayTime(),1.f,phaseCol,sf::Color(30,30,50));
    // Phase marker ticks at Morning/Noon/Evening boundaries
    static const float marks[] = {0.25f, 0.5f, 0.75f};
    for (float m : marks) {
        sf::RectangleShape mk({2.f,8.f}); mk.setPosition(14.f+140.f*m-1.f,150.f);
        mk.setFillColor(sf::Color(200,200,200,120)); window.draw(mk);
    }
    if (cycle.isNight()) {
        static float blinkTimer = 0.f; blinkTimer += 0.016f;
        if ((int)(blinkTimer*2.f)%2==0)
            drawText("* Sleep now! (Z near Bed)", 14, 163, 10, sf::Color(150,170,255));
    }

    // ── Hotbar ─────────────────────────────────────────────────────────────
    const int SLOTS=9, SZ=44;
    float hotbarX = (winW - SLOTS*(SZ+2)) / 2.f;
    float hotbarY = winH - SZ - 12;
    for (int i = 0; i < SLOTS; ++i) {
        float sx = hotbarX + i*(SZ+2);
        bool  sel = (i == selectedSlot);
        sf::RectangleShape slot({(float)SZ,(float)SZ}); slot.setPosition(sx,hotbarY);
        slot.setFillColor(sel ? sf::Color(80,160,80,200) : sf::Color(30,30,30,200));
        slot.setOutlineThickness(sel ? 2.f : 1.f);
        slot.setOutlineColor(sel ? sf::Color(140,255,140) : sf::Color(80,80,80));
        window.draw(slot);

        auto item = player.getInventory().getSlot(i);
        if (item) {
            // Choose icon colour by item type/name
            sf::Color ic = sf::Color(120,90,40);
            if (item->getType()==ItemType::FOOD)    ic=sf::Color(200,80,80);
            if (item->getName()=="Gold")            ic=sf::Color(255,200,0);
            if (item->getName()=="GoldenApple")     ic=sf::Color(255,215,0);
            if (item->getType()==ItemType::TOOL) {
                Tool* t = dynamic_cast<Tool*>(item.get());
                if (t) {
                    if (t->getToolType()==ToolType::Sword)    ic=sf::Color(180,180,220);
                    if (t->getToolType()==ToolType::Pickaxe)  ic=sf::Color(160,120,60);
                    if (t->getToolType()==ToolType::Bed)      ic=sf::Color(180,80,120);
                    if (t->getToolType()==ToolType::Campfire) ic=sf::Color(220,90,20);
                }
            }
            sf::RectangleShape icon({26.f,26.f}); icon.setPosition(sx+9,hotbarY+6);
            icon.setFillColor(ic); icon.setOutlineThickness(1.f);
            icon.setOutlineColor(sf::Color(0,0,0,80)); window.draw(icon);
            drawText(std::to_string(item->getQuantity()),
                     sx+SZ-14, hotbarY+SZ-16, 11, sf::Color::Yellow);
            drawText(item->getName().substr(0,2), sx+11, hotbarY+14, 11);
        }
        // Slot number label
        drawText(std::to_string(i+1), sx+2, hotbarY+2, 9, sf::Color(150,150,150));
    }

    // ── Action hints (bottom-right) ────────────────────────────────────────
    sf::RectangleShape hints({240.f,96.f}); hints.setPosition(winW-248,winH-104);
    hints.setFillColor(sf::Color(0,0,0,150)); window.draw(hints);
    drawText("[A/D] Move  [Space] Jump",       winW-244, winH-100, 11);
    drawText("[F] Attack  [E] Eat  [Z] Sleep", winW-244, winH-86,  11);
    drawText("[LClick] Mine  [RClick] Place",  winW-244, winH-72,  11);
    drawText("[Tab] Open Crafting Menu",        winW-244, winH-58,  11, sf::Color(255,220,100));
    drawText("[F5] Save  [F9] Load",            winW-244, winH-44,  11);
    drawText("Goal: Craft + Eat Golden Apple!", winW-244, winH-30,  11, sf::Color(255,215,0));

    // ── Event log (top-right) ──────────────────────────────────────────────
    sf::RectangleShape logPanel({260.f, (float)(log.size()*16+12)});
    logPanel.setPosition(winW-268, 8);
    logPanel.setFillColor(sf::Color(0,0,0,150)); window.draw(logPanel);
    for (int i = 0; i < (int)log.size(); ++i) {
        // Most recent line (back of vector) is full white; older lines are grey
        sf::Color c = (i == (int)log.size()-1) ? sf::Color::White : sf::Color(160,160,160);
        drawText(log[i], winW-264, 14+i*16, 11, c);
    }
}
