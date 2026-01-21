
#ifndef HELPERS_H
#define HELPERS_H
/**
 * @brief Emits a beep sound at the specified frequency and duration.
 *
 * @param frequency The frequency of the beep in Hertz (Hz).
 * @param duration The duration of the beep in milliseconds (ms).
 */
void beep(int frequency, int duration);

/** Waits for the spacebar key to be pressed and released. */
void wait_for_space();
/** Prints text at a specific position on screen. */
void print_at(int x, int y, char* texto, int col);

#endif