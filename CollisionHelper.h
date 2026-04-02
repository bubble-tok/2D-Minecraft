/**
 * @file CollisionHelper.h
 * @brief World constants, TileMap view struct, and tile-collision resolution functions.
 *
 * All physics-relevant world dimensions and the two-pass AABB tile collision
 * algorithm are defined here so that Game, Animal, and Zombie can share the
 * same movement resolution without a circular dependency on World.
 *
 * @author Group 46
 */

#pragma once
#include <vector>
#include <string>

/// Total number of tile columns in the world grid.
static const int WORLD_COLS = 120;

/// Total number of tile rows in the world grid.
static const int WORLD_ROWS = 20;

/// Row index of the nominal ground surface (before terrain height variation).
static const int GROUND_ROW = 9;

/// Width and height of one tile in pixels.
static const int TILE_SIZE  = 40;

/**
 * @struct TileMap
 * @brief Lightweight read-only view of the world block grid used for collision.
 *
 * TileMap is constructed from World::getTileMap() each physics frame and
 * passed to resolveTileCollision(). It does not own the block data.
 *
 * Three solid predicates allow different collision behaviour per block type:
 * - Leaves are fully non-solid (entities pass through canopy).
 * - Wood trunks are solid horizontally but not vertically (entities don't
 *   stand on top of a trunk mid-air).
 *
 * @author Group 46
 */
struct TileMap {
    const std::vector<std::vector<std::string>>& blocks; ///< Reference to the world block grid.
    int rows;     ///< Number of rows in the grid (== WORLD_ROWS).
    int cols;     ///< Number of columns in the grid (== WORLD_COLS).
    int tileSize; ///< Pixel size of each tile (== TILE_SIZE).

    /**
     * @brief Returns true if the tile at (r, c) blocks movement in any direction.
     *
     * Leaves are always non-solid. Out-of-bounds coordinates return false.
     *
     * @param r Row index.
     * @param c Column index.
     * @return True if the tile is solid; false if passable or out of bounds.
     */
    bool isSolid(int r, int c) const;

    /**
     * @brief Returns true if the tile acts as a floor or ceiling (Y-axis collision).
     *
     * Wood trunks are excluded from Y-axis collision so entities cannot
     * stand on top of a trunk tile floating in mid-air.
     *
     * @param r Row index.
     * @param c Column index.
     * @return True if the tile blocks vertical movement.
     */
    bool isSolidForY(int r, int c) const;

    /**
     * @brief Returns true if the tile blocks horizontal movement (X-axis collision).
     *
     * All solid tiles (including trunks) block horizontal movement.
     *
     * @param r Row index.
     * @param c Column index.
     * @return True if the tile blocks horizontal movement.
     */
    bool isSolidForX(int r, int c) const;
};

/**
 * @brief Resolves AABB tile collisions for a moving entity in two passes.
 *
 * Pass 1 resolves Y-axis overlaps (landing on floors / hitting ceilings).
 * Pass 2 resolves X-axis overlaps (walking into walls).
 * Separating the axes prevents corner-clipping artefacts common in single-pass
 * approaches.
 *
 * Uses isSolidForY and isSolidForX so that block-type-specific rules
 * (Wood trunks, Leaves) are respected automatically.
 *
 * @param px       Entity X position (modified in place).
 * @param py       Entity Y position (modified in place).
 * @param vx       Current horizontal velocity (read-only; used for push-out direction).
 * @param vy       Current vertical velocity (modified in place; zeroed on collision).
 * @param onGround Set to true if the entity lands on a tile this frame.
 * @param map      TileMap view to collide against.
 *
 * @author Group 46
 */
void resolveTileCollision(float& px, float& py,
                          float vx, float& vy,
                          bool& onGround,
                          const TileMap& map);

/**
 * @brief Returns true if there is at least one tile of clear space above the entity.
 *
 * Used to prevent entities from jumping into a ceiling by verifying headroom
 * before applying upward velocity.
 *
 * @param px  Entity X position.
 * @param py  Entity Y position.
 * @param map TileMap view to check against.
 * @return True if the tiles immediately above the entity are empty.
 *
 * @author Group 46
 */
bool hasHeadroomAbove(float px, float py, const TileMap& map);
