#ifndef BOSS_H
#define BOSS_H

#include "helpers.h"

/**
 * @file boss.h
 * @brief Level-7 boss: walks in, then fires oscillating wave
 *        projectiles at the player. 
 */

#define BOSS_MAX_WAVES 24 // active wave-projectile instances pool

/** @brief Loads the boss and wave sprites from the datafile. Call once at startup. */
void load_boss_spritesheet();

/** @brief Resets the boss and its projectiles for the stage (call on level load/restart). */
void reset_boss();

/** @brief Spawns the boss at its start position and begins the walk-in. */
void spawn_boss();

/** @brief Advances the boss state machine and wave projectiles. Once per tick. */
void boss_update(int scroll_x);

/** @brief Draws the boss, its wave trail and the HP bar. */
void boss_draw(int scroll_x);

/** @brief TRUE if the boss is currently active (spawned and not finished dying). */
int boss_is_active();

/** @brief Fills the boss body AABB (zero-sized when inactive/invulnerable-agnostic). */
void boss_get_aabb(collisionType* box);

/** @brief Fills AABB boxes for all active wave projectiles (for player collision). */
void boss_wave_get_all_aabb(collisionType* boxes);

/** @brief Applies one hit to the boss, honoring the invulnerability cooldown. */
int boss_on_hit();

#endif
