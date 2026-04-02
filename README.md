# Group 46 — 2D Survival Game

A 2D Minecraft-style survival game in C++ with SFML. Mine blocks, hunt animals, craft food, and eat a Golden Apple to win.

## Controls

| Key / Button | Action |
|---|---|
| A / D | Move left / right |
| Space | Jump |
| Left Click (hold) | Mine block |
| Right Click | Place selected block |
| 1 – 9 | Select hotbar slot |
| E | Eat selected food |
| F | Attack nearby enemy/animal |
| Z | Sleep |
| B | Craft Bread (3× Wheat) |
| C | Craft Cooked Meat (1× Raw Meat) |
| G | Craft Golden Apple (1× Apple + 1× Gold) → **WIN** |
| F5 | Save |
| F9 | Load |
| Escape | Quit |


## How to Build

Open `Project1.sln` in Visual Studio 2022 and press run.

Requires SFML (it is already configured in the project).

---

## Acceptance Tests

# 0.1 — Hunting

User Story:As a player, I want to hunt an animal and store its meat so I can eat it when hungry.

1. A pig is visible on screen, player moves within attack range, pig can be targeted.
2. Player is within range of a living pig, player presses `F`, pig takes damage, HP bar drops.
3. Pig's HP reaches 0, final hit lands, pig dies and `RawMeat` is added to inventory.
4. `RawMeat` is in inventory, player selects it and presses `E`, hunger increases, quantity drops by 1.
5. `RawMeat` is in inventory, player presses `C`, `RawMeat` is consumed, `CookedMeat` appears.
6. `CookedMeat` is in inventory, player selects it and presses `E`, hunger and HP restored, quantity drops by 1.

Pass: Meat appears in inventory after kill and eating increases hunger.  
Fail: Meat doesn't appear, or eating has no effect.

---

#0.2 — Moving forward and backwards

User Story: As a player, I want to move left and right to reach animals and blocks.

1. Player is alive and awake, user holds `A`, player moves left.
2. Player is alive and awake, user holds `D`, player moves right.
3. Player is exhausted (sleep empty), user holds `A` or `D`, player moves at half speed.

Pass: Player moves in the correct direction.  
Fail: Player doesn't move, or moves the wrong way.

---

# 0.3 — Jumping

User Story:As a player, I want to jump to get on top of blocks.

1. Player is on the ground, user presses `Space`, player jumps up.
2. A block one level above is in front, user jumps then moves forward, player lands on top of the block.
3. Player is already in the air, user presses `Space` again, nothing happens (no double jump).

Pass: Player can clear one block height.  
Fail: Player doesn't leave the ground, or can double-jump.

---

### 0.4 — Item placement

**User Story:** As a player, I want to place items from my inventory into the world.

1. Player has a block in inventory, player right-clicks an empty tile within reach, block appears in the world.
2. Block was placed successfully, placement completes, inventory quantity drops by 1.
3. Target tile is already occupied, player right-clicks it, block is not placed, inventory unchanged.
4. Target tile is out of reach, player right-clicks it, "Too far to reach!" shown, inventory unchanged.

Pass: Block appears in world and inventory count decreases.  
Fail: Block doesn't appear, or count doesn't decrease.

---

# 0.5 — Item pickup

User Story: As a player, I want to pick up items so I can store them for later.

1. An item is at the player's location, player picks it up, item is added to inventory.
2. Item was picked up, pick-up succeeds, item is removed from the world.
3. Inventory is full, player tries to pick up an item, item stays in the world.

Pass: Item appears in inventory and is removed from the world.  
Fail: Item doesn't appear in inventory, or stays in the world after pickup.

---

# 0.6 — Being attacked by zombies

User Story: As a player, if a zombie hits me, my health should go down.

1. A zombie is near the player, zombie reaches and attacks the player, player HP drops by 10.
2. HP has decreased, player checks health bar, bar visually reflects the lower HP.
3. Zombie keeps attacking, multiple hits land, HP keeps dropping each hit.
4. Player HP reaches 0, final hit lands, game ends.

Pass: HP bar decreases each time a zombie hits.  
Fail: HP doesn't change after being hit.

---

# 0.7 — Attack
User Story: As a player, I want to attack enemies and animals to get resources.

1. Target is within attack range, player presses `F`, target takes 10 damage.
2. Target is out of range, player presses `F`, no damage, "No target in range." shown.
3. Animal HP reaches 0, kill lands, animal drops `RawMeat` into inventory.
4. Zombie HP reaches 0, kill lands, zombie is removed from the world.

Pass: Target takes damage when in range, drops appear on kill.  
Fail: No damage dealt when in range, or no drops on kill.

---

# 0.8 — Fullness decreasing

User Story: As a player, if my hunger hits zero, my health should start dropping.

1. Hunger is at zero, time passes, HP decreases, log shows "[HUNGER] Starving!".
2. HP is dropping from starvation, player eats food, HP stops dropping, hunger increases.
3. HP reaches 0 from starvation, final damage tick, game ends, log shows "[DEAD] You starved to death!".

Pass: HP drops when hunger is zero, and eating stops it.  
Fail: HP doesn't drop at zero hunger, or eating doesn't stop the loss.
