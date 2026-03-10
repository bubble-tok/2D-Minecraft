#pragma once
#include <vector>
#include <string>

// Standalone tile collision resolver - no circular includes.
// All callers include this instead of pulling World into Animal/Monster.

static const int WORLD_COLS = 40;
static const int WORLD_ROWS = 15;
static const int GROUND_ROW = 9;
static const int TILE_SIZE = 40;

struct TileMap {
    const std::vector<std::vector<std::string>>& blocks;
    int rows, cols, tileSize;

    bool isSolid(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
        return !blocks[r][c].empty();
    }
};

// Resolves AABB collision for a 1-tile-sized entity against the tile map.
// px/py  = top-left world position (modified in place)
// vx/vy  = velocity this frame (vy modified on collision, vx used for direction only)
// onGround = set true if entity is resting on a tile
inline void resolveTileCollision(float& px, float& py,
    float  vx, float& vy,
    bool& onGround,
    const TileMap& map)
{
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;
    onGround = false;

    // --- Pass 1: Y axis ---
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolid(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            if (px + W <= tL || px >= tR) continue;  // no X overlap

            // Falling down
            if (vy >= 0.f && py + H > tT && py + H <= tB) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            // Moving up
            else if (vy < 0.f && py < tB && py >= tT) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // --- Pass 2: X axis (use post-Y py) ---
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolid(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            if (py + H <= tT || py >= tB) continue;  // no Y overlap

            // Moving right
            if (vx > 0.f && px + W > tL && px + W <= tR)
                px = tL - W;
            // Moving left
            else if (vx < 0.f && px < tR && px >= tL)
                px = tR;
        }
    }
}