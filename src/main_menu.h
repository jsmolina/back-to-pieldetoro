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

typedef struct {
    MainMenuOption selected;
    int lives;
    int current_level;
    int money; 
    int score;
    int books;
} MainMenuResult;

/**
 * @brief Shows the menu and handles input.
 * @return MainMenuResult with menu option 
 */
MainMenuResult show_main_menu();

#endif
