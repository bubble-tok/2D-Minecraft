/**
 * @file CollisionHelper.h
 * @brief Provides tile-based collision detection and resolution utilities.
 *
 * This file defines a simple tile map structure and a helper function used to
 * resolve axis-aligned bounding box (AABB) collisions between entities and
 * the world tile grid.
 */

#pragma once
#include <vector>
#include <string>

/**
 * @brief Number of tile columns in the world.
 */
static const int WORLD_COLS = 40;

/**
 * @brief Number of tile rows in the world.
 */
static const int WORLD_ROWS = 15;

/**
 * @brief Row index representing the ground layer.
 */
static const int GROUND_ROW = 9;

/**
 * @brief Size of each tile in world units (pixels).
 */
static const int TILE_SIZE = 40;

/**
 * @struct TileMap
 * @brief Lightweight representation of the tile grid used for collision detection.
 *
 * The TileMap stores a reference to the world block grid and provides
 * utility methods to determine whether a tile is solid.
 */
struct TileMap {
    const std::vector<std::vector<std::string>>& blocks; ///< Reference to the world block grid
    int rows;     ///< Number of rows in the tile map
    int cols;     ///< Number of columns in the tile map
    int tileSize; ///< Size of each tile (in pixels)

    /**
     * @brief Checks if a tile position contains a solid block.
     *
     * @param r Row index of the tile
     * @param c Column index of the tile
     * @return True if the tile contains a solid block, otherwise false
     */
    bool isSolid(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
        return !blocks[r][c].empty();
    }
};

/**
 * @brief Resolves AABB collision between an entity and the tile map.
 *
 * Performs a two-pass collision resolution:
 * - Pass 1: Vertical (Y-axis) collisions
 * - Pass 2: Horizontal (X-axis) collisions
 *
 * The entity is assumed to be exactly one tile in size.
 *
 * @param px Reference to the entity's X position (top-left world coordinate)
 * @param py Reference to the entity's Y position (top-left world coordinate)
 * @param vx Horizontal velocity for the current frame
 * @param vy Reference to the vertical velocity (modified on collision)
 * @param onGround Set to true if the entity is resting on a tile
 * @param map Reference to the tile map used for collision detection
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

            // Skip if there is no horizontal overlap
            if (px + W <= tL || px >= tR) continue;

            // Falling downward
            if (vy >= 0.f && py + H > tT && py + H <= tB) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            // Moving upward
            else if (vy < 0.f && py < tB && py >= tT) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // --- Pass 2: X axis (after Y adjustment) ---
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolid(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // Skip if there is no vertical overlap
            if (py + H <= tT || py >= tB) continue;

            // Moving right
            if (vx > 0.f && px + W > tL && px + W <= tR)
                px = tL - W;

            // Moving left
            else if (vx < 0.f && px < tR && px >= tL)
                px = tR;
        }
    }
}