#ifndef DOOR_H
#define DOOR_H

#include "helpers.h"

#define MAX_DOORS 5

/**
 * @file door.h
 * @brief Header file for door/portal objects in the game.
 *
 * Doors are placed as TMX objects (name="DOOR", type="O").
 */

/** @brief Resets all door slots to inactive. Call before loading a new level. */
void reset_doors();

/**
 * @brief Parses the TMX data for the given level and populates the door pool.
 * @param level_id The current level identifier (used to look up TMX data).
 */
void load_level_doors(int level_id);

/**
 * @brief Fills the provided array with AABB boxes for all active doors.
 *        Inactive doors get a zero-sized box.
 * @param boxes Array of at least MAX_DOORS collisionType elements.
 */
void door_get_all_aabb(collisionType* boxes);

#endif
