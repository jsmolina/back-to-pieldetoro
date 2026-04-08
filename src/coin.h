#ifndef COIN_H
#define COIN_H

#include "helpers.h"

#define MAX_COINS 20

/**
 * @file coin.h
 * @brief Header file for collectible coin objects in the game.
 *
 * Coins are placed as TMX objects (name="COIN", type="O") and collected
 * when the player walks over them (AABB collision).
 */

/** @brief Loads the coin sprite from the datafile. Call once at startup. */
void load_coin_spritesheet();

/** @brief Resets all coin slots to inactive/uncollected. Call before loading a new level. */
void reset_coins();

/**
 * @brief Parses the TMX data for the given level and populates the coin pool.
 * @param level_id The current level identifier (used to look up TMX data).
 */
void load_level_coins(int level_id);

/**
 * @brief Draws all active, uncollected coins that are within the visible screen area.
 * @param scroll_x The current horizontal scroll offset.
 */
void draw_coins(int scroll_x);

/**
 * @brief Fills the provided array with AABB boxes for all active, uncollected coins.
 *        Inactive or collected coins get a zero-sized box.
 * @param boxes Array of at least MAX_COINS collisionType elements.
 */
void coin_get_all_aabb(collisionType* boxes);

/**
 * @brief Marks the coin at the given index as collected.
 * @param index Index into the coin pool (0 .. MAX_COINS-1).
 */
void coin_on_collect(int index);

/**
 * @brief Returns the total number of coins collected so far.
 * @return Number of collected coins.
 */
int get_coins_collected();

#endif
