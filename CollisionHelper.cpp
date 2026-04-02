/**
 * @file CollisionHelper.cpp
 * @brief The CollisionHelper.cpp file implements TileMap solid checks and the two-pass AABB collision resolver.
 *
 * @author Group 46
 */

#include "CollisionHelper.h"

// ── TileMap solid predicates ──────────────────────────────────────────────────

bool TileMap::isSolid(int r, int c) const {
    // The function treats out-of-bounds tiles as air.
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false;

    const std::string& b = blocks[r][c];

    // The function treats an empty tile as air.
    if (b.empty()) return false;

    // The function allows entities to pass through Leaves tiles.
    if (b == "Leaves") return false;

    return true;
}

bool TileMap::isSolidForY(int r, int c) const {
    return isSolid(r, c);
}

bool TileMap::isSolidForX(int r, int c) const {
    // The function uses the general solid check for horizontal collision, including tree trunks.
    return isSolid(r, c);
}

// ── Tile collision resolution ─────────────────────────────────────────────────

/**
 * @brief The resolveTileCollision function resolves tile collisions for an entity using separate vertical and horizontal passes.
 *
 * The resolveTileCollision function first resolves vertical overlap so the entity
 * can land on floors or hit ceilings correctly. The resolveTileCollision function
 * then resolves horizontal overlap so the entity does not pass through walls.
 *
 * @param px The px parameter stores the x-position of the entity and is updated during collision resolution.
 * @param py The py parameter stores the y-position of the entity and is updated during collision resolution.
 * @param vx The vx parameter represents the horizontal velocity of the entity.
 * @param vy The vy parameter stores the vertical velocity of the entity and is updated during collision resolution.
 * @param onGround The onGround parameter indicates whether the entity is standing on solid ground after collision resolution.
 * @param map The map parameter represents the TileMap used for collision checks.
 */
void resolveTileCollision(float& px, float& py,
                          float  vx, float& vy,
                          bool& onGround,
                          const TileMap& map)
{
    const float W = (float)map.tileSize; // The W variable stores the tile width in pixels.
    const float H = (float)map.tileSize; // The H variable stores the tile height in pixels.
    onGround = false;

    // The first pass resolves collisions on the Y axis.
    // The overlap test does not depend on velocity alone, which helps prevent tunneling through thin floors.
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForY(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // The function skips tiles that do not overlap horizontally with the entity.
            // The small epsilon reduces jitter when the entity stands on a tile edge.
            if (px + W <= tL + 0.5f || px >= tR - 0.5f) continue;

            bool overlapY = (py < tB) && (py + H > tT);
            if (!overlapY) continue;

            float playerBottom = py + H;
            float playerTop = py;

            // The function treats the collision as a landing when the entity approaches from above.
            if (vy > 0.f && playerBottom <= tT + 12.f && playerTop < tT) {
                py = tT - H;
                vy = 0.f;
                onGround = true;
            }
            // The function treats the collision as a ceiling hit when the entity approaches from below.
            else if (vy < 0.f && playerTop >= tB - 12.f) {
                py = tB;
                vy = 0.f;
            }
        }
    }

    // The second pass resolves collisions on the X axis.
    for (int r = 0; r < map.rows; ++r) {
        for (int c = 0; c < map.cols; ++c) {
            if (!map.isSolidForX(r, c)) continue;

            float tL = c * W, tR = tL + W;
            float tT = r * H, tB = tT + H;

            // The function skips tiles that do not overlap vertically with the entity.
            if (py + H <= tT + 0.5f || py >= tB - 0.5f) continue;

            bool overlapX = (px < tR) && (px + W > tL);
            if (!overlapX) continue;

            // The function compares the two penetration depths and pushes the entity out on the shallower side.
            float penLeft  = (px + W) - tL; // The penLeft variable stores the overlap depth from the left side.
            float penRight = tR - px;       // The penRight variable stores the overlap depth from the right side.

            if (penLeft < penRight)
                px = tL - W; // The function places the entity to the left of the tile.
            else
                px = tR;     // The function places the entity to the right of the tile.
        }
    }
}

// ── Headroom check ────────────────────────────────────────────────────────────

/**
 * @brief The hasHeadroomAbove function checks whether there is open space directly above the entity.
 *
 * The hasHeadroomAbove function checks the tiles above the left and right sides
 * of the entity. The function returns false if solid terrain blocks the space above.
 *
 * @param px The px parameter represents the x-position of the entity.
 * @param py The py parameter represents the y-position of the entity.
 * @param map The map parameter represents the TileMap used for collision checks.
 * @return The function returns true if the space above the entity is clear, and false if solid tiles block the space.
 */
bool hasHeadroomAbove(float px, float py, const TileMap& map) {
    const float W = (float)map.tileSize;
    const float H = (float)map.tileSize;

    int headRow = (int)((py - 1.f) / H);      // The headRow variable stores the row directly above the entity.
    int colL    = (int)(px / W);              // The colL variable stores the column at the left edge of the entity.
    int colR    = (int)((px + W - 1.f) / W);  // The colR variable stores the column at the right edge of the entity.

    // The function returns false when the checked space is above the world boundary.
    if (headRow < 0) return false;

    // The function returns false when the left side above the entity is blocked.
    if (map.isSolid(headRow, colL)) return false;

    // The function returns false when the right side above the entity is blocked.
    if (map.isSolid(headRow, colR)) return false;

    return true;
}