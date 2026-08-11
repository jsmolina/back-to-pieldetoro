#ifndef PAUSE_H
#define PAUSE_H

/**
 * @file pause.h
 * @brief Pause menu system for the game.
 */

enum PauseMenuOption {
    PCONTINUE = 0,
    PMENU = 1,
    PEXIT_TO_DOS = 2,
};

/**
 * @brief Shows the pause menu and handles input.
 * @return Selected menu option (PAUSE_CONTINUE, PAUSE_MENU, or PAUSE_EXIT_TO_DOS).
 */
enum PauseMenuOption show_pause_menu(const char* passcode);

#endif
