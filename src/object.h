#ifndef OBJECT_H
#define OBJECT_H
#include "helpers.h"

#define HARMFUL_TILE_1 852
#define HARMFUL_TILE_2 923
#define BACK_IN_TIME_TILE 876
#define DOOR_TILE_1 234
#define DOOR_TILE_2 235
#define DOOR_TILE_3 236

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

/**
 * @brief Checks for collisions between enemies and the player.
 * If an enemy collides with the player, the player loses a life and the enemy is marked as killed.
 */
void collision_check_enemy_vs_player(int scroll_x);

int martin_is_on_obj();
int car_is_on_obj();

/** @brief Checks if Martin is standing over a room door tile.
 *  @return TRUE if any tile overlapping the player is a door tile, FALSE otherwise.
 */
int martin_is_over_room_door();

/**
 * @brief Checks for collisions between the player and coins.
 * If the player overlaps a coin, it is collected.
 */
void collision_check_player_vs_coins();

#endif