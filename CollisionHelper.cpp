/**
 * @file CollisionHelper.cpp
 * @brief Implements TileMap solid predicates and the two-pass AABB collision resolver.
 *
 * @author Group 46
 */

#include "CollisionHelper.h"

// ── TileMap solid predicates ──────────────────────────────────────────────────

bool TileMap::isSolid(int r, int c) const {
    // Out-of-bounds tiles are treated as air
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
    const std::string& b = blocks[r][c];
    if (b.empty())     return false; // air tile
    if (b == "Leaves") return false; // canopy — entities pass through
    return true;
}

bool TileMap::isSolidForY(int r, int c) const {
    return isSolid(r, c);
}

bool TileMap::isSolidForX(int r, int c) const {
    // All solid blocks (including trunks) block horizontal movement
    return isSolid(r, c);
}

// ── Tile collision resolution ─────────────────────────────────────────────────

void resolveTileCollision(float& px, float& py,
                          float  vx, float& vy,
                          bool& onGround,
                          const TileMap& map)
{
    const float W = (float)map.tileSize; // tile width in pixels
    const float H = (float)map.tileSize; // tile height in pixels
    onGround = false;

    // ── Pass 1: Y axis ────────────────────────────────────────────────────────
    // AABB overlap is tested directly without a velocity gate so that fast-
    // moving entities cannot tunnel through thin floors in a single frame.
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForY(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // Skip tiles that don't overlap horizontally with the entity
            // (small epsilon avoids jitter when standing on a tile edge)
            if (px + W <= tL + 0.5f || px >= tR - 0.5f) continue;

            bool overlapY = (py < tB) && (py + H > tT);
            if (!overlapY) continue;

            float playerBottom = py + H;
            float playerTop = py;

            // Landing only if the player is above the tile top
            if (vy > 0.f && playerBottom <= tT + 12.f && playerTop < tT) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            // Ceiling hit only if the player is below the tile
            else if (vy < 0.f && playerTop >= tB - 12.f) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // ── Pass 2: X axis ────────────────────────────────────────────────────────
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForX(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            if (py + H <= tT + 0.5f || py >= tB - 0.5f) continue;

            bool overlapX = (px < tR) && (px + W > tL);
            if (!overlapX) continue;

            // Push out whichever side has less penetration depth
            float penLeft  = (px + W) - tL; // overlap depth from left side
            float penRight = tR - px;        // overlap depth from right side

            if (penLeft < penRight)
                px = tL - W; // entity came from the left
            else
                px = tR;     // entity came from the right
        }
    }
}

// ── Headroom check ────────────────────────────────────────────────────────────

bool hasHeadroomAbove(float px, float py, const TileMap& map) {
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    int headRow = (int)((py - 1.f) / H); // row directly above the entity
    int colL    = (int)(px / W);           // left edge column
    int colR    = (int)((px + W - 1.f) / W); // right edge column

    if (headRow < 0) return false; // above the world boundary
    if (map.isSolid(headRow, colL)) return false;
    if (map.isSolid(headRow, colR)) return false;
    return true;
}
