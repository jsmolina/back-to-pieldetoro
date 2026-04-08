
#ifndef HELPERS_H
#define HELPERS_H

#define FINISHED 1
#define NOT_FINISHED 0

// helpers define common types and functions used across multiple modules, such as player, enemy, and game management.
#include <allegro.h>

// this is the maximum number of frames an animation can have, not the total number of frames across all animations
#define MAX_FRAMES 13

/** @struct animeItem
 * @brief Represents an animation item with frame information
 * @var animeItem::move_count - Number of moves for the animation
 * @var animeItem::frames - Array of frame indices
 * @var animeItem::length - Length of the animation
 * @var animeItem::frame_interval - Interval between frames
 */
typedef struct {
    int move_count;
    int frames[MAX_FRAMES];
    uint8_t length;
    int frame_interval;
} animeItem;

typedef struct {
    int x;
    int y;
} coordsType;


 /**
 * @struct collisionType
 * @brief Represents a collision bounding box with position and dimensions
 * @var collisionType::x - X coordinate of the collision box
 * @var collisionType::y - Y coordinate of the collision box
 * @var collisionType::w - Width of the collision box
 * @var collisionType::h - Height of the collision box
 */
typedef struct {
    int x;
    int y;
    int w;
    int h;
} collisionType;

/**
 * @brief Emits a beep sound at the specified frequency and duration.
 *
 * @param frequency The frequency of the beep in Hertz (Hz).
 * @param duration The duration of the beep in milliseconds (ms).
 */
void beep(int frequency, int duration);

/** @brief Waits for the spacebar key to be pressed and released. */
void wait_for_space();
int space_key_freed();
/** @brief Prints text at a specific position on screen. */
void print_at(int x, int y, char* texto, int col);

/** @brief Converts a level number to its corresponding data ID. */
int level_to_dat_id(int level);

#endif