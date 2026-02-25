#ifndef OBJECT_H
#define OBJECT_H

#define HARMFUL_TILE_1 852
#define HARMFUL_TILE_2 923
#define BACK_IN_TIME_TILE 876

#define ROAD 0
#define HARMFUL 1
#define BACK_IN_TIME 2
/**
 * @file object.h
 * @brief Header file for object collision detection and management
 * 
 * @struct collisionType
 * @brief Represents a collision bounding box with position and dimensions
 * @var collisionType::x - X coordinate of the collision box
 * @var collisionType::y - Y coordinate of the collision box
 * @var collisionType::w - Width of the collision box
 * @var collisionType::h - Height of the collision box
 * 
 * @function checkOverObj()
 * @brief Checks if objects are overlapping
 * @return int - Result of overlap check (FALSE = no overlap, TRUE = overlap detected)
 * 
 * @function checkHitObj()
 * @brief Checks if a collision/hit has occurred between objects
 * @return int - Result of hit check (FALSE = no hit, TRUE = hit detected)
 * 
 * @function wheels_on_tiles()
 * @brief Checks if any wheels are positioned on harmful tile types
 * @return int - Result of check (FALSE = no wheels on harmful tiles, TRUE = wheels detected on harmful tiles)
 * 
 * @var tiles_at_positions
 * @brief Global array storing tile information at 4 specific positions
 * @note TODO: remove from global scope
 */

struct collisionType {
    int x;
    int y;
    int w;
    int h;
};

struct coordsType {
    int x;
    int y;
};

int checkOverObj();
int checkHitObj();
int wheels_on_tiles();

// TODO: remove from global
extern int tiles_at_positions[4];

#endif