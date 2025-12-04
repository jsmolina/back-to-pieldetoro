#ifndef ERRORS_H
#define ERRORS_H
/**
 * @brief Terminates the program with an error message
 * 
 * Prints a formatted error message to standard error and exits the program
 * with a failure status code. This function uses printf-style formatting.
 * 
 * @param format A printf-style format string describing the error message
 * @param ... Variable arguments corresponding to the format string specifiers
 * 
 * @note This function does not return - it terminates program execution
 * 
 * @example
 * die("Failed to open file: %s", filename);
 */
void die(const char *format, ...);

#endif
