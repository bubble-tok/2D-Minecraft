/**
 * @file CollisionHelper.cpp
 * @brief Implements tile-based collision detection and resolution utilities.
 *
 * This file provides helper functions used to detect and resolve collisions
 * between entities and the tile-based world. It includes:
 * 
 * - Solid tile checks (TileMap)
 * - Two-pass collision resolution (vertical then horizontal)
 * - Headroom detection for jumping logic
 *
 * The collision system uses Axis-Aligned Bounding Boxes (AABB) and separates
 * vertical and horizontal movement to ensure stable and predictable behavior.
 *
 * @author Group 46
 */

#include "CollisionHelper.h"

// ── TileMap solid predicates ──────────────────────────────────────────────────

/**
 * @brief Determines whether a tile is solid at a given row and column.
 *
 * This function checks if a tile should block movement. It treats:
 * - Out-of-bounds tiles as non-solid (air)
 * - Empty tiles as non-solid
 * - "Leaves" as non-solid (entities can pass through)
 *
 * @param r Tile row index.
 * @param c Tile column index.
 * @return True if the tile is solid, false otherwise.
 */
bool TileMap::isSolid(int r, int c) const {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false;

    const std::string& b = blocks[r][c];

    if (b.empty()) return false;
    if (b == "Leaves") return false;

    return true;
}

/**
 * @brief Checks vertical collision solidity.
 *
 * Currently identical to isSolid(), but separated for flexibility
 * if different behavior is needed for vertical collisions.
 *
 * @param r Tile row.
 * @param c Tile column.
 * @return True if solid for vertical collision.
 */
bool TileMap::isSolidForY(int r, int c) const {
    return isSolid(r, c);
}

/**
 * @brief Checks horizontal collision solidity.
 *
 * Uses the same logic as isSolid(), but allows customization
 * for horizontal movement if needed.
 *
 * @param r Tile row.
 * @param c Tile column.
 * @return True if solid for horizontal collision.
 */
bool TileMap::isSolidForX(int r, int c) const {
    return isSolid(r, c);
}

// ── Tile collision resolution ─────────────────────────────────────────────────

/**
 * @brief Resolves collisions between an entity and solid tiles.
 *
 * This function performs collision resolution in two passes:
 *
 * 1. Vertical pass:
 *    - Handles landing on the ground
 *    - Handles hitting ceilings
 *    - Updates vertical velocity and ground state
 *
 * 2. Horizontal pass:
 *    - Prevents movement through walls
 *    - Pushes entity out of tiles based on smallest overlap
 *
 * Separating the axes avoids jitter and ensures stable physics behavior.
 *
 * @param px Reference to entity x-position (modified during resolution).
 * @param py Reference to entity y-position (modified during resolution).
 * @param vx Horizontal velocity (used for context, not modified here).
 * @param vy Reference to vertical velocity (modified when collision occurs).
 * @param onGround Set to true if entity lands on a surface.
 * @param map The TileMap used for collision checks.
 */
void resolveTileCollision(float& px, float& py,
                          float  vx, float& vy,
                          bool& onGround,
                          const TileMap& map)
{
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    onGround = false;

    // ── PASS 1: Vertical collision ───────────────────────────────
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForY(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // Skip if no horizontal overlap
            if (px + W <= tL + 0.5f || px >= tR - 0.5f) continue;

            bool overlapY = (py < tB) && (py + H > tT);
            if (!overlapY) continue;

            float playerBottom = py + H;
            float playerTop = py;

            // Landing on ground
            if (vy > 0.f && playerBottom <= tT + 12.f && playerTop < tT) {
                py = tT - H;   // Snap to top of tile
                vy = 0.f;      // Stop downward velocity
                onGround = true;
            }
            // Hitting ceiling
            else if (vy < 0.f && playerTop >= tB - 12.f) {
                py = tB;       // Push down
                vy = 0.f;      // Stop upward velocity
            }
        }
    }

    // ── PASS 2: Horizontal collision ─────────────────────────────
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForX(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // Skip if no vertical overlap
            if (py + H <= tT + 0.5f || py >= tB - 0.5f) continue;

            bool overlapX = (px < tR) && (px + W > tL);
            if (!overlapX) continue;

            // Calculate penetration depths
            float penLeft  = (px + W) - tL;
            float penRight = tR - px;

            // Resolve on the shallowest side
            if (penLeft < penRight)
                px = tL - W;
            else
                px = tR;
        }
    }
}

// ── Headroom check ────────────────────────────────────────────────────────────

/**
 * @brief Checks if there is space above the entity.
 *
 * This is used for jump validation to prevent the player from moving upward
 * into a solid block. It checks both left and right edges above the entity.
 *
 * @param px Entity x-position.
 * @param py Entity y-position.
 * @param map TileMap used for checking.
 * @return True if space above is clear, false otherwise.
 */
bool hasHeadroomAbove(float px, float py, const TileMap& map) {
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    int headRow = (int)((py - 1.f) / H);
    int colL    = (int)(px / W);
    int colR    = (int)((px + W - 1.f) / W);

    if (headRow < 0) return false;

    if (map.isSolid(headRow, colL)) return false;
    if (map.isSolid(headRow, colR)) return false;

    return true;
}