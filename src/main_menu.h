#ifndef MAIN_MENU_H
#define MAIN_MENU_H

/**
 * @file pause.h
 * @brief Pause menu system for the game.
 */

typedef enum  {
    NEW_GAME = 0,
    PASSWORD = 1,
    EXIT_TO_DOS = 2,
} MainMenuOption;

/**
 * @brief Shows the menu and handles input.
 * @return Selected menu option 
 */
MainMenuOption show_main_menu();

#endif
