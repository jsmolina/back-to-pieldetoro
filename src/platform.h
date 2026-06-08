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

/**
 * @brief Returns movement delta applied during the last platform_update for a platform index.
 * @param index Platform pool index.
 * @param dx Output horizontal delta.
 * @param dy Output vertical delta.
 * @return TRUE if index is valid and active, FALSE otherwise.
 */
int platform_get_delta(int index, int* dx, int* dy);

/**
 * @brief Finds a platform that supports the provided collision area from below.
 * @param area Area used as support probe (typically player feet area).
 * @param max_snap_pixels Vertical tolerance around platform top.
 * @param platform_index Output supported platform index.
 * @param platform_top_y Output top y coordinate of the platform.
 * @return TRUE when support is found, FALSE otherwise.
 */
int platform_find_support(collisionType area, int max_snap_pixels, int* platform_index, int* platform_top_y);

#endif
