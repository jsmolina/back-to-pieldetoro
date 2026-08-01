#ifndef PAUSE_H
#define PAUSE_H

/**
 * @file pause.h
 * @brief Pause menu system for the game.
 */

enum MainMenuOption {
    NEW_GAME = 0,
    PASSWORD = 1,
    EXIT_TO_DOS = 2,
};

/**
 * @brief Shows the pause menu and handles input.
 * @return Selected menu option (PAUSE_CONTINUE, PAUSE_MENU, or PAUSE_EXIT_TO_DOS).
 */
enum MainMenuOption show_pause_menu(const char* passcode);

#endif
