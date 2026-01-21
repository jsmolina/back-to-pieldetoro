
#ifndef GAME_H
#define GAME_H
#include <allegro.h>

/**
 * @file game.h
 * @brief Core game state and rendering management header
 * 
 * This header defines the main game loop interface and external game state variables.
 * It provides functions for initializing, updating, and rendering the game, as well as
 * managing game memory and pause state.
 * 
 * @details
 * - SCREEN_VIRTUAL: Virtual screen width constant (640 pixels)
 * - scroller: Bitmap used for scrolling background effects
 * - current_background: Bitmap for the currently displayed background
 * - game_pause: Flag indicating whether the game is paused (non-zero = paused)
 * 
 * @see Allegro library documentation for BITMAP type
 */

//extern BITMAP *scroller;
extern BITMAP *current_background;
extern int game_pause;
extern int GROUND_Y;

/**
 * @brief Initializes a new game session
 * 
 * Resets all game variables, loads initial assets, and prepares the game state
 * for the start of gameplay.
 */
void start_new_game();

/**
 * @brief Updates game logic for the current frame
 * 
 * Processes input, updates entity positions, collision detection, and game state.
 * Should be called once per game loop iteration.
 * 
 * @note This is an inline function for performance optimization
 */
inline void update_game();

/**
 * @brief Renders the current game frame
 * 
 * Draws all game entities, backgrounds, and UI elements to the screen buffer.
 * Should be called once per game loop iteration after update_game().
 * 
 * @note This is an inline function for performance optimization
 */
inline void draw_game();


/**
 * @brief Releases all allocated game memory
 * 
 * Frees bitmaps, textures, and other dynamically allocated resources.
 * Should be called before program termination or when unloading the game.
 */
void unload_game_memory();

#endif
