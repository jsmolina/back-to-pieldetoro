#ifndef SINKING_H
#define SINKING_H

#include "helpers.h"

#define MAX_SINKING 20

/**
 * @file sinking.h
 * @brief Sinking platforms: they appear (solid, standable) and then crumble and
 *        disappear so the player falls, cycling forever. Placed as TMX objects
 *        (name="SINKING"). They do not move, only appear/disappear.
 */

/** @brief Loads the sinking platform sprites from the datafile. Call once at startup. */
void load_sinking_spritesheet();

/** @brief Resets all sinking slots to inactive. Call before loading a new level. */
void reset_sinking();

/**
 * @brief Parses the TMX data for the given level and populates the sinking pool.
 * @param level_id The current level identifier (used to look up TMX data).
 */
void load_level_sinking(int level_id);

/** @brief Advances the appear/crumble/gone state machine. Call once per game tick. */
void sinking_update(int scroll_x);

/**
 * @brief Draws all visible sinking platforms (solid or crumbling).
 * @param scroll_x The current horizontal scroll offset.
 */
void sinking_draw(int scroll_x);

/**
 * @brief Fills the provided array with AABB boxes for platforms that are currently
 *        solid (standable). Non-solid platforms get a zero-sized box.
 * @param boxes Array of at least MAX_SINKING collisionType elements.
 */
void sinking_get_all_aabb(collisionType* boxes);

/**
 * @brief Finds a solid sinking platform that supports the provided area from below.
 * @param area Support probe (typically player feet area).
 * @param max_snap_pixels Vertical tolerance around the platform top.
 * @param out_index Output supported platform index.
 * @param out_top_y Output top y coordinate of the platform.
 * @return TRUE when support is found, FALSE otherwise.
 */
int sinking_find_support(collisionType area, int max_snap_pixels, int* out_index, int* out_top_y);

#endif
