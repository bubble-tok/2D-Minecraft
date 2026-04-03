# Group 46 — 2D Survival Game

A 2D Minecraft-style survival game built in C++ with SFML.  
Explore, mine blocks, gather resources, hunt animals, fight zombies, craft items, and eat a Golden Apple to win.

---

## Controls

| Key / Button                 | Action                                         |
|----------------------------|-----------------------------------------------|
| A / D                      | Move left / right                              |
| Space                      | Jump                                           |
| Left Click (hold)          | Mine block                                     |
| Right Click                | Place selected block or placeable item         |
| 1 – 9                      | Select hotbar slot                             |
| E                          | Eat selected food                              |
| F                          | Attack nearby enemy or animal                  |
| Z                          | Sleep when near a Bed at night                 |
| Tab                        | Open / close crafting menu                     |
| Mouse Click (Crafting UI)  | Select recipe / press **CRAFT**                |
| Mouse Wheel                | Scroll crafting recipes                        |
| F5                         | Save                                           |
| F9                         | Load                                           |
| Escape                     | Close menu or quit                             |

---

## Objective

Survive long enough to craft and eat a **Golden Apple**.

- Recipe: **1 Apple + 1 Gold**
- Apples drop from trees/leaves
- Gold is mined from the world
- Eat the Golden Apple (`E`) to win

---

## Core Gameplay Features

The player must manage three survival stats:

- **Health** — decreases from damage or starvation  
- **Hunger** — drains over time (faster when moving)  
- **Sleep** — drains over time (faster at night)  

Gameplay systems:

- Mining and placing blocks
- Hunting animals for food
- Fighting zombies
- Crafting items using the crafting menu
- Using structures (Crafting Table, Campfire, Bed)
- Day/Night cycle
- Save and load system
- Death and win screens

---

## How to Build

Open `Project1.sln` in **Visual Studio 2022** and run.

Requirements:
- SFML (already configured)

---

## Acceptance Tests

---

### 0.1 — Hunting

**User Story:** As a player, I want to hunt animals for food.

1. A pig is visible, player moves within range.
2. Player presses **F**, pig takes damage.
3. Pig HP reaches 0 → dies → `RawMeat` added to inventory.
4. Player tries to eat `RawMeat` → action is rejected.
5. Player crafts `CookedMeat` near a **Campfire**.
6. Player eats `CookedMeat` → hunger and HP increase.

**Pass:** Meat drops correctly, raw meat cannot be eaten, cooked meat works.  
**Fail:** No drops, or food system behaves incorrectly.

---

### 0.2 — Movement

**User Story:** As a player, I want to move around the world.

1. Hold **A** → move left  
2. Hold **D** → move right  
3. Sleep is 0 → movement speed is reduced  

**Pass:** Movement direction and speed behave correctly  
**Fail:** No movement or incorrect behavior  

---

### 0.3 — Jumping

**User Story:** As a player, I want to jump onto terrain.

1. Press **Space** → player jumps  
2. Player can reach one-block height  
3. No double jump allowed  
4. Short "coyote-time" jump works after leaving ground  

**Pass:** Jump behaves naturally  
**Fail:** Infinite jump or broken physics  

---

### 0.4 — Item Placement

**User Story:** As a player, I want to place items in the world.

1. Right-click empty tile → item placed  
2. Inventory decreases by 1  
3. Occupied tile → placement fails  
4. Out of range → placement fails  

**Pass:** Placement works and updates inventory  
**Fail:** Incorrect placement or inventory bug  

---

### 0.5 — Item Pickup

**User Story:** As a player, I want to collect resources.

1. Mining or killing drops items  
2. Items are added to inventory  
3. If inventory is full → item is not added  

**Pass:** Items are stored correctly  
**Fail:** Items disappear or overflow  

---

### 0.6 — Zombie Attacks

**User Story:** As a player, I take damage from enemies.

1. Zombie reaches player → attack  
2. HP decreases  
3. HP bar updates  
4. HP = 0 → death screen  

**Pass:** Damage applies correctly  
**Fail:** No damage or no death trigger  

---

### 0.7 — Combat

**User Story:** As a player, I want to attack enemies.

1. Press **F** in range → target takes damage  
2. Out of range → no damage  
3. Animal dies → drops meat  
4. Zombie dies → removed  

**Pass:** Combat works correctly  
**Fail:** No damage or incorrect drops  

---

### 0.8 — Hunger System

**User Story:** As a player, I lose health when starving.

1. Hunger = 0 → HP decreases  
2. Eat food → hunger increases  
3. Starvation stops after eating  
4. HP = 0 → death  

**Pass:** Starvation works correctly  
**Fail:** No HP loss or no recovery  

---

### 0.9 — Sleep System

**User Story:** As a player, I want to restore energy.

1. Near Bed at night → press **Z** → sleep restores  
2. Daytime → sleep fails  
3. Not near Bed → sleep fails  
4. Sleep = 0 → slower movement  

**Pass:** Sleep works with correct conditions  
**Fail:** Sleep works incorrectly or has no effect  

---

### 0.10 — Crafting System

**User Story:** As a player, I want to craft items.

1. Press **Tab** → open crafting menu  
2. Select recipe → click **CRAFT**  
3. Correct ingredients → item created  
4. Missing station → crafting fails  
5. Near required station → crafting succeeds  

**Pass:** Crafting respects requirements  
**Fail:** Crafting ignores rules or UI breaks  

---

### 0.11 — Win Condition

**User Story:** As a player, I want to win the game.

1. Collect Apple + Gold  
2. Craft **Golden Apple**  
3. Eat Golden Apple  
4. Win screen appears  

**Pass:** Game ends with victory  
**Fail:** No win trigger  

---

### 0.12 — Save & Load

**User Story:** As a player, I want to save progress.

1. Press **F5** → game saves  
2. Change player state  
3. Press **F9** → state restored  

**Pass:** Player data restores correctly  
**Fail:** Missing or incorrect data after load  

---
