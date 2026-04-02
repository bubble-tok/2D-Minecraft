#pragma once
#include <vector>
#include <string>

/**
 * @brief Total number of columns in the world tile grid.
 */
static const int WORLD_COLS = 120;

/**
 * @brief Total number of rows in the world tile grid.
 */
static const int WORLD_ROWS = 20;

/**
 * @brief Row index of the main ground surface.
 */
static const int GROUND_ROW = 9;

/**
 * @brief Size of each tile in pixels.
 */
static const int TILE_SIZE = 40;

/**
 * @struct TileMap
 * @brief A lightweight view of the world block grid used for collision detection.
 */
struct TileMap {
    const std::vector<std::vector<std::string>>& blocks;
    int rows, cols, tileSize;

    /**
     * @brief Returns true if the tile at (r, c) is solid.
     * @param r Row index
     * @param c Column index
     */
    bool isSolid(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
        return !blocks[r][c].empty();
    }
};

/**
 * @brief Resolves tile collisions for an entity moving through the world.
 *
 * Performs two passes — first on the Y axis, then on the X axis —
 * to correctly separate the entity from any overlapping solid tiles.
 *
 * @param px Entity X position (modified in place)
 * @param py Entity Y position (modified in place)
 * @param vx Entity horizontal velocity
 * @param vy Entity vertical velocity (modified in place)
 * @param onGround Set to true if the entity lands on a tile this frame
 * @param map The tile map to collide against
 */
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

            if (px + W <= tL + 1.f || px >= tR - 1.f) continue;

            if (vy >= 0.f && py + H > tT && py + H <= tB) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            else if (vy < 0.f && py < tB && py >= tT) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // --- Pass 2: X axis ---
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolid(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            if (py + H <= tT + 1.f || py >= tB - 1.f) continue;

            if (vx > 0.f && px + W > tL && px + W <= tR)
                px = tL - W;
            else if (vx < 0.f && px < tR && px >= tL)
                px = tR;
        }
    }
}

/**
 * @brief Returns true if there is a clear tile-height gap above the entity.
 *
 * Used to prevent jumping through a ceiling.
 *
 * @param px Entity X position
 * @param py Entity Y position
 * @param map The tile map to check against
 */
inline bool hasHeadroomAbove(float px, float py, const TileMap& map) {
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    int headRow = (int)((py - 1.f) / H);
    int colL = (int)(px / W);
    int colR = (int)((px + W - 1.f) / W);

    if (headRow < 0) return false;
    if (map.isSolid(headRow, colL)) return false;
    if (map.isSolid(headRow, colR)) return false;
    return true;
}
