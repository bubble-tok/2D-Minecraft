#pragma once
#include <vector>
#include <string>

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

            // Strict X overlap - entity must genuinely overlap, not just touch edge
            if (px + W <= tL + 1.f || px >= tR - 1.f) continue;

            // Falling down - feet land on tile top
            if (vy >= 0.f && py + H > tT && py + H <= tB) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            // Moving up - head hits tile bottom
            else if (vy < 0.f && py < tB && py >= tT) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // --- Pass 2: X axis (uses post-Y py) ---
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolid(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // Strict Y overlap - must genuinely overlap vertically
            if (py + H <= tT + 1.f || py >= tB - 1.f) continue;

            // Moving right - right edge hits tile left face
            if (vx > 0.f && px + W > tL && px + W <= tR)
                px = tL - W;
            // Moving left - left edge hits tile right face
            else if (vx < 0.f && px < tR && px >= tL)
                px = tR;
        }
    }
}

// Returns true if there is a clear tile-height gap above (px, py).
// Used to prevent jumping through a ceiling.
inline bool hasHeadroomAbove(float px, float py, const TileMap& map) {
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    // The tile row that would be directly above the entity's head
    int headRow = (int)((py - 1.f) / H);  // 1px above top edge
    int colL = (int)(px / W);
    int colR = (int)((px + W - 1.f) / W);

    if (headRow < 0) return false;  // already at top of world - no room
    if (map.isSolid(headRow, colL)) return false;
    if (map.isSolid(headRow, colR)) return false;
    return true;
}
