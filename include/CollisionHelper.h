/**
 * @file CollisionHelper.h
 * @brief The CollisionHelper.h file defines world constants, the TileMap view structure, and tile-collision resolution functions.
 *
 * The CollisionHelper.h file stores the physics-related world dimensions and
 * declares the two-pass AABB tile collision algorithm. The Game, Animal, and
 * Zombie classes use these shared definitions so movement and collision logic
 * stay consistent without creating a circular dependency on the World class.
 *
 * @author Group 46
 */

#pragma once
#include <vector>
#include <string>

/// The WORLD_COLS constant stores the total number of tile columns in the world grid.
static const int WORLD_COLS = 120;

/// The WORLD_ROWS constant stores the total number of tile rows in the world grid.
static const int WORLD_ROWS = 20;

/// The GROUND_ROW constant stores the row index of the nominal ground surface before terrain height variation.
static const int GROUND_ROW = 9;

/// The TILE_SIZE constant stores the width and height of one tile in pixels.
static const int TILE_SIZE  = 40;

/**
 * @struct TileMap
 * @brief The TileMap structure represents a lightweight read-only view of the world block grid used for collision.
 *
 * The TileMap structure is constructed from World::getTileMap() during each
 * physics frame and is passed to resolveTileCollision(). The TileMap structure
 * does not own the block data.
 *
 * The TileMap structure provides three solid-checking functions so different
 * block types can use different collision behavior:
 * - Leaves tiles are fully non-solid, so entities pass through the canopy.
 * - Wood trunk tiles are solid for horizontal collision but not for vertical collision,
 *   so entities do not stand on top of a trunk tile floating in mid-air.
 *
 * @author Group 46
 */
struct TileMap {
    const std::vector<std::vector<std::string>>& blocks; ///< The blocks variable stores a reference to the world block grid.
    int rows;     ///< The rows variable stores the number of rows in the grid.
    int cols;     ///< The cols variable stores the number of columns in the grid.
    int tileSize; ///< The tileSize variable stores the pixel size of each tile.

    /**
     * @brief The isSolid function checks whether the tile at the given row and column blocks movement in any direction.
     *
     * The isSolid function treats Leaves tiles as non-solid. The isSolid function
     * also returns false for out-of-bounds coordinates.
     *
     * @param r The r parameter represents the row index.
     * @param c The c parameter represents the column index.
     * @return The function returns true if the tile is solid, and false if the tile is passable or out of bounds.
     */
    bool isSolid(int r, int c) const;

    /**
     * @brief The isSolidForY function checks whether the tile acts as a floor or ceiling for vertical collision.
     *
     * The isSolidForY function excludes Wood trunk tiles from Y-axis collision
     * so entities cannot stand on top of a trunk tile floating in mid-air.
     *
     * @param r The r parameter represents the row index.
     * @param c The c parameter represents the column index.
     * @return The function returns true if the tile blocks vertical movement.
     */
    bool isSolidForY(int r, int c) const;

    /**
     * @brief The isSolidForX function checks whether the tile blocks horizontal movement.
     *
     * The isSolidForX function treats all solid tiles, including trunk tiles,
     * as blockers for horizontal movement.
     *
     * @param r The r parameter represents the row index.
     * @param c The c parameter represents the column index.
     * @return The function returns true if the tile blocks horizontal movement.
     */
    bool isSolidForX(int r, int c) const;
};

/**
 * @brief The resolveTileCollision function resolves AABB tile collisions for a moving entity in two passes.
 *
 * The resolveTileCollision function uses the first pass to resolve Y-axis
 * overlaps, such as landing on floors or hitting ceilings. The
 * resolveTileCollision function uses the second pass to resolve X-axis
 * overlaps, such as walking into walls. This separation helps prevent
 * corner-clipping problems that often appear in single-pass approaches.
 *
 * The resolveTileCollision function uses isSolidForY and isSolidForX so that
 * block-specific collision rules, such as Wood trunks and Leaves tiles, are
 * applied automatically.
 *
 * @param px The px parameter stores the entity X position and is modified in place.
 * @param py The py parameter stores the entity Y position and is modified in place.
 * @param vx The vx parameter represents the current horizontal velocity and is used to determine push-out direction.
 * @param vy The vy parameter stores the current vertical velocity and is modified in place.
 * @param onGround The onGround parameter is set to true if the entity lands on a tile during the current frame.
 * @param map The map parameter represents the TileMap view used for collision checks.
 *
 * @author Group 46
 */
void resolveTileCollision(float& px, float& py,
                          float vx, float& vy,
                          bool& onGround,
                          const TileMap& map);

/**
 * @brief The hasHeadroomAbove function checks whether there is clear space directly above the entity.
 *
 * The hasHeadroomAbove function is used to prevent an entity from jumping into
 * a ceiling by checking whether the tiles above the entity are empty before
 * upward movement is applied.
 *
 * @param px The px parameter represents the entity X position.
 * @param py The py parameter represents the entity Y position.
 * @param map The map parameter represents the TileMap view used for the check.
 * @return The function returns true if the tiles immediately above the entity are empty.
 *
 * @author Group 46
 */
bool hasHeadroomAbove(float px, float py, const TileMap& map);