
#ifndef GAME_H
#define GAME_H
#define LEVEL_MOUNTAIN 4
#define LEVEL_CITY 5
#define LEVEL_AUTOVOICE 6
#define LEVEL_BOSS 7
#include <allegro.h>

#define COIN_MONEY_VALUE 5

enum PauseMenuResult {
    PAUSE_RESULT_CONTINUE = 0,
    PAUSE_RESULT_RESTART = 1,
    PAUSE_RESULT_EXIT = 2,
};

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

// extern BITMAP *scroller;
extern BITMAP* current_background;
extern int game_pause;
extern int GROUND_Y;
extern int score;
extern int stage_elapsed_minutes;
extern int stage_elapsed_seconds;
extern int megahit_mode; // flag to enable megahit mode for testing

/**
 * @brief Initializes a new game session
 *
 * Resets all game variables, loads initial assets, and prepares the game state
 * for the start of gameplay.
 */
void start_new_game();
void continue_game(int cl, int liv, int mon, int sco, int books);

/** @brief Adds one collected coin reward to game economy counters. */
void game_on_coin_collected();

/** @brief Handles pause menu display and returns the selected action. */
enum PauseMenuResult game_handle_pause(void);

/** @brief Returns collected coin count in the current run. */
int game_get_coins_collected();

/** @brief Returns current money in the current run. */
int game_get_money();

/** @brief Tries to spend money and returns TRUE if it was possible. */
int game_try_spend_money(int amount);

/**
 * @brief Updates game logic for the current frame
 *
 * Processes input, updates entity positions, collision detection, and game state.
 * Should be called once per game loop iteration.
 *
 * @note This is an inline function for performance optimization
 */
inline int update_game();

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
