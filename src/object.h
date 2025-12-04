#ifndef OBJECT_H
#define OBJECT_H
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
 * @return int - Result of overlap check (0 = no overlap, non-zero = overlap detected)
 * 
 * @function checkHitObj()
 * @brief Checks if a collision/hit has occurred between objects
 * @return int - Result of hit check (0 = no hit, non-zero = hit detected)
 */

struct collisionType {
    int x;
    int y;
    int w;
    int h;
};

int checkOverObj();
int checkHitObj();

// TODO: remove from global
extern int tiles_at_positions[4];

#endif