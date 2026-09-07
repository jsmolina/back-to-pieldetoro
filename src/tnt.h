#ifndef TNT_H
#define TNT_H

#include "helpers.h"

#define MAX_TNT 10 // level actually has 10 tnt
#define MAX_TNT_CARRY 2   // TNT the player can carry at once
#define MAX_TNT_PER_BOX 2 // TNT a single BOX can hold

/**
 * @file tnt.h
 * @brief Level-6 TNT objects: collectible TNT and BOX targets.
 *
 * TNT (TMX name="TNT") is collected by walking into it (AABB), up to
 * MAX_TNT_CARRY. A BOX (TMX name="BOX") is a door-style interaction target:
 * pressing Space while overlapping it places carried TNT on the box, up to
 * MAX_TNT_PER_BOX. Modelled on piece.c, with the BOX interaction copied from
 * door.c. Only used when level == 6.
 */

/** @brief Loads the TNT sprite from datafile. Call once at startup. */
void load_tnt_spritesheet();

/** @brief Resets TNT/BOX slots and the player's carried TNT for the stage. */
void reset_tnt();

/** @brief Loads level TNT and BOX objects from TMX (names "TNT" and "BOX"). */
void load_level_tnt(int level_id);

/** @brief Draws world TNT, placed BOX TNT, and the carried-TNT inventory. */
void tnt_draw(int scroll_x);

/** @brief Fills AABB boxes for uncollected world TNT (for player collision). */
void tnt_get_all_aabb(collisionType* boxes);

/** @brief Collects world TNT at index if the player has room to carry it. */
void tnt_on_collect(int index);

/** @brief Fills AABB boxes for all active BOX targets (for the Space interaction). */
void tnt_box_get_all_aabb(collisionType* boxes);

/**
 * @brief Places carried TNT onto the BOX at index, up to MAX_TNT_PER_BOX,
 *        consuming it from the player's inventory. No-op if the box is full or
 *        the player has none.
 */
void tnt_place_on_box(int index);

#endif
