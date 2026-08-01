#ifndef PASSCODE_H
#define PASSCODE_H
/**
 * @file passcode.h
 * @brief Functions to generate and load game passcodes.
 */

/**
 * @brief Generate a game passcode.
 *
 * @param level The current level.
 * @param lifes The number of lifes.
 * @param money The amount of money.
 * @param score The current score.
 * @param out_pass Buffer to store the generated passcode (must be at least 11 characters).
 */
void generate_pass(int level, int lifes, int money, int score, char* out_pass);

/**
 * @brief Load and validate a game passcode.
 *
 * @param pass The passcode string to load.
 * @param level Pointer to store the loaded level.
 * @param lifes Pointer to store the loaded number of lifes.
 * @param money Pointer to store the loaded amount of money.
 * @param score Pointer to store the loaded score.
 * @return TRUE if the passcode is valid, FALSE otherwise.
 */
int load_pass(const char* pass, int *level, int *lifes, int *money, int *score);

#define PASSCODE_LENGTH 10

#endif
