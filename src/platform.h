#ifndef PLATFORM_H
#define PLATFORM_H

#include "helpers.h"

#define MAX_PLATFORMS 20

/**
 * @file platform.h
 * @brief Header file for one-way platform objects in the game.
 *
 * Platforms are placed as TMX objects (name="PLATFORM", type="LR") and
 * checked for collision with the player (AABB).
 */

/** @brief Loads the platform sprite from the datafile. Call once at startup. */
void load_platform_spritesheet();

/** @brief Resets all platform slots to inactive. Call before loading a new level. */
void reset_platforms();

/**
 * @brief Parses the TMX data for the given level and populates the platform pool.
 * @param level_id The current level identifier (used to look up TMX data).
 */
void load_level_platforms(int level_id);

/** @brief Updates platform positions for moving platforms. Call once per game tick. */
void platform_update(int scroll_x);

/**
 * @brief Draws all active platforms that are within the visible screen area.
 * @param scroll_x The current horizontal scroll offset.
 */
void draw_platforms(int scroll_x);

/**
 * @brief Fills the provided array with AABB boxes for all active platforms.
 *        Inactive platforms get a zero-sized box.
 * @param boxes Array of at least MAX_PLATFORMS collisionType elements.
 */
void platform_get_all_aabb(collisionType* boxes);

#endif
