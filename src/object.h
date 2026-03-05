#ifndef OBJECT_H
#define OBJECT_H
#include "helpers.h"

#define HARMFUL_TILE_1 852
#define HARMFUL_TILE_2 923
#define BACK_IN_TIME_TILE 876

#define ROAD 0
#define HARMFUL 1
#define BACK_IN_TIME 2
#define PLATFORM 3
/**
 * @file object.h
 * @brief Header file for object collision detection and management
*/

/**
* @brief Checks if objects are overlapping
* @param area The collision box area to check for overlaps
* @return int - Result of overlap check (FALSE = no overlap, TRUE = overlap detected)
*/
int checkOverObj(collisionType area);

/**
* @brief Checks a head hit, TODO: implement
* @return int - Result of hit check (FALSE = no hit, TRUE = hit detected
*/
int checkHitObj();

/**
* @brief Checks if any wheels are positioned on harmful tile types. Equivalent of checkOverObj for car.
* @return int - Result of check (FALSE = no wheels on harmful tiles, TRUE = wheels detected on harmful tiles)
*/
int wheels_on_tiles();

// extern int tiles_at_positions[4];

/** 
* @brief Checks for collisions between throwable objects and enemies.
* This function retrieves the bounding boxes of all active throwable objects and enemies, and checks for overlaps between them. 
*If a collision is detected, it calls the appropriate functions to handle the hit logic for both the throwable object and the enemy.
*/
void collision_check_throwable_vs_enemy();

#endif